#include "configuration.h"

#if POW_PD_EXT == 2
#include "BSP.h"
#include "FS2711.hpp"
#include "FS2711_defines.h"
#ifdef FS2711_CHECK_BOTH_I2C_BUSES
#include "I2CBB1.hpp"
#endif
#include "I2CBB2.hpp"
#include "Settings.h"
#include "Utils.hpp"
#include "cmsis_os.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifndef USB_PD_VMAX
#error Max PD Voltage must be defined
#endif

extern int32_t powerSupplyWattageLimit;

#ifdef FS2711_CHECK_BOTH_I2C_BUSES
// Two hardware revisions per model: the FS2711 sits either on bus 1 (shared with the OLED) or, on the
// newer boards - S99 v1.5 and S60P v1.2 - on its own bus 2. Which one answers at boot is remembered here.
static uint8_t selected_i2c_port = 2;
#endif

fs2711_state_t FS2711::state;

void i2c_write(uint8_t addr, uint8_t data) {
#ifdef FS2711_CHECK_BOTH_I2C_BUSES
  if (selected_i2c_port == 1) {
    I2CBB1::Mem_Write(FS2711_ADDR, addr, &data, 1);
    return;
  }
#endif
  I2CBB2::Mem_Write(FS2711_ADDR, addr, &data, 1);
}

uint8_t i2c_read(uint8_t addr) {
  uint8_t data = 0;
#ifdef FS2711_CHECK_BOTH_I2C_BUSES
  if (selected_i2c_port == 1) {
    I2CBB1::Mem_Read(FS2711_ADDR, addr, &data, 1);
    return data;
  }
#endif
  I2CBB2::Mem_Read(FS2711_ADDR, addr, &data, 1);
  return data;
}

bool i2c_probe(uint8_t addr) {
#ifdef FS2711_CHECK_BOTH_I2C_BUSES
  if (selected_i2c_port == 1) {
    return I2CBB1::probe(addr);
  }
#endif
  return I2CBB2::probe(addr);
}

#ifdef FS2711_CHECK_BOTH_I2C_BUSES
// Two unused addresses just below the FS2711. Probing them first gives the bus a couple of
// throw-away start/address/stop cycles before the real probe: straight after power up the
// first transaction to the FS2711 was seen to go un-ACKed, which would wrongly select bus 1.
static const uint8_t FS2711_WAKE_PROBE_ADDR_1 = FS2711_ADDR - 2; // 0x58
static const uint8_t FS2711_WAKE_PROBE_ADDR_2 = FS2711_ADDR - 1; // 0x59

uint8_t FS2711::detect_i2c_bus_num() {
  I2CBB2::probe(FS2711_WAKE_PROBE_ADDR_1);
  I2CBB2::probe(FS2711_WAKE_PROBE_ADDR_2);
  if (I2CBB2::probe(FS2711_ADDR)) {
    selected_i2c_port = 2;
  } else {
    selected_i2c_port = 1;
  }
  return selected_i2c_port;
}
#endif /* FS2711_CHECK_BOTH_I2C_BUSES */

void FS2711::start() {
  memset(&state, 0, sizeof(fs2711_state_t));
  state.req_pdo_num = 0xFF;

  // S99: bring-up sequence taken from the S99 stock firmware's disassembly - a system reset, then a
  // mode-set / port-reset cycle so the FS2711 re-attaches to the source cleanly before PD is enabled.
  i2c_write(FS2711_REG_SYSTEM_RESET, FS2711_ENABLE);
  osDelay(100);
  i2c_write(FS2711_REG_MODE_SET, 2);
  i2c_write(FS2711_REG_PORT_RESET, 0);
  osDelay(2);
  i2c_write(FS2711_REG_MODE_SET, 0);
  i2c_write(FS2711_REG_PORT_RESET, 1);

  select_protocol(FS2711_PROTOCOL_PD);
  enable_protocol(true);
  // Stock waits 500 ms for the source to settle before reading the PDOs; PDNegTimeout can extend this
  uint32_t waitMs = getSettingValue(SettingsOptions::PDNegTimeout) * 100;
  osDelay(waitMs > 500 ? waitMs : 500);
}

