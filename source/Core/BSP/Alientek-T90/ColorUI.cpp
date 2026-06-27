/*
 * ColorUI.cpp
 *
 *  Native RGB565 "hero" screens for the Alientek T90 (see ColorUI.hpp).
 */

#include "ColorUI.hpp"

#ifdef OLED_GC9D01

#include "BSP.h"            // getInputVoltageX10, powerPWM
#include "ColorTheme.hpp"   // rgb565, heatColorForTempC, heatBarColorAt
#include "Font.h"           // ExtraFontChars
#include "GC9Display.hpp"   // colorBegin/colorPush/colorEnd + panel geometry
#include "IRQ.h"            // pendingPWM
#include "OLED.hpp"         // OLED::getRotation (left/right-hand orientation)
#include "Settings.h"       // getSettingValue, SettingsOptions
#include "TipThermoModel.h" // TipThermoModel::getTipInC / getTipInF
#include "Translation.h"    // FontSectionInfo (font glyph tables)
#include "main.hpp"         // currentTempTargetDegC, getCurrentOperatingMode
#include "power.hpp"        // x10WattHistory

// Landscape canvas. The native panel is 40(W) x 160(H) portrait; we author 160x40 landscape and the
// panel scan order maps (row,col) -> landscape (x,y) = (GC9_PANEL_H-1-row, col), exactly as the mono
// blit does, so the native screen has the same orientation as the working mono screens.
#define LCD_W GC9_PANEL_H // 160 (landscape width)
#define LCD_H GC9_PANEL_W // 40  (landscape height)

// OperatingMode integer values (mirrors Threads/UI/logic/OperatingModes.h; read via the C accessor
// so the BSP does not depend on the deep UI enum header).
#define MODE_SOLDERING 1

// Smoothed, unit-aware displayed tip temperature (defined in the UI logic, SolderingCommon.cpp).
// Same source the mono soldering screen draws, so the big number is as stable as the mono readout
// (a moving average over FILTER_DISPLAYED_TIP_TEMP samples) rather than the raw per-cycle value.
TemperatureType_t getTipTemp(void);

