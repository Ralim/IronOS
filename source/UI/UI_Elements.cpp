#include "UI_Elements.h"
#include "FontUtils.h"
#include "OLED.hpp"
#include "UI.h"
void render_Text(const ElementSettings_t *settings, screen_arg_t *args);
void render_Number(const ElementSettings_t *settings, screen_arg_t *args);
void render_Image(const ElementSettings_t *settings, screen_arg_t *args);
void render_PowerSource(const ElementSettings_t *settings, screen_arg_t *args);
void render_Temperature(const ElementSettings_t *settings, screen_arg_t *args);
void render_InputVoltage(const ElementSettings_t *settings, screen_arg_t *args);
void render_ScrollBar(const ElementSettings_t *settings, screen_arg_t *args);
void render_CheckBox(const ElementSettings_t *settings, screen_arg_t *args);
void render_TextScroller(const ElementSettings_t *settings, screen_arg_t *args);

void ui_render_element(const ElementTypes_t element, const ElementSettings_t *settings, screen_arg_t *args) {
  switch (element) {
  case ElementTypes_t::Text:
    render_Text(settings, args);
    break;
  case ElementTypes_t::Number:
    render_Number(settings, args);
    break;
  case ElementTypes_t::Image:
    render_Image(settings, args);
    break;
  case ElementTypes_t::PowerSource:
    render_PowerSource(settings, args);
    break;
  case ElementTypes_t::Temperature:
    render_Temperature(settings, args);
    break;
  case ElementTypes_t::InputVoltage:
    render_InputVoltage(settings, args);
    break;
  case ElementTypes_t::ScrollBar:
    render_ScrollBar(settings, args);
    break;
  case ElementTypes_t::CheckBox:
    render_CheckBox(settings, args);
    break;
  case ElementTypes_t::TextScroller:
    render_TextScroller(settings, args);
    break;
  }
}

void render_Text(const ElementSettings_t *settings, screen_arg_t *args) {
  // Draw text clipped into bounds
  // Args shall be a pointer to an encoded string
  OLED::printBounded((const char *)args->ptr, settings->position.x, settings->position.y, settings->size.w, settings->size.h);
}
void render_Number(const ElementSettings_t *settings, screen_arg_t *args) {
  // Arg is an int32 of the number to display
  OLED::printNumberBounded(args->i32, true, settings->position.x, settings->position.y, settings->size.w, settings->size.h);
}
void render_Image(const ElementSettings_t *settings, screen_arg_t *args) {
  // arg is a pointer to the raw image data to display
  OLED::drawArea(settings->position.x, settings->position.y, settings->size.w, settings->size.h, (uint8_t *)args->ptr);
}
void render_PowerSource(const ElementSettings_t *settings, screen_arg_t *args) {
  //
}
void render_Temperature(const ElementSettings_t *settings, screen_arg_t *args) {
  //
}
void render_InputVoltage(const ElementSettings_t *settings, screen_arg_t *args) {
  //
}
void render_ScrollBar(const ElementSettings_t *settings, screen_arg_t *args) {
  //
}
void render_CheckBox(const ElementSettings_t *settings, screen_arg_t *args) {
  //
}
void render_TextScroller(const ElementSettings_t *settings, screen_arg_t *args) {
  //
}
