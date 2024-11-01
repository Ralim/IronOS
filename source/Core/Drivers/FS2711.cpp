#include "configuration.h"

#if POW_PD_EXT == 2
#include "BSP.h"
#include "FS2711.hpp"
#include "FS2711_defines.h"
#include "I2CBB2.hpp"
#include "Settings.h"
#include "cmsis_os.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifndef USB_PD_VMAX
#error Max PD Voltage must be defined
#endif

#define PROTOCOL_TIMEOUT 100 // ms

extern int32_t powerSupplyWattageLimit;

fs2711_state_t FS2711::state;

inline void i2c_write(uint8_t addr, uint8_t data) { I2CBB2::Mem_Write(FS2711_ADDR, addr, &data, 1); }

inline uint8_t i2c_read(uint8_t addr) {
  uint8_t data = 0;
  I2CBB2::Mem_Read(FS2711_ADDR, addr, &data, 1);
  return data;
}

inline bool i2c_probe(uint8_t addr) { return I2CBB2::probe(addr); }

void FS2711::start() {
  memset(&state, 0, sizeof(fs2711_state_t));
  state.req_pdo_num = 0xFF;

  enable_protocol(false);
  osDelay(PROTOCOL_TIMEOUT);
  select_protocol(FS2711_PROTOCOL_PD);
  enable_protocol(true);
  osDelay(PROTOCOL_TIMEOUT);
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
        if (min_resistance_omhsx10 <= tip_resistance) {
          if (pdo_max_mv > best_voltage) {
            pps          = false;
            best_pdoid   = i;
            best_voltage = pdo_max_mv;
            best_current = pdo_max_curr;
          }
        }
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

// FS2711 does current in mV so it needs to be converted to x100 intead of x1000
uint16_t FS2711::source_currentx100() { return state.source_current / 10; }

uint16_t FS2711::debug_pdo_max_voltage(uint8_t pdoid) { return state.pdo_max_volt[pdoid]; }

uint16_t FS2711::debug_pdo_min_voltage(uint8_t pdoid) { return state.pdo_min_volt[pdoid]; }

uint16_t FS2711::debug_pdo_source_current(uint8_t pdoid) { return state.pdo_max_curr[pdoid]; }

uint16_t FS2711::debug_pdo_type(uint8_t pdoid) { return state.pdo_type[pdoid]; }

fs2711_state_t FS2711::debug_get_state() { return state; }

#endif
