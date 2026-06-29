/*
 * ColorTheme.hpp
 *
 *  Alientek T90 (Nations N32L40x) color theming for the GC9 RGB565 panel.
 *
 *  IronOS renders a 1bpp mono UI; the GC9 blit (GC9Display::Transmit) is the
 *  only place a mono bit becomes a 16-bit color. This module turns the live
 *  iron state (tip temperature, PID setpoint, heater duty, tip-present) into a
 *  per-frame color scheme: the foreground is heat-mapped (cold blue -> hot red)
 *  and a thin power bar runs along the bottom edge. Stage 1 keeps the IronOS
 *  mono layout untouched and only colorizes it; later stages can grow native
 *  color screens here.
 */

#ifndef BSP_ALIENTEK_T90_COLORTHEME_HPP_
#define BSP_ALIENTEK_T90_COLORTHEME_HPP_

#include "configuration.h"
#ifdef OLED_GC9D01

#include <stdint.h>

// Pack 8-8-8 RGB into RGB565 (constexpr so palette tables are link-time constants).
constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) { return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)); }

// Height (in landscape rows = native panel columns) of the bottom power bar.
// The blit derives the band's first column as GC9_PANEL_W - GC9_HEATBAR_H.
#define GC9_HEATBAR_H 4

// Per-frame color scheme derived from live iron state. Computed once per refresh.
struct FrameTheme {
  uint16_t fg;      // foreground color for lit (set) UI pixels
  uint16_t bg;      // background color for clear pixels
  uint16_t barTrack; // unlit portion of the power bar
  bool     heatBar; // draw the bottom power bar this frame
  uint8_t  heatPct; // 0..100 heater duty, for the bar fill length
};

namespace T90Theme {
// Read live state and build the frame theme (also advances the internal flash tick).
FrameTheme computeFrame();
// Heat ramp by tip temperature in degC: icy blue -> cyan -> green -> amber -> red.
uint16_t heatColorForTempC(int32_t degC);
// Power-bar ramp by position along the bar (0..255): green -> yellow -> red.
uint16_t heatBarColorAt(uint8_t pos);
} // namespace T90Theme

#endif // OLED_GC9D01
#endif /* BSP_ALIENTEK_T90_COLORTHEME_HPP_ */
