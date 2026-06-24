/*
 * GC9Display.cpp
 *
 *  Alientek T90 (Nations N32L40x) GC9-family color LCD over hardware SPI1.
 *
 *  Re-implementation against the Nations std-periph driver (no ST HAL). The
 *  mono 1bpp 128x32 IronOS framebuffer is expanded to RGB565 and blitted, with
 *  a 90-degree software transpose and centering, into the native 40x160
 *  portrait panel. Init sequence and pinmap are from the verified RE dossier
 *  (t90b-gc9d01-pinmap-re.md).
 */

#include "GC9Display.hpp"

#ifdef OLED_GC9D01

#include "BSP.h"  // delay_ms
#include "OLED.hpp" // FRAMEBUFFER_START, OLED_WIDTH, OLED_HEIGHT
#include "Pins.h" // LCD_* pin macros + n32l40x.h (pulls in the std-periph drivers)

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

// 1bpp pixel payload length pushed on a full frame refresh:
// FRAMEBUFFER_START(17) + OLED_WIDTH(128) * (OLED_HEIGHT(32)/8) = 17 + 512 = 529.
#define GC9_FULLFRAME_SIZE (FRAMEBUFFER_START + (OLED_WIDTH * (OLED_HEIGHT / 8)))
// setDisplayState pushes only the 16-byte command header (FRAMEBUFFER_START - 1).
#define GC9_STATE_SIZE (FRAMEBUFFER_START - 1)

// GC9 command opcodes used by the shim.
#define GC9_CMD_INVOFF 0x20
#define GC9_CMD_INVON  0x21
#define GC9_CMD_CASET  0x2A
#define GC9_CMD_RASET  0x2B
#define GC9_CMD_RAMWR  0x2C
#define GC9_CMD_DISPOFF 0x28
#define GC9_CMD_DISPON  0x29
#define GC9_CMD_SLPOUT  0x11
#define GC9_CMD_COLMOD  0x3A
#define GC9_CMD_MADCTL  0x36

// SSD1306 on/off opcodes the Core writes into the header (screenBuffer[1]).
#define SSD1306_DISPLAY_ON  0xAF
#define SSD1306_DISPLAY_OFF 0xAE
// SSD1306 inverse/normal opcodes (setInverseDisplay).
#define SSD1306_INVERSE 0xA7
#define SSD1306_NORMAL  0xA6

bool GC9Display::initialised = false;

// No full-frame RGB565 buffer: 40*160*2 = 12.8 KB would not fit the 24 KB SRAM alongside the RTOS
// heap. Pixels are computed from the 1bpp framebuffer and streamed to the panel on the fly.

// ---------------------------------------------------------------------------
// Low-level SPI1 helpers (polled, blocking; Mode 3, half-duplex TX-only)
// ---------------------------------------------------------------------------

// Bounded spin so a wedged SPI peripheral can never permanently block the GUI thread (a hang here
// would otherwise stall every display refresh until the watchdog resets). ~100k iterations is far
// longer than any real byte time at PCLK/2 yet bounded.
#define GC9_SPI_TIMEOUT 100000U

// Wait until the TX register is empty and the bus is idle, then change DFF.
// DFF must only be toggled while SPI is not busy (silicon requirement).
static inline void gc9_wait_idle(void) {
  uint32_t to = GC9_SPI_TIMEOUT;
  while (SPI_I2S_GetStatus(SPI1, SPI_I2S_TE_FLAG) == RESET && --to) {}
  to = GC9_SPI_TIMEOUT;
  while (SPI_I2S_GetStatus(SPI1, SPI_I2S_BUSY_FLAG) != RESET && --to) {}
}

static inline void gc9_set_datalen(uint16_t len) {
  gc9_wait_idle();
  SPI_ConfigDataLen(SPI1, len);
}

// Push one byte (assumes 8-bit DFF). Caller frames CS/DC.
static inline void gc9_spi_byte(uint8_t b) {
  uint32_t to = GC9_SPI_TIMEOUT;
  while (SPI_I2S_GetStatus(SPI1, SPI_I2S_TE_FLAG) == RESET && --to) {}
  SPI_I2S_TransmitData(SPI1, b);
}

// Push one 16-bit word (assumes 16-bit DFF). Caller frames CS.
static inline void gc9_spi_word(uint16_t w) {
  uint32_t to = GC9_SPI_TIMEOUT;
  while (SPI_I2S_GetStatus(SPI1, SPI_I2S_TE_FLAG) == RESET && --to) {}
  SPI_I2S_TransmitData(SPI1, w);
}

