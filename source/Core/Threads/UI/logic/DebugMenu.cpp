#include "OperatingModes.h"
#include "ui_drawing.hpp"

OperatingMode showDebugMenu(const ButtonState buttons, guiContext *cxt) {

  ui_draw_debug_menu(cxt->scratch_state.state1);

  if (buttons == BUTTON_B_SHORT) {
    cxt->transitionMode = TransitionAnimation::Up;
    return OperatingMode::HomeScreen;
  } else if (buttons == BUTTON_F_SHORT) {
    cxt->scratch_state.state1++;
#if defined(MCU_TEMP_CUTOFF_C) || defined(DUAL_TIP_SOCKETS)
    // Skip the items this build doesn't have
#ifndef HALL_SENSOR
    if (cxt->scratch_state.state1 == 16) {
      cxt->scratch_state.state1++;
    }
#endif
#ifndef MCU_TEMP_CUTOFF_C
    if (cxt->scratch_state.state1 == 17) {
      cxt->scratch_state.state1++;
    }
#endif
#ifdef DUAL_TIP_SOCKETS
    if (cxt->scratch_state.state1 > 19) {
#else
    if (cxt->scratch_state.state1 > 17) {
#endif
      cxt->scratch_state.state1 = 0;
    }
#else
    // Keep these compile time constants: a variable modulo pulls in the division helper
#if defined(HALL_SENSOR)
    cxt->scratch_state.state1 = cxt->scratch_state.state1 % 17;
#else
    cxt->scratch_state.state1 = cxt->scratch_state.state1 % 16;
#endif
#endif
  }
  return OperatingMode::DebugMenuReadout; // Stay in debug menu
}