uint8_t FS2711::selected_protocol() { return i2c_read(FS2711_REG_SELECT_PROTOCOL); }

void FS2711::enable_protocol(bool enable) { i2c_write(FS2711_REG_ENABLE_PROTOCOL, enable ? FS2711_ENABLE : FS2711_DISABLE); }

void FS2711::select_protocol(uint8_t protocol) { i2c_write(FS2711_REG_SELECT_PROTOCOL, protocol); }

void FS2711::enable_voltage() { i2c_write(FS2711_REG_ENABLE_VOLTAGE, FS2711_ENABLE); }

bool FS2711::probe() { return i2c_probe(FS2711_ADDR); }

void FS2711::pdo_update() {
  uint8_t pdo_b0 = 0, pdo_b1 = 0, pdo_b2 = 0, pdo_b3 = 0;

  state.pdo_num = 0;
  memset(state.pdo_type, 0, 7);
  memset(state.pdo_min_volt, 0, 7);
  memset(state.pdo_max_volt, 0, 7);
  memset(state.pdo_max_curr, 0, 7);

  for (uint8_t i = 0; i < 7; i++) {
    pdo_b0 = i2c_read(FS2711_REG_PDO_B0 + i * 4);
    pdo_b1 = i2c_read(FS2711_REG_PDO_B1 + i * 4);
    pdo_b2 = i2c_read(FS2711_REG_PDO_B2 + i * 4);
    pdo_b3 = i2c_read(FS2711_REG_PDO_B3 + i * 4);

    if (pdo_b0) {
      if ((pdo_b3 & FS2711_REG_PDO_B0) == FS2711_REG_PDO_B0) {
        state.pdo_type[i]     = FS2711_PDO_PPS;
        state.pdo_min_volt[i] = pdo_b1 * 100;
        state.pdo_max_volt[i] = ((pdo_b2 >> 1) + ((pdo_b3 & 0x1) << 7)) * 100;
        state.pdo_max_curr[i] = (pdo_b0 & 0x7F) * 50;
      } else {
        state.pdo_type[i]     = FS2711_PDO_FIX;
        state.pdo_min_volt[i] = ((pdo_b1 >> 2) + ((pdo_b2 & 0xF) << 6)) * 50;
        state.pdo_max_volt[i] = state.pdo_min_volt[i];
        state.pdo_max_curr[i] = (pdo_b0 + ((pdo_b1 & 0x3) << 8)) * 10;
      }
      state.pdo_num++;
    }
  }
}

bool FS2711::open_pps(uint8_t pdoid, uint16_t volt, uint16_t max_curr) {
  uint16_t wr;

  if (pdoid > state.pdo_num)
    return false;
  if ((volt > state.pdo_max_volt[pdoid]) || (volt < state.pdo_min_volt[pdoid]))
    return false;
  if ((volt > state.pdo_max_volt[pdoid]) || (volt < state.pdo_min_volt[pdoid]))
    return false;
  if (max_curr > state.pdo_max_curr[pdoid])
    return false;
  if (state.pdo_type[pdoid] != FS2711_PDO_PPS)
    return false;

  if (FS2711::selected_protocol() == FS2711_PROTOCOL_PD) {
    select_protocol(FS2711_PROTOCOL_PPS);
    enable_protocol(true);
  }

  if (FS2711::selected_protocol() != FS2711_PROTOCOL_PPS) {
    return false;
  }

  i2c_write(FS2711_REG_PDO_IDX, pdoid + (pdoid << 4));
  wr = (volt - state.pdo_min_volt[pdoid]) / 20;
  i2c_write(FS2711_PROTOCOL_PPS_CURRENT, max_curr / 50);

  i2c_write(FS2711_REG_VOLT_CFG_B0, wr & 0xFF);
  i2c_write(FS2711_REG_VOLT_CFG_B1, (wr >> 8) & 0xFF);
  i2c_write(FS2711_REG_VOLT_CFG_B2, wr & 0xFF);
  i2c_write(FS2711_REG_VOLT_CFG_B3, (wr >> 8) & 0xFF);

  enable_voltage();

  state.source_voltage    = volt;
  state.source_current    = max_curr;
  state.req_pdo_num       = pdoid;
  powerSupplyWattageLimit = ((volt * max_curr) / 1000000) - 2;
  return true;
}