// Write a command byte: DC low, CS low, send (8-bit), CS high, DC high.
static void gc9_cmd(uint8_t cmd) {
  GPIO_ResetBits(LCD_DC_GPIO_Port, LCD_DC_Pin); // command
  GPIO_ResetBits(LCD_CS_GPIO_Port, LCD_CS_Pin); // select
  gc9_spi_byte(cmd);
  gc9_wait_idle();
  GPIO_SetBits(LCD_CS_GPIO_Port, LCD_CS_Pin);
  GPIO_SetBits(LCD_DC_GPIO_Port, LCD_DC_Pin); // back to data idle
}

// Write a data byte: DC high, CS low, send (8-bit), CS high.
static void gc9_data(uint8_t d) {
  GPIO_SetBits(LCD_DC_GPIO_Port, LCD_DC_Pin); // data
  GPIO_ResetBits(LCD_CS_GPIO_Port, LCD_CS_Pin);
  gc9_spi_byte(d);
  gc9_wait_idle();
  GPIO_SetBits(LCD_CS_GPIO_Port, LCD_CS_Pin);
}

// Write a 16-bit data value big-endian (hi byte then lo byte), 8-bit DFF.
static void gc9_data16(uint16_t v) {
  gc9_data((uint8_t)(v >> 8));
  gc9_data((uint8_t)(v & 0xFF));
}

// Send a command followed by N data bytes (init stream helper).
static void gc9_cmd_data(uint8_t cmd, const uint8_t *data, uint8_t n) {
  gc9_cmd(cmd);
  for (uint8_t i = 0; i < n; i++) {
    gc9_data(data[i]);
  }
}

// ---------------------------------------------------------------------------
// GC9 init command stream (verified vendor table, RE dossier section (a))
// ---------------------------------------------------------------------------

