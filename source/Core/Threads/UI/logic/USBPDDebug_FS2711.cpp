#include "FS2711.hpp"
#include "OperatingModes.h"
#include "stdbool.h"
#include "ui_drawing.hpp"
#if POW_PD_EXT == 2
#ifdef HAS_POWER_DEBUG_MENU

OperatingMode showPDDebug(const ButtonState buttons, guiContext *cxt) {
  // Print out the USB-PD state
  // Basically this is like the Debug menu, but instead we want to print out the PD status
  uint16_t *screen = &(cxt->scratch_state.state1);

  if (*screen > 7) {
    *screen = 0;
  }
  if (*screen == 0) {
    // Print the PD Debug state
    fs2711_state_t state = FS2711::debug_get_state();

    ui_draw_usb_pd_debug_state(0, state.pdo_num);
  } else {

    // Print out the Proposed power options one by one
    uint16_t max_voltage = FS2711::debug_pdo_max_voltage(*screen - 1);
    if (max_voltage == 0) {
      *screen += 1;
    } else {
      uint16_t min_voltage = FS2711::debug_pdo_min_voltage(*screen - 1);
      uint16_t current     = FS2711::debug_pdo_source_current(*screen - 1);
      uint16_t pdo_type    = FS2711::debug_pdo_type(*screen - 1);
      if (pdo_type != 1) {
        min_voltage = 0;
      }

      ui_draw_usb_pd_debug_pdo(*screen, min_voltage / 1000, max_voltage / 1000, current * 1, 0);
    }
  }

  OLED::refresh();

  if (buttons == BUTTON_B_SHORT) {
    return OperatingMode::InitialisationDone;
  } else if (buttons == BUTTON_F_SHORT) {
    *screen++;
  }

  return OperatingMode::UsbPDDebug;
}
#endif
#endif
