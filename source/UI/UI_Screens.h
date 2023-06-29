#pragma once
#include "UI_Layouts.h"

typedef void (*render_prep_fn)();
typedef void (*tick_fn)();
typedef void (*button_handler_fn)();

typedef struct {
  // on_enter
  // on_exit
  tick_fn           tick;
  render_prep_fn    render_prepare;
  screenLayout_t    layout; // Render layout used
  button_handler_fn handle_button;
} Screen_t;