static void gc9_run_init_sequence(void) {
  // Inner register enable (GC9-family software unlock).
  gc9_cmd(0xFE);
  gc9_cmd(0xEF);

  // Inter-register page-enable block: 0x80..0x8F each = 0xFF.
  for (uint8_t reg = 0x80; reg <= 0x8F; reg++) {
    static const uint8_t ff = 0xFF;
    gc9_cmd_data(reg, &ff, 1);
  }

  // Pixel format = 16 bpp RGB565 (mandatory, matches the RGB565 blit).
  {
    static const uint8_t d[] = {0x05};
    gc9_cmd_data(GC9_CMD_COLMOD, d, sizeof(d));
  }

  // Vendor charge-pump / timing.
  { static const uint8_t d[] = {0x11};       gc9_cmd_data(0xEC, d, sizeof(d)); }
  { static const uint8_t d[] = {0x7A};       gc9_cmd_data(0x7E, d, sizeof(d)); }
  { static const uint8_t d[] = {0x02, 0x0E, 0x00, 0x00, 0x28, 0x00, 0x00}; gc9_cmd_data(0x74, d, sizeof(d)); }
  { static const uint8_t d[] = {0x3E};       gc9_cmd_data(0x98, d, sizeof(d)); }
  { static const uint8_t d[] = {0x3E};       gc9_cmd_data(0x99, d, sizeof(d)); }
  { static const uint8_t d[] = {0x0E, 0x0E}; gc9_cmd_data(0xB5, d, sizeof(d)); }

  // Gate / source timing.
  { static const uint8_t d[] = {0x38, 0x09, 0x6D, 0x67};             gc9_cmd_data(0x60, d, sizeof(d)); }
  { static const uint8_t d[] = {0x38, 0xAD, 0x6D, 0x67, 0x05};       gc9_cmd_data(0x63, d, sizeof(d)); }
  { static const uint8_t d[] = {0x38, 0x0B, 0x70, 0xAB, 0x6D, 0x67}; gc9_cmd_data(0x64, d, sizeof(d)); }
  { static const uint8_t d[] = {0x38, 0x0F, 0x70, 0xAF, 0x6D, 0x67}; gc9_cmd_data(0x66, d, sizeof(d)); }
  { static const uint8_t d[] = {0x00, 0x00};                          gc9_cmd_data(0x6A, d, sizeof(d)); }
  { static const uint8_t d[] = {0x3B, 0x08, 0x04, 0x00, 0x04, 0x64, 0x67}; gc9_cmd_data(0x68, d, sizeof(d)); }
  { static const uint8_t d[] = {0x22, 0x02, 0x22, 0x02, 0x22, 0x22, 0x50}; gc9_cmd_data(0x6C, d, sizeof(d)); }

  // Gamma / gate LUT (32 bytes).
  {
    static const uint8_t d[] = {0x00, 0x00, 0x00, 0x00, 0x07, 0x01, 0x13, 0x11, 0x0B, 0x09, 0x16, 0x15, 0x1D, 0x1E, 0x00, 0x00,
                                0x00, 0x00, 0x1E, 0x1D, 0x15, 0x16, 0x0A, 0x0C, 0x12, 0x14, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00};
    gc9_cmd_data(0x6E, d, sizeof(d));
  }

  // Power / VREG.
  { static const uint8_t d[] = {0x1B}; gc9_cmd_data(0xA9, d, sizeof(d)); }
  { static const uint8_t d[] = {0x6B}; gc9_cmd_data(0xA8, d, sizeof(d)); }
  { static const uint8_t d[] = {0x6D}; gc9_cmd_data(0xA8, d, sizeof(d)); } // re-write
  { static const uint8_t d[] = {0x40}; gc9_cmd_data(0xA7, d, sizeof(d)); }
  { static const uint8_t d[] = {0x47}; gc9_cmd_data(0xAD, d, sizeof(d)); }
  { static const uint8_t d[] = {0x73}; gc9_cmd_data(0xAF, d, sizeof(d)); }
  { static const uint8_t d[] = {0x73}; gc9_cmd_data(0xAF, d, sizeof(d)); } // re-write
  { static const uint8_t d[] = {0x44}; gc9_cmd_data(0xAC, d, sizeof(d)); }
  { static const uint8_t d[] = {0x6C}; gc9_cmd_data(0xA3, d, sizeof(d)); }
  { static const uint8_t d[] = {0x00}; gc9_cmd_data(0xCB, d, sizeof(d)); }
  { static const uint8_t d[] = {0x22}; gc9_cmd_data(0xCD, d, sizeof(d)); }
  { static const uint8_t d[] = {0x10}; gc9_cmd_data(0xC2, d, sizeof(d)); }
  { static const uint8_t d[] = {0x00}; gc9_cmd_data(0xC5, d, sizeof(d)); }
  { static const uint8_t d[] = {0x0E}; gc9_cmd_data(0xC6, d, sizeof(d)); }
  { static const uint8_t d[] = {0x1F}; gc9_cmd_data(0xC7, d, sizeof(d)); }
  { static const uint8_t d[] = {0x0E}; gc9_cmd_data(0xC8, d, sizeof(d)); }
  { static const uint8_t d[] = {0x00}; gc9_cmd_data(0xBF, d, sizeof(d)); }
  { static const uint8_t d[] = {0x20}; gc9_cmd_data(0xF9, d, sizeof(d)); }
  { static const uint8_t d[] = {0x3B}; gc9_cmd_data(0x9B, d, sizeof(d)); }
  { static const uint8_t d[] = {0x33, 0x7F, 0x00}; gc9_cmd_data(0x93, d, sizeof(d)); }
  { static const uint8_t d[] = {0x0E, 0x0F, 0x03, 0x0E, 0x0F, 0x03}; gc9_cmd_data(0x70, d, sizeof(d)); }
  { static const uint8_t d[] = {0x0E, 0x16, 0x03}; gc9_cmd_data(0x71, d, sizeof(d)); }
  { static const uint8_t d[] = {0x0E, 0x09}; gc9_cmd_data(0x91, d, sizeof(d)); }
  { static const uint8_t d[] = {0x2C}; gc9_cmd_data(0xC3, d, sizeof(d)); }
  { static const uint8_t d[] = {0x1A}; gc9_cmd_data(0xC4, d, sizeof(d)); }

  // Split gamma.
  { static const uint8_t d[] = {0x51, 0x13, 0x0C, 0x06, 0x00, 0x2F}; gc9_cmd_data(0xF0, d, sizeof(d)); }
  { static const uint8_t d[] = {0x51, 0x13, 0x0C, 0x06, 0x00, 0x33}; gc9_cmd_data(0xF2, d, sizeof(d)); }
  { static const uint8_t d[] = {0x3C, 0x94, 0x4F, 0x33, 0x34, 0xCF}; gc9_cmd_data(0xF1, d, sizeof(d)); }
  { static const uint8_t d[] = {0x4D, 0x94, 0x4F, 0x33, 0x34, 0xCF}; gc9_cmd_data(0xF3, d, sizeof(d)); }

  // Orientation default; rotation is done in software (the transpose).
  { static const uint8_t d[] = {0x00}; gc9_cmd_data(GC9_CMD_MADCTL, d, sizeof(d)); }

  // Sleep out + mandatory 200 ms wait, display on, start memory write.
  gc9_cmd(GC9_CMD_SLPOUT);
  delay_ms(200);
  gc9_cmd(GC9_CMD_DISPON);
  gc9_cmd(GC9_CMD_RAMWR);
}

