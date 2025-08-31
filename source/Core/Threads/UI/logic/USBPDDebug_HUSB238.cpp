#include "HUB238.hpp"
#include "OperatingModes.h"
#include "ui_drawing.hpp"
#if POW_PD_EXT == 1
#ifdef HAS_POWER_DEBUG_MENU
OperatingMode showPDDebug(const ButtonState buttons, guiContext *cxt) {
  // Print out the USB-PD state
  // Basically this is like the Debug menu, but instead we want to print out the PD status
  uint16_t *screen = &(cxt->scratch_state.state1);

  if (*screen > 6) {
    *screen = 0;
  }
  if (*screen == 0) {
    // Print the PD Debug state
    uint16_t temp = hub238_debug_state();
    ui_draw_usb_pd_debug_state(0, temp);
  } else {

    // Print out the Proposed power options one by one
    const uint8_t voltages[]  = {5, 9, 12, 15, 18, 20};
    uint16_t      voltage     = voltages[*screen - 1];
    uint16_t      currentx100 = hub238_getVoltagePDOCurrent(voltage);

    ui_draw_usb_pd_debug_pdo(*screen, 0, voltage, currentx100, 0);
  }

  if (buttons == BUTTON_B_SHORT) {
    return OperatingMode::InitialisationDone;
  } else if (buttons == BUTTON_F_SHORT) {
    *screen++;
  }

  return OperatingMode::UsbPDDebug;
}
#endif
#endif
