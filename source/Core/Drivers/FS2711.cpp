#include "FS2711.hpp"
#include "FS2711_define.h"
#include "I2CBB2.hpp"
#include "configuration.h"
#if POW_PD_EXT == 2
#include "FreeRTOS.h"
#include <stdbool.h>
#include <string.h>

#ifndef USB_PD_VMAX
#error Max PD Voltage must be defined
#endif

fs2711_state_t FS2711::state;

void i2c_write(uint8_t addr, uint8_t data) { I2CBB2::Mem_Write(FS2711_ADDR, addr, &data, 1); }

uint8_t i2c_read(uint8_t addr) {
  uint8_t data = 0;
  I2CBB2::Mem_Read(FS2711_ADDR, addr, &data, 1);
  return data;
}

bool i2c_probe(uint8_t addr) { return I2CBB2::probe(addr); }

void osDelay(uint32_t delayms) {
  // Convert ms -> ticks
  TickType_t ticks = delayms / portTICK_PERIOD_MS;

  vTaskDelay(ticks ? ticks : 1);
}

void FS2711::start() {
  state.up                = false;
  state.pdo_num           = 0;
  state.req_pdo_num       = 10; // Disabled
  state.negotiated        = false;
  state.failed_pdo_checks = false;
  memset(state.pdo_type, 0, 7);
  memset(state.pdo_min_volt, 0, 7);
  memset(state.pdo_max_volt, 0, 7);
  memset(state.pdo_max_curr, 0, 7);

  // start_scan();
  // update_state();
  probe_pd();
}

void FS2711::enable_protocol(bool enable) { i2c_write(FS2711_REG_ENABLE_PROTOCOL, enable ? FS2711_ENABLE : FS2711_DISABLE); }

void FS2711::select_protocol(uint8_t protocol) { i2c_write(FS2711_REG_SELECT_PROTOCOL, protocol); }

void FS2711::enable_voltage() { i2c_write(FS2711_REG_ENABLE_VOLTAGE, FS2711_ENABLE); }

bool FS2711::probe() { return i2c_probe(FS2711_ADDR); }

void FS2711::system_reset() { i2c_write(FS2711_REG_SYSTEM_RESET, 0x01); }

void FS2711::port_reset() {
  i2c_write(FS2711_REG_MODE_SET, 0x02);   // Disconnects Rd?
  i2c_write(FS2711_REG_PORT_RESET, 0x00); // Port Reset
  osDelay(1);
  i2c_write(FS2711_REG_MODE_SET, 0x00);
  i2c_write(FS2711_REG_PORT_RESET, 0x01);
}

void FS2711::dpdm_reset() { i2c_write(FS2711_REG_DPDM, 0x00); }

void FS2711::start_scan() { i2c_write(FS2711_REG_SCAN_START, 0x01); }