// ---------------------------------------------------------------------------
// GPIO + SPI1 bringup
// ---------------------------------------------------------------------------

static void gc9_init_hw(void) {
  // Clocks: GPIOA/GPIOB/GPIOD + SPI1 all live on APB2.
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_GPIOD | RCC_APB2_PERIPH_SPI1 | RCC_APB2_PERIPH_AFIO, ENABLE);

  GPIO_InitType io;
  GPIO_InitStruct(&io);

  // CS = PA10, BL = PA6, DC = PD0: plain push-pull GPIO outputs.
  io.Pin           = LCD_CS_Pin | LCD_BL_Pin;
  io.GPIO_Mode     = GPIO_Mode_Out_PP;
  io.GPIO_Slew_Rate = GPIO_Slew_Rate_High;
  io.GPIO_Pull     = GPIO_No_Pull;
  io.GPIO_Current  = GPIO_DC_8mA;
  GPIO_InitPeripheral(LCD_CS_GPIO_Port, &io); // PA10, PA6 (same port)

  GPIO_InitStruct(&io);
  io.Pin           = LCD_DC_Pin;
  io.GPIO_Mode     = GPIO_Mode_Out_PP;
  io.GPIO_Slew_Rate = GPIO_Slew_Rate_High;
  io.GPIO_Pull     = GPIO_No_Pull;
  io.GPIO_Current  = GPIO_DC_8mA;
  GPIO_InitPeripheral(LCD_DC_GPIO_Port, &io); // PD0

  // SCK = PB3, MOSI = PB5: SPI1 alternate function.
  GPIO_InitStruct(&io);
  io.Pin            = LCD_SCK_Pin | LCD_MOSI_Pin;
  io.GPIO_Mode      = GPIO_Mode_AF_PP;
  io.GPIO_Alternate = GPIO_AF5_SPI1;
  io.GPIO_Slew_Rate = GPIO_Slew_Rate_High;
  io.GPIO_Pull      = GPIO_No_Pull;
  io.GPIO_Current   = GPIO_DC_8mA;
  GPIO_InitPeripheral(LCD_SCK_GPIO_Port, &io); // PB3, PB5 (same port)

  // Idle levels: CS high (deselected), DC high (data), BL high (backlight off).
  GPIO_SetBits(LCD_CS_GPIO_Port, LCD_CS_Pin);
  GPIO_SetBits(LCD_DC_GPIO_Port, LCD_DC_Pin);
  GPIO_SetBits(LCD_BL_GPIO_Port, LCD_BL_Pin);

  // SPI1: Mode 3, master, software NSS, half-duplex TX-only, baud PCLK/2, MSB.
  // Reproduces the RE-observed CR1 = 0xC307.
  SPI_InitType s;
  SPI_InitStruct(&s);
  s.DataDirection = SPI_DIR_SINGLELINE_TX;
  s.SpiMode       = SPI_MODE_MASTER;
  s.DataLen       = SPI_DATA_SIZE_8BITS;
  s.CLKPOL        = SPI_CLKPOL_HIGH;       // CPOL=1
  s.CLKPHA        = SPI_CLKPHA_SECOND_EDGE; // CPHA=1 -> Mode 3
  s.NSS           = SPI_NSS_SOFT;
  s.BaudRatePres  = SPI_BR_PRESCALER_2;    // fPCLK/2
  s.FirstBit      = SPI_FB_MSB;
  s.CRCPoly       = 7;                       // unused, must be >= 1
  SPI_Init(SPI1, &s);
  SPI_SetNssLevel(SPI1, SPI_NSS_HIGH);      // SSI high, avoid MODF
  SPI_Enable(SPI1, ENABLE);
}

// Open a full-panel pixel write: window 0..39 x 0..159, RAMWR, then assert CS and switch to 16-bit
// pixel framing. Pixels are streamed by the caller (no full-frame buffer).
static void gc9_begin_frame(void) {
  gc9_cmd(GC9_CMD_CASET);
  gc9_data16(0);
  gc9_data16(GC9_PANEL_W - 1);
  gc9_cmd(GC9_CMD_RASET);
  gc9_data16(0);
  gc9_data16(GC9_PANEL_H - 1);
  gc9_cmd(GC9_CMD_RAMWR);
  GPIO_SetBits(LCD_DC_GPIO_Port, LCD_DC_Pin); // data
  GPIO_ResetBits(LCD_CS_GPIO_Port, LCD_CS_Pin);
  gc9_set_datalen(SPI_DATA_SIZE_16BITS);
}