namespace {

// --- 1bpp glyph bit access (column-major, LSB = top pixel of each 8px strip; matches OLED.cpp) ---
bool fontBit(const uint8_t *font, uint8_t index, uint8_t w, uint8_t h, int gx, int gy) {
  if (gx < 0 || gy < 0 || gx >= w || gy >= h) {
    return false;
  }
  const uint8_t *g = font + ((uint16_t)w * (h / 8)) * index;
  return (g[(gy >> 3) * w + gx] >> (gy & 7)) & 1;
}

// Small-font glyph indices for the non-digit chars we draw (from the Translation.EN.cpp char map;
// digit value N maps to glyph index N in both fonts).
enum : uint8_t { SM_DOT = 45, SM_V = 42, SM_W = 46 };

// A placed glyph: a font table + index, top-left landscape position, integer pixel scale, color.
struct Placement {
  const uint8_t *font;
  uint8_t        index, w, h, scale;
  int16_t        x, y;
  uint16_t       color;
};

Placement g_place[40];
int       g_nplace  = 0;
uint16_t  g_bg      = 0;
bool      g_bar     = false;
int       g_barFill = 0; // lit length of the bottom bar, 0..LCD_W-1

const uint8_t *font12() { return FontSectionInfo.font12_start_ptr; }
const uint8_t *font06() { return FontSectionInfo.font06_start_ptr; }

void put(const uint8_t *font, uint8_t index, uint8_t w, uint8_t h, uint8_t scale, int x, int y, uint16_t color) {
  if (g_nplace < (int)(sizeof(g_place) / sizeof(g_place[0]))) {
    g_place[g_nplace++] = {font, index, w, h, scale, (int16_t)x, (int16_t)y, color};
  }
}

// Large-font unsigned integer (digit glyph index == digit value). Returns the next x.
int putBigNumber(int value, int x, int y, uint8_t scale, uint16_t color) {
  uint8_t digits[5];
  int     n = 0;
  if (value < 0) {
    value = 0;
  }
  do {
    digits[n++] = value % 10;
    value /= 10;
  } while (value > 0 && n < 5);
  for (int i = n - 1; i >= 0; i--) {
    put(font12(), digits[i], 12, 16, scale, x, y, color);
    x += 12 * scale;
  }
  return x;
}

// Small-font unsigned integer. Returns the next x.
int putSmallNumber(int value, int x, int y, uint16_t color) {
  uint8_t digits[5];
  int     n = 0;
  if (value < 0) {
    value = 0;
  }
  do {
    digits[n++] = value % 10;
    value /= 10;
  } while (value > 0 && n < 5);
  for (int i = n - 1; i >= 0; i--) {
    put(font06(), digits[i], 6, 8, 1, x, y, color);
    x += 6;
  }
  return x;
}

void putSmallChar(uint8_t idx, int x, int y, uint16_t color) { put(font06(), idx, 6, 8, 1, x, y, color); }

uint16_t scenePixel(int x, int y) {
  // Bottom power bar band (last 3 landscape rows).
  if (g_bar && y >= LCD_H - 3) {
    if (x <= g_barFill) {
      return T90Theme::heatBarColorAt((uint8_t)((uint32_t)x * 255u / (LCD_W - 1)));
    }
    return rgb565(26, 30, 36); // faint unlit track
  }
  for (int i = 0; i < g_nplace; i++) {
    const Placement &p  = g_place[i];
    const int        pw = p.w * p.scale, ph = p.h * p.scale;
    if (x >= p.x && x < p.x + pw && y >= p.y && y < p.y + ph) {
      if (fontBit(p.font, p.index, p.w, p.h, (x - p.x) / p.scale, (y - p.y) / p.scale)) {
        return p.color;
      }
    }
  }
  return g_bg;
}

void renderSoldering() {
  const bool     inF     = getSettingValue(SettingsOptions::TemperatureInF) != 0;
  const int32_t  tipC    = TipThermoModel::getTipInC(); // Celsius, drives the heat color
  const int32_t  tipShow = getTipTemp();                // smoothed + unit-aware, drives the big number
  const int32_t  setShow = getSettingValue(SettingsOptions::SolderingTemp);
  const uint16_t vX10    = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
  const uint32_t wX10    = x10WattHistory.average(); // deci-watts (W x10)
  const uint16_t heat    = T90Theme::heatColorForTempC(tipC);

  g_nplace = 0;
  g_bg     = rgb565(0, 0, 0);
  g_bar    = false;

  // Big heat-mapped temperature number (2x large font) + degree/unit glyph.
  int x = putBigNumber((int)tipShow, 6, 4, 2, heat);
  put(ExtraFontChars, inF ? 0 : 1, 12, 16, 1, x + 3, 4, heat); // ExtraFontChars: 0=degF, 1=degC

  // Right column: voltage (teal), wattage (white), setpoint (amber).
  const int      rx    = 100;
  const uint16_t teal  = rgb565(0, 200, 210);
  const uint16_t white = rgb565(235, 235, 235);
  const uint16_t amber = rgb565(255, 170, 40);
  {
    int xx = putSmallNumber(vX10 / 10, rx, 3, teal);
    putSmallChar(SM_DOT, xx, 3, teal);
    xx = putSmallNumber(vX10 % 10, xx + 6, 3, teal);
    putSmallChar(SM_V, xx, 3, teal);
  }
  {
    int xx = putSmallNumber(wX10 / 10, rx, 15, white);
    putSmallChar(SM_DOT, xx, 15, white);
    xx = putSmallNumber(wX10 % 10, xx + 6, 15, white);
    putSmallChar(SM_W, xx, 15, white);
  }
  putSmallNumber((int)setShow, rx, 27, amber);

  // Bottom power bar from the live heater duty.
  const uint16_t denom = powerPWM ? powerPWM : 255;
  int            pct   = (int)((uint32_t)pendingPWM * 100u / denom);
  if (pct > 100) {
    pct = 100;
  }
  g_bar     = true;
  g_barFill = (LCD_W - 1) * pct / 100;

  // Stream the scene to the panel (panel scan order: row outer, col inner), honouring the left/right-
  // hand orientation the same way as the mono blit. Use getRawRotation() (the true state): this shim
  // rotates the whole frame, right-hand (false) keeps the original transpose, left-hand is the 180.
  const bool leftHanded = OLED::getRawRotation();
  GC9Display::colorBegin();
  for (int row = 0; row < GC9_PANEL_H; row++) {
    const int lx = leftHanded ? (GC9_PANEL_H - 1 - row) : row;
    for (int col = 0; col < GC9_PANEL_W; col++) {
      const int ly = leftHanded ? col : (GC9_PANEL_W - 1 - col);
      GC9Display::colorPush(scenePixel(lx, ly));
    }
  }
  GC9Display::colorEnd();
}

} // namespace

bool ColorUI::renderActiveScreen() {
  // Only take over the panel for the active soldering screen WITH a tip fitted. For a disconnected tip
  // (and every other mode/screen) fall back to IronOS's own mono framebuffer, so the no-tip icon,
  // warnings, boot logo, menus and the home/sleep screens all render normally via the core pipeline.
  if (getCurrentOperatingMode() == MODE_SOLDERING && !isTipDisconnected()) {
    renderSoldering();
    return true;
  }
  return false;
}

#endif // OLED_GC9D01
