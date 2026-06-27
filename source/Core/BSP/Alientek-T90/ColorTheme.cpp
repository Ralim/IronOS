/*
 * ColorTheme.cpp
 *
 *  Live-state -> color scheme for the Alientek T90 GC9 panel (see ColorTheme.hpp).
 */

#include "ColorTheme.hpp"

#ifdef OLED_GC9D01

#include "BSP.h"            // powerPWM, isTipDisconnected
#include "IRQ.h"            // pendingPWM (live heater duty 0..powerPWM)
#include "TipThermoModel.h" // TipThermoModel::getTipInC
#include "main.hpp"         // currentTempTargetDegC (active PID setpoint, always degC)

namespace {

// Linearly interpolate two RGB565 colors in component space: a + (b-a)*num/den.
uint16_t lerp565(uint16_t a, uint16_t b, int num, int den) {
  if (den <= 0) {
    return a;
  }
  if (num < 0) {
    num = 0;
  }
  if (num > den) {
    num = den;
  }
  const int ar = (a >> 11) & 0x1F, ag = (a >> 5) & 0x3F, ab = a & 0x1F;
  const int br = (b >> 11) & 0x1F, bg = (b >> 5) & 0x3F, bb = b & 0x1F;
  const int r  = ar + (br - ar) * num / den;
  const int g  = ag + (bg - ag) * num / den;
  const int bl = ab + (bb - ab) * num / den;
  return (uint16_t)((r << 11) | (g << 5) | bl);
}

// Heat ramp keyed on tip temperature (degC). Cold reads icy blue, hot reads red.
struct HeatStop {
  int16_t  t;
  uint16_t c;
};
constexpr HeatStop kHeat[] = {
    { 40, rgb565(170, 210, 255)}, // icy blue-white (cold / idle-cool)
    {130,   rgb565(0, 255, 255)}, // cyan
    {210,  rgb565(40, 230,  60)}, // green
    {290, rgb565(255, 225,   0)}, // yellow
    {350, rgb565(255, 140,   0)}, // orange
    {430, rgb565(255,  30,  20)}, // red (near tip max)
};
constexpr int kHeatStops = (int)(sizeof(kHeat) / sizeof(kHeat[0]));

// Power-bar ramp keyed on position along the bar (0..255). Green -> yellow -> red.
constexpr uint16_t kBarLo  = rgb565(30, 220, 40);
constexpr uint16_t kBarMid = rgb565(255, 210, 0);
constexpr uint16_t kBarHi  = rgb565(255, 40, 20);

// Static accents.
constexpr uint16_t kIdleCool     = rgb565(220, 232, 255); // cool near-white for cold idle / menus
constexpr uint16_t kDisconnected = rgb565(120, 132, 145); // gray when no tip
constexpr uint16_t kOverheat     = rgb565(255, 255, 255); // flash highlight when dangerously hot
constexpr uint16_t kBackground   = rgb565(0, 0, 0);       // black
constexpr uint16_t kBarTrack     = rgb565(26, 30, 36);    // faint unlit bar track

// Tip temperature considered "still hot" for cooling display when the heater is off.
constexpr int32_t kHotTipC = 55;
// Tip temperature at which the foreground flashes as an overheat warning.
constexpr int32_t kOverheatC = 440;

// Per-frame tick used to animate the overheat flash.
uint16_t flashTick = 0;

} // namespace

uint16_t T90Theme::heatColorForTempC(int32_t degC) {
  if (degC <= kHeat[0].t) {
    return kHeat[0].c;
  }
  if (degC >= kHeat[kHeatStops - 1].t) {
    return kHeat[kHeatStops - 1].c;
  }
  for (int i = 1; i < kHeatStops; i++) {
    if (degC < kHeat[i].t) {
      const int span = kHeat[i].t - kHeat[i - 1].t;
      return lerp565(kHeat[i - 1].c, kHeat[i].c, (int)(degC - kHeat[i - 1].t), span);
    }
  }
  return kHeat[kHeatStops - 1].c;
}

uint16_t T90Theme::heatBarColorAt(uint8_t pos) {
  if (pos < 128) {
    return lerp565(kBarLo, kBarMid, pos, 127);
  }
  return lerp565(kBarMid, kBarHi, pos - 128, 127);
}

FrameTheme T90Theme::computeFrame() {
  flashTick++;

  FrameTheme th;
  th.bg       = kBackground;
  th.barTrack = kBarTrack;
  th.heatBar  = false;
  th.heatPct  = 0;

  const int32_t tip    = TipThermoModel::getTipInC(false); // cached, no forced ADC sample
  const int32_t target = currentTempTargetDegC;            // 0 when the heater is off

  if (isTipDisconnected()) {
    th.fg = kDisconnected;
    return th;
  }

  // Heater duty 0..powerPWM -> 0..100 percent.
  const uint16_t denom = powerPWM ? powerPWM : 255;
  uint16_t       pct   = (uint16_t)((uint32_t)pendingPWM * 100u / denom);
  if (pct > 100) {
    pct = 100;
  }

  if (target > 0) {
    // Actively heating (soldering / sleep / profile): heat-map the foreground and show the bar.
    th.fg      = heatColorForTempC(tip);
    th.heatBar = true;
    th.heatPct = (uint8_t)pct;
    if (tip >= kOverheatC && (flashTick & 0x8)) {
      th.fg = kOverheat;
    }
  } else if (tip > kHotTipC) {
    // Idle but the tip is still hot (cooling readout on the home screen): keep the heat color.
    th.fg = heatColorForTempC(tip);
  } else {
    // Cold idle / menus / settings: clean cool near-white.
    th.fg = kIdleCool;
  }

  return th;
}

#endif // OLED_GC9D01
