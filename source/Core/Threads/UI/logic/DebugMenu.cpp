#include "OperatingModes.h"
#include "ui_drawing.hpp"

OperatingMode showDebugMenu(const ButtonState buttons, guiContext *cxt) {

  ui_draw_debug_menu(cxt->scratch_state.state1);

  if (buttons == BUTTON_B_SHORT) {
    cxt->transitionMode = TransitionAnimation::Up;
    return OperatingMode::HomeScreen;
  } else if (buttons == BUTTON_F_SHORT) {
    cxt->scratch_state.state1++;
    // Keep these compile time constants: a variable modulo pulls in the division helper
#if defined(MCU_TEMP_CUTOFF_C) && defined(HALL_SENSOR)
    cxt->scratch_state.state1 = cxt->scratch_state.state1 % 18;
#elif defined(MCU_TEMP_CUTOFF_C)
    cxt->scratch_state.state1 = cxt->scratch_state.state1 % 17;
#elif defined(HALL_SENSOR)
    cxt->scratch_state.state1 = cxt->scratch_state.state1 % 17;
#else
    cxt->scratch_state.state1 = cxt->scratch_state.state1 % 16;
#endif
  }
  return OperatingMode::DebugMenuReadout; // Stay in debug menu
}
