#pragma once
#include "UI_Layouts.h"
#include <stdint.h>

typedef union {
  int32_t i32;
  void   *ptr;
} screen_arg_t;

typedef struct {
  screen_arg_t args[4];
} ScreenContext_t;

//
void ui_render_screen(screenLayout_t *screen, ScreenContext_t *context);
void ui_render_element(const ElementTypes_t element, const ElementSettings_t *settings, screen_arg_t *args);