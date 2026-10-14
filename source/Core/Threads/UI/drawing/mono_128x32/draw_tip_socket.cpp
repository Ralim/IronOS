#include "ui_drawing.hpp"
#if defined(OLED_128x32) && defined(DUAL_TIP_SOCKETS)
// Name of the tip socket in use ("T12" / "TS") at the cursor, in the 6x8 font
void ui_draw_tip_socket(void) { OLED::print(getActiveTipChannel() ? SmallSymbolSocketTS : SmallSymbolSocketT12, FontStyle::TINY); }
#endif
