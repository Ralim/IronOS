#include "HUB238.hpp"
#include "I2CBB2.hpp"
#include "Utils.hpp"
#include "configuration.h"

#if POW_PD_EXT == 1
bool hub238_probe() { return I2CBB2::probe(HUB238_ADDR); }

extern int32_t powerSupplyWattageLimit;

uint16_t hub238_debug_state() {
  uint8_t status0 = 0;
  uint8_t status1 = 0;
  if (!I2CBB2::Mem_Read(HUB238_ADDR, HUB238_REG_PD_STATUS0, &status0, 1)) {
    return 0xFFFF;
  }
  if (!I2CBB2::Mem_Read(HUB238_ADDR, HUB238_REG_PD_STATUS1, &status1, 1)) {
    return 0xFFFF;
  }
  return status1 | (((uint16_t)status0) << 8);
}
uint16_t pdo_slot_to_currentx100(uint8_t temp) {
  temp = temp & 0b1111;
  switch (temp) {
  case 0b0000:
    return 50;
  case 0b0001:
    return 70;
  case 0b0010:
    return 100;
  case 0b0011:
    return 125;
  case 0b0100:
    return 150;
  case 0b0101:
    return 175;
  case 0b0110:
    return 200;
  case 0b0111:
    return 225;
  case 0b1000:
    return 250;
  case 0b1001:
    return 275;
  case 0b1010:
    return 300;
  case 0b1011:
    return 325;
  case 0b1100:
    return 350;
  case 0b1101:
    return 400;
  case 0b1110:
    return 450;
  case 0b1111:
    return 500;
  }
}
uint16_t hub238_getVoltagePDOCurrent(uint8_t voltage) {
  uint8_t reg = HUB238_REG_SRC_PDO_5V;
  switch (voltage) {
  case 5:
    reg = HUB238_REG_SRC_PDO_5V;
    break;
  case 9:
    reg = HUB238_REG_SRC_PDO_9V;
    break;
  case 12:
    reg = HUB238_REG_SRC_PDO_12V;
    break;
  case 15:
    reg = HUB238_REG_SRC_PDO_15V;
    break;
  case 18:
    reg = HUB238_REG_SRC_PDO_18V;
    break;
  case 20:
    reg = HUB238_REG_SRC_PDO_20V;
    break;
  default:
    return 0;
  }
  uint8_t temp = 0;
  if (I2CBB2::Mem_Read(HUB238_ADDR, reg, &temp, 1) == true) {
    if (temp & HUB238_PDO_DETECTED) {
      return pdo_slot_to_currentx100(temp);
    }
  }
  return 0;
}
uint8_t findBestPDO() {
  uint8_t  temp              = 0;
  uint16_t ilim              = 0;
  uint16_t minimumx10current = 0;
#if USB_PD_VMAX >= 20
  ilim              = hub238_getVoltagePDOCurrent(20);
  minimumx10current = Utils::RequiredCurrentForTipAtVoltage(200);
  if (ilim != 0 && ilim / 10 >= minimumx10current) {
    powerSupplyWattageLimit = ((20 * ilim) / 100) - 2; // We take off 2W for safety of overhead
    return 0b1010;
  }
#endif
#if USB_PD_VMAX >= 18
  ilim              = hub238_getVoltagePDOCurrent(18);
  minimumx10current = Utils::RequiredCurrentForTipAtVoltage(180);
  if (ilim != 0 && ilim / 10 >= minimumx10current) {
    powerSupplyWattageLimit = ((18 * ilim) / 100) - 2; // We take off 2W for safety of overhead
    return 0b1001;
  }
#endif
#if USB_PD_VMAX >= 15
  ilim              = hub238_getVoltagePDOCurrent(15);
  minimumx10current = Utils::RequiredCurrentForTipAtVoltage(150);
  if (ilim != 0 && ilim / 10 >= minimumx10current) {
    powerSupplyWattageLimit = ((15 * ilim) / 100) - 2; // We take off 2W for safety of overhead
    return 0b1000;
  }
#endif
#if USB_PD_VMAX >= 12
  ilim              = hub238_getVoltagePDOCurrent(12);
  minimumx10current = Utils::RequiredCurrentForTipAtVoltage(120);
  if (ilim != 0 && (ilim / 10) >= minimumx10current) {
    powerSupplyWattageLimit = ((12 * ilim) / 100) - 2; // We take off 2W for safety of overhead
    return 0b0011;
  }
#endif
#if USB_PD_VMAX >= 9
  ilim              = hub238_getVoltagePDOCurrent(9);
  minimumx10current = Utils::RequiredCurrentForTipAtVoltage(90);
  if (ilim != 0 && ilim / 10 >= minimumx10current) {
    powerSupplyWattageLimit = ((9 * ilim) / 100) - 2; // We take off 2W for safety of overhead
    return 0b0010;
  }
#endif

  powerSupplyWattageLimit = 10;
  return 0b0001; // 5V PDO
}
volatile uint8_t haveSelected = 0xFF;

