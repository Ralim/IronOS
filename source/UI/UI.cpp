#include "UI.h"
#include "OLED.hpp"
void ui_render_screen(screenLayout_t *screen, ScreenContext_t *context) {
  // Walk the struct associated to the screen, calling render for each element of the screen
  // Then start OLED refresh

  OLED::refresh();
}