void FS2711::update_state() {
  uint8_t i      = 0;
  uint8_t pdo_b0 = 0, pdo_b1 = 0, pdo_b2 = 0, pdo_b3 = 0;

  uint8_t state0 = ~i2c_read(FS2711_REG_STATE0);
  uint8_t state1 = ~i2c_read(FS2711_REG_STATE1);

  // state.state = (state1 | ((uint16_t)state0) << 8);
  state.state = state1;

  if (state1 & 0x1) {
    state.proto_exists = i2c_read(FS2711_REG_PROTOCOL_EXISTS);
    state.proto_exists |= i2c_read(FS2711_REG_PROTOCOL_EXISTS + 1) << 8;
    state.proto_exists |= i2c_read(FS2711_REG_PROTOCOL_EXISTS + 2) << 16;

    state.qc_max_volt = i2c_read(FS2711_PROTOCOL_QC_MAX_VOLT);
  }

  if (state1 & 0x2) {
    state.pdo_num = 0;
    memset(state.pdo_type, 0, 7);
    memset(state.pdo_min_volt, 0, 7);
    memset(state.pdo_max_volt, 0, 7);
    memset(state.pdo_max_curr, 0, 7);
    osDelay(5000);

    for (i = 0; 7 > i; i++) {
      pdo_b0 = i2c_read(FS2711_REG_PDO_B0 + i * 4);
      pdo_b1 = i2c_read(FS2711_REG_PDO_B1 + i * 4);
      pdo_b2 = i2c_read(FS2711_REG_PDO_B2 + i * 4);
      pdo_b3 = i2c_read(FS2711_REG_PDO_B3 + i * 4);

      if (pdo_b0) {
        if (pdo_b3 & 0xC0) {
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
}

void FS2711::probe_pd() {
  enable_protocol(false);
  osDelay(500);
  select_protocol(FS2711_PROTOCOL_PD);
  enable_protocol(true);

  uint8_t i      = 0;
  uint8_t pdo_b0 = 0, pdo_b1 = 0, pdo_b2 = 0, pdo_b3 = 0;

  state.pdo_num = 0;
  memset(state.pdo_type, 0, 7);
  memset(state.pdo_min_volt, 0, 7);
  memset(state.pdo_max_volt, 0, 7);
  memset(state.pdo_max_curr, 0, 7);
  osDelay(5000);

  for (i = 0; 7 > i; i++) {
    pdo_b0 = i2c_read(FS2711_REG_PDO_B0 + i * 4);
    pdo_b1 = i2c_read(FS2711_REG_PDO_B1 + i * 4);
    pdo_b2 = i2c_read(FS2711_REG_PDO_B2 + i * 4);
    pdo_b3 = i2c_read(FS2711_REG_PDO_B3 + i * 4);

    if (pdo_b0) {
      if (pdo_b3 & 0xC0) {
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

fs2711_state_t FS2711::get_state() { return state; }

bool FS2711::open_pps(uint8_t PDOID, uint16_t volt, uint16_t max_curr) {
  uint16_t wr;

  if (PDOID > state.pdo_num)
    return false;
  if ((volt > state.pdo_max_volt[PDOID]) || (volt < state.pdo_min_volt[PDOID]))
    return false;
  if ((volt > state.pdo_max_volt[PDOID]) || (volt < state.pdo_min_volt[PDOID]))
    return false;
  if (max_curr > state.pdo_max_curr[PDOID])
    return false;
  if (state.pdo_type[PDOID] != FS2711_PDO_PPS)
    return false;

  if (state.protocol != FS2711_PROTOCOL_PPS) {
    enable_protocol(false);
    osDelay(500);
    select_protocol(FS2711_PROTOCOL_PD);
    enable_protocol(true);
    osDelay(100);
    select_protocol(FS2711_PROTOCOL_PPS);
    enable_protocol(true);
    osDelay(3000);
  }

  i2c_write(FS2711_PROTOCOL_PD_PDOID, PDOID + (PDOID << 4));
  wr = (volt - state.pdo_min_volt[PDOID]) / 20;
  i2c_write(FS2711_PROTOCOL_PPS_CURRENT, max_curr / 50);

  i2c_write(0xF4, wr & 0xFF);
  i2c_write(0xF5, (wr >> 8) & 0xFF);
  i2c_write(0xF6, wr & 0xFF);
  i2c_write(0xF7, (wr >> 8) & 0xFF);

  enable_voltage();

  state.source_voltage = volt;
  state.source_current = max_curr;
  state.protocol       = FS2711_PROTOCOL_PPS;
  state.up             = true;
  return true;
}

bool FS2711::open_pd(uint8_t PDOID) {
  if (PDOID > state.pdo_num) {
    return false;
  }
  if (state.pdo_type[PDOID] != FS2711_PDO_FIX) {
    return false;
  }

  if (state.protocol != FS2711_PROTOCOL_PD) {
    enable_protocol(false);
    osDelay(500);
    select_protocol(FS2711_PROTOCOL_PD);
    enable_protocol(true);
    osDelay(3000);
  }

  i2c_write(FS2711_PROTOCOL_PD_PDOID, PDOID + (PDOID << 4));

  enable_voltage();

  state.source_current = state.pdo_max_curr[PDOID];
  state.source_voltage = state.pdo_max_volt[PDOID];
  state.protocol       = FS2711_PROTOCOL_PD;
  state.up             = true;
  return true;
}

void FS2711::negotiate() {
  uint8_t        pdoid      = 0;
  bool           pps        = false;
  bool           negotiated = false;
  uint16_t       voltage    = 0;
  int            i;
  const uint16_t vmax = USB_PD_VMAX * 1000;

  for (i = 0; state.pdo_num > i; i++) {
    // We check if the PDO Voltage is higher to our current voltage;
    if (state.pdo_max_volt[i] > voltage) {
      // Checks if we have a fixed PDO and if the voltage is lower or equal to our max allowed voltage.
      if (state.pdo_type[i] == 0 && vmax >= state.pdo_max_volt[i]) {
        voltage    = state.pdo_max_volt[i];
        pdoid      = i;
        negotiated = true;
        pps        = false;
        // Checks if we have a PPS supply and if the minimun voltage is lower or equal to our max allowed voltage.
      } else if (state.pdo_type[i] == 1 && vmax >= state.pdo_min_volt[i]) {
        pdoid      = i;
        negotiated = true;
        pps        = true;
        // Select the smallest number between max allowed voltage and max psu voltage.
        voltage = vmax < state.pdo_max_volt[i] ? vmax : state.pdo_max_volt[i];
      }
    }
  }

  state.negotiated   = negotiated;
  state.pps          = pps;
  state.req_pdo_num  = i;
  state.req_pdo_volt = voltage ? voltage / 1000 : 0;
  state.protocol     = FS2711_PROTOCOL_PD;

  if (negotiated) {
    if (pps) {
      FS2711::open_pps(pdoid, voltage, state.pdo_max_curr[pdoid]);
    } else {
      FS2711::open_pd(pdoid);
    }
  }
}

bool FS2711::has_run_selection() { return state.up; }

uint16_t FS2711::source_voltage() { return state.source_voltage; }

uint16_t FS2711::source_currentx100() { return state.source_current / 10; }

uint16_t FS2711::debug_pdo_source_voltage(uint8_t pdoid) { return state.pdo_max_volt[pdoid]; }

uint16_t FS2711::debug_pdo_source_current(uint8_t pdoid) { return state.pdo_max_curr[pdoid]; }

uint16_t FS2711::debug_pdo_type(uint8_t pdoid) { return state.pdo_type[pdoid]; }

bool FS2711::debug_detected_pd() { return (state.proto_exists >> FS2711_PROTOCOL_PD) & 0x00000001; }

uint16_t FS2711::debug_state() {
  //  uint8_t state0 = 0, state1 = 0;
  //
  //  if (!I2CBB2::Mem_Read(FS2711_READ_ADDR, FS2711_REG_STATE0, &state0, 1)) {
  //    return 0;
  //  }
  //
  //  if (!I2CBB2::Mem_Read(FS2711_READ_ADDR, FS2711_REG_STATE1, &state1, 1)) {
  //    return 0;
  //  }
  //
  //  return ~(state1 | (((uint16_t)state0) << 8));
  return state.state;
}
#endif
