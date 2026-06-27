/*
 * GC9Display.hpp
 *
 *  Alientek T90 (Nations N32L40x) GC9-family color LCD over hardware SPI1.
 *
 *  This shim presents the exact static surface of the IronOS I2C backends
 *  (see Drivers/I2C_Wrapper.hpp) so that `#define I2C_CLASS GC9Display` in
 *  OLED.hpp substitutes cleanly. The mono 1bpp 128x32 framebuffer is expanded
 *  to RGB565 and pushed (90-degree software transpose, centered) into the
 *  native 40x160 portrait panel. The first arg `DevAddress` is an I2C relic
 *  and is ignored on every method (there is no address on the SPI bus).
 */

#ifndef BSP_ALIENTEK_T90_GC9DISPLAY_HPP_
#define BSP_ALIENTEK_T90_GC9DISPLAY_HPP_

#include "configuration.h"
#ifdef OLED_GC9D01

#include <stdbool.h>
#include <stdint.h>

// RGB565 colors (mono UI: foreground white on background black).
#define COLOR_FG ((uint16_t)0xFFFF)
#define COLOR_BG ((uint16_t)0x0000)

// Native GC9 panel GRAM geometry (portrait), zero start offset on both axes.
#define GC9_PANEL_W 40  // columns (CASET 0..39)
#define GC9_PANEL_H 160 // rows    (RASET 0..159)

// Landscape canvas is 160x40; the 128x32 mono UI is centered inside it.
#define GC9_X_OFF 16 // (160 - 128) / 2 -> left/right margin in landscape x
#define GC9_Y_OFF 4  // ( 40 -  32) / 2 -> top/bottom margin in landscape y

class GC9Display {
public:
  // Lifecycle: both perform the full (idempotent) panel + SPI + GPIO bringup.
  static void init();
  static void FRToSInit();

  // Workhorse: dispatch on Size (full frame / display-state / ignore). REAL.
  static void Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size);

  // Stage 2 native color path (ColorUI): open a full-panel RGB565 pixel stream, push words in panel
  // scan order (row 0..159 outer, col 0..39 inner), then close it.
  static void colorBegin();
  static void colorPush(uint16_t color);
  static void colorEnd();

  // Map the SSD1306 inverse cmd to GC9 INVON/INVOFF; otherwise succeed. REAL (optional).
  static bool I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data);

  // Compile-only stubs - never reached on the display path (no I2C / no MISO).
  static bool    probe(uint16_t DevAddress) { return true; }
  static bool    wakePart(uint16_t DevAddress) { return true; }
  static bool    Mem_Read(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size) { return false; }
  static bool    Mem_Write(uint16_t DevAddress, uint16_t MemAddress, const uint8_t *pData, uint16_t Size) { return true; }
  static void    Receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size) {}
  static void    TransmitReceive(uint16_t DevAddress, uint8_t *pData_tx, uint16_t Size_tx, uint8_t *pData_rx, uint16_t Size_rx) {}
  static uint8_t I2C_RegisterRead(uint8_t address, uint8_t reg) { return 0; }
  static void    CpltCallback() {} // blocking/polled SPI - no DMA IRQ on first boot

  // Required type: OLED.cpp declares `I2C_CLASS::I2C_REG OLED_Setup_Array[]`.
  // Layout identical to I2CBB2.hpp / I2C_Wrapper.hpp. Declared before
  // writeRegistersBulk so the signature can name it.
  typedef struct {
    const uint8_t reg;      // The register to write to
    uint8_t       val;      // The value to write to this register
    const uint8_t pause_ms; // How many ms to pause _after_ writing this reg
  } I2C_REG;

  // SSD1306 init stream is meaningless to the GC9; always succeed. REAL (no-op true).
  static bool writeRegistersBulk(const uint8_t address, const I2C_REG *registers, const uint8_t registersLength);

private:
  static bool initialised;
};

#endif // OLED_GC9D01
#endif /* BSP_ALIENTEK_T90_GC9DISPLAY_HPP_ */
