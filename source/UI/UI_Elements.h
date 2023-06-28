#pragma once
#include "UI.h"
typedef enum {
  Text,         // Basic text splat, using re-encoded strings
  Number,       // Draws numbers using best size for the height (always one line)
  Image,        // Pre-rendered bitmap that can be memcpy'ed in
  PowerSource,  // Draws a battery icon, or text for voltage, or DC power plug
  Temperature,  // Draws the number with temperature symbol following (height picks font)
  InputVoltage, // Draws the number with V following and also 1 dp(height picks font)
  ScrollBar,    // Draws a vertical scrollbar, number sets percentage 0-100
  CheckBox,     // Draws checkbox, ticked = number!=0
  TextScroller, // Renders text, scrolling with time
} ElementTypes_t;

typedef struct {
  struct {
    uint8_t x;
    uint8_t y;
  } position;
  struct {
    uint8_t w;
    uint8_t h;
  } size;


} ElementSettings_t;

void ui_render_element(ElementTypes_t element, screen_arg_t *args);