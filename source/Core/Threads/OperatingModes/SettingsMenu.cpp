#include "OperatingModes.h"

OperatingMode handleSettingsButtons(const ButtonState buttons, guiContext *cxt) {
  switch (buttons) {
  case BUTTON_NONE:
    // Do nothing
    break;
  case BUTTON_BOTH:
    break;

  case BUTTON_B_LONG:
    return OperatingMode::DebugMenuReadout;
    break;
  case BUTTON_F_LONG:
#ifdef PROFILE_SUPPORT
    if (!isTipDisconnected()) {
      return OperatingMode::SolderingProfile;
    }
#else
    return OperatingMode::TemperatureAdjust;
#endif
    break;
  case BUTTON_F_SHORT:
    if (!isTipDisconnected()) {
      return OperatingMode::Soldering;
    }
    break;
  case BUTTON_B_SHORT:
    return OperatingMode::SettingsMenu;
    break;
  default:
    break;
  }
  return OperatingMode::HomeScreen;
}

OperatingMode gui_SettingsMenu(const ButtonState buttons, guiContext *cxt) {
  // Render out the current settings menu
  // State 1 -> Root menu
  // State 2 -> Sub entry
  uint16_t *mainEntry = &(cxt->scratch_state.state1);
  uint16_t *subEntry  = &(cxt->scratch_state.state2);

  return handleSettingsButtons(buttons, cxt);
}