// Close a pixel write: drain the bus, restore 8-bit framing, deselect.
static void gc9_end_frame(void) {
  gc9_wait_idle();
  gc9_set_datalen(SPI_DATA_SIZE_8BITS);
  GPIO_SetBits(LCD_CS_GPIO_Port, LCD_CS_Pin);
}

// Fill the whole panel with the background colour (streamed, no buffer).
static void gc9_clear(void) {
  gc9_begin_frame();
  for (int i = 0; i < GC9_PANEL_W * GC9_PANEL_H; i++) {
    gc9_spi_word(COLOR_BG);
  }
  gc9_end_frame();
}

// ---------------------------------------------------------------------------
// Public lifecycle
// ---------------------------------------------------------------------------

void GC9Display::init() {
  if (initialised) {
    return;
  }
  gc9_init_hw();
  gc9_run_init_sequence();
  gc9_clear();
  // Backlight on (PA6 active-low) once the panel shows background.
  GPIO_ResetBits(LCD_BL_GPIO_Port, LCD_BL_Pin);
  initialised = true;
}

void GC9Display::FRToSInit() { init(); }

// ---------------------------------------------------------------------------
// Transmit - the workhorse
// ---------------------------------------------------------------------------

void GC9Display::Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
  (void)DevAddress;

  // Display state change (setDisplayState): header only, 16 bytes, no pixels.
  if (Size == GC9_STATE_SIZE) {
    if (pData[1] == SSD1306_DISPLAY_ON) {
      gc9_cmd(GC9_CMD_DISPON);
      GPIO_ResetBits(LCD_BL_GPIO_Port, LCD_BL_Pin); // backlight on (active-low)
    } else if (pData[1] == SSD1306_DISPLAY_OFF) {
      gc9_cmd(GC9_CMD_DISPOFF);
      GPIO_SetBits(LCD_BL_GPIO_Port, LCD_BL_Pin); // backlight off, saves power
    }
    return;
  }

  // Anything smaller than a full frame that is not the state header (e.g. the
  // scroll-indicator column-mask blob) is a micro-optimisation; ignore it and
  // let the next refresh() redraw the whole frame.
  if (Size < GC9_FULLFRAME_SIZE) {
    return;
  }

  // Full frame: stream the centered, transposed 128x32 mono UI into the 40x160 RGB565 panel,
  // computing each pixel on the fly (no 12.8 KB frame buffer -> fits the 24 KB SRAM). Panel scan
  // order after RAMWR is row-major with the column incrementing fastest (col 0..39, then row
  // 0..159). The centering transpose maps UI (x,y) -> panel (col = y + Y_OFF, row = 159 - (x +
  // X_OFF)); inverting it per panel (row,col) recovers UI (x,y), and anything outside the 128x32
  // UI window is background.
  const uint8_t *pix = pData + FRAMEBUFFER_START; // 512 bytes of 1bpp, [17..528]
  gc9_begin_frame();
  for (int row = 0; row < GC9_PANEL_H; row++) {
    const int x = (GC9_PANEL_H - 1 - row) - GC9_X_OFF; // UI column (may be out of range)
    for (int col = 0; col < GC9_PANEL_W; col++) {
      const int y     = col - GC9_Y_OFF;               // UI row (may be out of range)
      uint16_t  color = COLOR_BG;
      if (x >= 0 && x < OLED_WIDTH && y >= 0 && y < OLED_HEIGHT) {
        const uint8_t *strip = pix + (y >> 3) * OLED_WIDTH;
        if ((strip[x] >> (y & 7)) & 1) {
          color = COLOR_FG;
        }
      }
      gc9_spi_word(color);
    }
  }
  gc9_end_frame();
}

// ---------------------------------------------------------------------------
// Bulk register write - SSD1306 init stream is meaningless; succeed.
// ---------------------------------------------------------------------------

bool GC9Display::writeRegistersBulk(const uint8_t address, const I2C_REG *registers, const uint8_t registersLength) {
  (void)address;
  (void)registers;
  (void)registersLength;
  return true;
}

// ---------------------------------------------------------------------------
// Single register write - map SSD1306 inverse/normal to GC9 INVON/INVOFF.
// ---------------------------------------------------------------------------

bool GC9Display::I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data) {
  (void)address;
  (void)reg;
  if (data == SSD1306_INVERSE) {
    gc9_cmd(GC9_CMD_INVON);
  } else if (data == SSD1306_NORMAL) {
    gc9_cmd(GC9_CMD_INVOFF);
  }
  return true;
}

#endif // OLED_GC9D01