bool FS2711::open_pd(uint8_t pdoid) {
  if (pdoid >= state.pdo_num) {
    return false;
  }
  if (state.pdo_type[pdoid] != FS2711_PDO_FIX) {
    return false;
  }

  if (FS2711::selected_protocol() != FS2711_PROTOCOL_PD) {
    return false;
  }

  i2c_write(FS2711_REG_PDO_IDX, pdoid + (pdoid << 4));

  enable_voltage();

  state.source_voltage = state.pdo_max_volt[pdoid];
  state.source_current = state.pdo_max_curr[pdoid];
  state.req_pdo_num    = pdoid;

  powerSupplyWattageLimit = ((state.source_voltage * state.source_current) / 1000000) - 2;
  return true;
}

void FS2711::negotiate() {
  uint16_t best_voltage = 0;
  uint16_t best_current = 0;
  uint8_t  best_pdoid   = 0xFF;
  bool     pps          = false;

  int min_resistance_omhsx10 = 0;

  // FS2711 uses mV instead of V
  const uint16_t vmax           = USB_PD_VMAX * 1000;
  uint8_t        tip_resistance = getTipResistanceX10();
  if (getSettingValue(SettingsOptions::USBPDMode) == usbpdMode_t::DEFAULT) {
    tip_resistance += 5;
  }
#ifdef TIP_CURRENT_LIMIT_CHOP
  // No inductor: a PDO whose current is below V / R_tip can only be used by chopping the output, which shows up
  // at the charger as current pulses at the full tip current. So pick the PDO that delivers the most power as
  // min(V^2 / R, V * I) and, within 10 %, prefer one that does not need chopping at all - e.g. 15 V / 3 A over
  // 20 V / 2.25 A for a 5.5 ohm cartridge (41 W clean instead of 45 W pulsed).
  const uint16_t real_tip_resistance = getTipResistanceX10() > 0 ? getTipResistanceX10() : 1;
  uint32_t       best_deliverable_mw = 0;
  bool           best_needs_chop     = true;
  // Second candidate: the best PDO whose chop pulses stay within 1.5x its rating. Sources with tight
  // constant-current limiting fold back on harder pulses, but a low resistance cartridge cannot avoid
  // them at all, so this is only preferred when it does not cost most of the power (see below).
  uint8_t  gentle_pdoid          = 0xFF;
  uint16_t gentle_voltage        = 0;
  uint16_t gentle_current        = 0;
  uint32_t gentle_deliverable_mw = 0;
#endif
#ifdef MODEL_HAS_DCDC
  // If this device has step down DC/DC inductor to smooth out current spikes
  // We can instead ignore resistance and go for max voltage we can accept; and rely on the DC/DC regulation to keep under current limit
  tip_resistance = 255; // (Push to 25.5 ohms to effectively disable this check)
#endif

  uint16_t pdo_min_mv = 0, pdo_max_mv = 0, pdo_max_curr = 0, pdo_type = 0;

  FS2711::pdo_update();

  for (int i = 0; state.pdo_num > i; i++) {
    pdo_min_mv   = state.pdo_min_volt[i];
    pdo_max_mv   = state.pdo_max_volt[i];
    pdo_max_curr = state.pdo_max_curr[i];
    pdo_type     = state.pdo_type[i];

    min_resistance_omhsx10 = (pdo_max_mv / pdo_max_curr) * 10;

    switch (pdo_type) {
    case FS2711_PDO_FIX:
      if (pdo_max_mv > 0 && vmax >= pdo_max_mv) {
#ifdef TIP_CURRENT_LIMIT_CHOP
        {
          const uint32_t v_mv         = pdo_max_mv;
          const uint32_t by_tip_mw    = (v_mv * v_mv * 10) / real_tip_resistance / 1000; // V^2 / R
          const uint32_t by_supply_mw = (v_mv * pdo_max_curr) / 1000;                    // V * I
          const uint32_t deliverable  = by_tip_mw < by_supply_mw ? by_tip_mw : by_supply_mw;
          const bool     needs_chop   = by_tip_mw > by_supply_mw;
          // With no inductor the chop pulses run at the full tip current V / R
          if (!needs_chop || (by_tip_mw * 2) <= (by_supply_mw * 3)) {
            if (deliverable > gentle_deliverable_mw) {
              gentle_pdoid          = i;
              gentle_voltage        = pdo_max_mv;
              gentle_current        = pdo_max_curr;
              gentle_deliverable_mw = deliverable;
            }
          }
          bool better = deliverable > best_deliverable_mw;
          if (best_pdoid != 0xFF && !pps) {
            // Within 10 % of the best so far: the one without chopping wins, otherwise the higher voltage
            if (deliverable * 10 >= best_deliverable_mw * 9 && deliverable <= best_deliverable_mw) {
              better = best_needs_chop && !needs_chop;
            } else if (deliverable >= best_deliverable_mw && deliverable * 9 <= best_deliverable_mw * 10) {
              better = !(needs_chop && !best_needs_chop);
            }
          }
          if (better) {
            pps                 = false;
            best_pdoid          = i;
            best_voltage        = pdo_max_mv;
            best_current        = pdo_max_curr;
            best_deliverable_mw = deliverable;
            best_needs_chop     = needs_chop;
          }
        }
        (void)min_resistance_omhsx10;
        (void)tip_resistance;
#else
        if (min_resistance_omhsx10 <= tip_resistance) {
          if (pdo_max_mv > best_voltage) {
            pps          = false;
            best_pdoid   = i;
            best_voltage = pdo_max_mv;
            best_current = pdo_max_curr;
          }
        }
#endif /* TIP_CURRENT_LIMIT_CHOP */
      }
      break;

    case FS2711_PDO_PPS: {
      int ideal_mv = tip_resistance * (pdo_max_curr / 10);
      if (ideal_mv > pdo_max_mv) {
        ideal_mv = pdo_max_mv;
      }

      if (ideal_mv > vmax) {
        ideal_mv = vmax;
      }

      if (ideal_mv > best_voltage) {
        best_pdoid   = i;
        best_voltage = ideal_mv;
        best_current = pdo_max_curr;
        pps          = true;
      }
    }

    break;

    default:
      break;
    }
  }

#ifdef TIP_CURRENT_LIMIT_CHOP
  // Prefer the gentle PDO only while it keeps at least 70 % of the power: an Apple 35 W adapter with a
  // 5.5 ohm cartridge moves from 20 V / 1.75 A (pulses 2.1x its rating, browns out) to 15 V / 2.33 A
  // (31 W), while a 2.5 ohm cartridge - which cannot be run without hard pulses on any PDO - keeps
  // 20 V / 3.25 A (58 W) instead of dropping to 9 V (20 W).
  if (!pps && gentle_pdoid != 0xFF && gentle_pdoid != best_pdoid && (gentle_deliverable_mw * 10) >= (best_deliverable_mw * 7)) {
    best_pdoid   = gentle_pdoid;
    best_voltage = gentle_voltage;
    best_current = gentle_current;
  }
#endif

  if (best_pdoid != 0xFF && best_pdoid != state.req_pdo_num) {
    if (pps) {
      FS2711::open_pps(best_pdoid, best_voltage, best_current);
    } else {
      FS2711::open_pd(best_pdoid);
    }
  }
}

bool FS2711::has_run_selection() { return state.req_pdo_num != 0xFF; }

uint16_t FS2711::source_voltage() { return state.source_voltage / 1000; }

// FS2711 does current in mA so it needs to be converted to x100 intead of x1000
uint16_t FS2711::source_currentx100() { return state.source_current / 10; }

uint16_t FS2711::debug_pdo_max_voltage(uint8_t pdoid) { return state.pdo_max_volt[pdoid]; }

uint16_t FS2711::debug_pdo_min_voltage(uint8_t pdoid) { return state.pdo_min_volt[pdoid]; }

uint16_t FS2711::debug_pdo_source_current(uint8_t pdoid) { return state.pdo_max_curr[pdoid]; }

uint16_t FS2711::debug_pdo_type(uint8_t pdoid) { return state.pdo_type[pdoid]; }

fs2711_state_t FS2711::debug_get_state() { return state; }

#endif
