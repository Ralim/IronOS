/*
 * ColorUI.hpp
 *
 *  Alientek T90 (GC9 RGB565 panel) native color "hero" screens.
 *
 *  Stage 2 of the color UI: instead of tinting the 1bpp mono framebuffer, selected operating modes
 *  (currently the soldering screen) are drawn natively in RGB565 with a factory-style layout (a big
 *  heat-mapped temperature number, voltage / wattage, and a power bar). The mono GUI logic still
 *  runs (buttons, mode transitions, setpoint); only the visual is replaced. Menus and the other
 *  screens stay on the Stage-1 tinted mono path. Driven from GC9Display::Transmit.
 */

#ifndef BSP_ALIENTEK_T90_COLORUI_HPP_
#define BSP_ALIENTEK_T90_COLORUI_HPP_

#include "configuration.h"
#ifdef OLED_GC9D01

namespace ColorUI {
// If the current operating mode has a native color screen, render it straight to the panel and
// return true (the caller then skips the mono blit). Otherwise return false.
bool renderActiveScreen();
} // namespace ColorUI

#endif // OLED_GC9D01
#endif /* BSP_ALIENTEK_T90_COLORUI_HPP_ */