void hub238_check_negotiation() {
  // Dont do anything for first 2 seconds as its internal state machine corrupts if we ask it to change too fast

  if (xTaskGetTickCount() < 2000) {
    return;
  }
  // Want to check if there is a better PDO to be using
  // First, exit early if we already have changed _or_ no PD
  // Even if it negotiates the same voltage as we want, we still re-run it as that makes it ignore the resistor
  // and instead ask for max amps
  if (haveSelected != 0xFF || !hub238_has_negotiated() || hub238_source_voltage() == 0) {
    return;
  }
  uint8_t currentPDO = 0;
  vTaskDelay(5);

  uint8_t bestPDO = findBestPDO();

  if (I2CBB2::Mem_Read(HUB238_ADDR, HUB238_REG_SRC_PDO, &currentPDO, 1) == true) {
    currentPDO >>= 4; // grab upper bits
    if (currentPDO == bestPDO) {
      haveSelected = bestPDO;
      return;
    }
    currentPDO = bestPDO << 4;
    if (I2CBB2::Mem_Write(HUB238_ADDR, HUB238_REG_SRC_PDO, &currentPDO, 1) == true) {

      currentPDO = 0x01; // request for new PDO
      if (I2CBB2::Mem_Write(HUB238_ADDR, HUB238_REG_GO_COMMAND, &currentPDO, 1) == true) {
        haveSelected = bestPDO;
        vTaskDelay(50);

        return;
      }
    }
  }
}
bool hub238_has_run_selection() { return haveSelected != 0xFF; }

bool hub238_has_negotiated() {
  uint8_t temp = 0;
  if (I2CBB2::Mem_Read(HUB238_ADDR, HUB238_REG_PD_STATUS1, &temp, 1) == true) {
    temp >>= 3;
    return (temp & 0b111) == 0b001; // success
  }
  return false;
}

// Return selected source voltage in V
uint16_t hub238_source_voltage() {
  uint8_t temp = 0;
  if (I2CBB2::Mem_Read(HUB238_ADDR, HUB238_REG_PD_STATUS0, &temp, 1) == true) {
    temp >>= 4;
    switch (temp) {
    case 0b0001:
      return 5;
    case 0b0010:
      return 9;
    case 0b0011:
      return 12;
    case 0b0100:
      return 15;
    case 0b0101:
      return 18;
    case 0b0110:
      return 20;
    }
  }
  return 0;
}
// Return selected source current in Amps * 100
uint8_t hub238_source_currentX100() {
  uint8_t temp = 0;
  if (I2CBB2::Mem_Read(HUB238_ADDR, HUB238_REG_PD_STATUS0, &temp, 1) == true) {
    temp &= 0b1111;
    return pdo_slot_to_currentx100(temp);
  }
  return 10; // Failsafe to 0.1 amp
}
#endif
