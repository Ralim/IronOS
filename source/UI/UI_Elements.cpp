#include "UI_Elements.h"

void render_Text(screen_arg_t *args);
void render_Number(screen_arg_t *args);
void render_Image(screen_arg_t *args);
void render_PowerSource(screen_arg_t *args);
void render_Temperature(screen_arg_t *args);
void render_InputVoltage(screen_arg_t *args);
void render_ScrollBar(screen_arg_t *args);
void render_CheckBox(screen_arg_t *args);
void render_TextScroller(screen_arg_t *args);

void ui_render_element(ElementTypes_t element, screen_arg_t *args) {
  switch (element) {
  case ElementTypes_t::Text:
    render_Text(args);
    break;
  case ElementTypes_t::Number:
    render_Number(args);
    break;
  case ElementTypes_t::Image:
    render_Image(args);
    break;
  case ElementTypes_t::PowerSource:
    render_PowerSource(args);
    break;
  case ElementTypes_t::Temperature:
    render_Temperature(args);
    break;
  case ElementTypes_t::InputVoltage:
    render_InputVoltage(args);
    break;
  case ElementTypes_t::ScrollBar:
    render_ScrollBar(args);
    break;
  case ElementTypes_t::CheckBox:
    render_CheckBox(args);
    break;
  case ElementTypes_t::TextScroller:
    render_TextScroller(args);
    break;
  }
}

void render_Text(screen_arg_t *args) {}
void render_Number(screen_arg_t *args) {}
void render_Image(screen_arg_t *args) {}
void render_PowerSource(screen_arg_t *args) {}
void render_Temperature(screen_arg_t *args) {}
void render_InputVoltage(screen_arg_t *args) {}
void render_ScrollBar(screen_arg_t *args) {}
void render_CheckBox(screen_arg_t *args) {}
void render_TextScroller(screen_arg_t *args) {}