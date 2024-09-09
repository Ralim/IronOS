// BSP mapping functions

#include "BSP.h"
#include "BootLogo.h"
#include "I2C_Wrapper.hpp"
#include "IRQ.h"
#include "Pins.h"
#include "Setup.h"
#include "TipThermoModel.h"
#include "USBPD.h"
#include "Utils.h"
#include "bl702_adc.h"
#include "configuration.h"
#include "crc32.h"
#include "hal_flash.h"
#include "history.hpp"
#include "main.hpp"

extern ADC_Gain_Coeff_Type adcGainCoeffCal;

// These control the period's of time used for the PWM
const uint16_t powerPWM         = 255;
uint8_t        holdoffTicks     = 25; // This is the tick delay before temp measure starts (i.e. time for op-amp recovery)
uint8_t        tempMeasureTicks = 25;

uint16_t totalPWM = 255; // Total length of the cycle's ticks

void resetWatchdog() {
  // #TODO
}

#ifdef TEMP_NTC
// Lookup table for the NTC
// Stored as ADCReading,Temp in degC
static const int32_t NTCHandleLookup[] = {
    // ADC Reading , Temp in C x10
    // Based on NTCG163JF103FTDS thermocouple datasheet values,
    // arranged in a voltage divider configuration, with the NTC
    // pulling up towards 3.3V, and with a 10k 1% pull-down resistor.
    // ADC Reading = 3.3V * 10 / (10 + TypkOhm) / 3.2V * (2 ^ 16)
    3405,  -400, //
    4380,  -350, //
    5572,  -300, //
    6999,  -250, //
    8688,  -200, //
    10650, -150, //
    12885, -100, //
    15384, -50,  //
    18129, 0,    //
    21074, 50,   //
    24172, 100,  //
    27362, 150,  //
    30595, 200,  //
    33792, 250,  //
    36907, 300,  //
    39891, 350,  //
    42704, 400,  //
    45325, 450,  //
    47736, 500,  //
    49929, 550,  //
    51912, 600,  //
    53689, 650,  //
    55274, 700,  //
    56679, 750,  //
    57923, 800,  //
    59020, 850,  //
    59984, 900,  //
    60832, 950,  //
    61580, 1000, //
    62232, 1050, //
    62810, 1100, //
    63316, 1150, //
    63765, 1200, //
    64158, 1250, //

};
#endif
uint16_t getHandleTemperature(uint8_t sample) {
  int32_t result = getADCHandleTemp(sample);
  // Tip is wired up with an NTC thermistor
  // 10K NTC balanced with a 10K pulldown
  // NTCG163JF103FTDS
  return Utils::InterpolateLookupTable(NTCHandleLookup, sizeof(NTCHandleLookup) / (2 * sizeof(int32_t)), result);
}

uint16_t getInputVoltageX10(uint16_t divisor, uint8_t sample) {
  uint32_t res = getADCVin(sample);
  res *= 4;
  res /= divisor;
  return res;
}

void unstick_I2C() {
  /* configure SDA/SCL for GPIO */
  // GPIO_BC(GPIOB) |= SDA_Pin | SCL_Pin;
  // gpio_init(SDA_GPIO_Port, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, SDA_Pin | SCL_Pin);
  // for (int i = 0; i < 8; i++) {
  //   asm("nop");
  //   asm("nop");
  //   asm("nop");
  //   asm("nop");
  //   asm("nop");
  //   GPIO_BOP(GPIOB) |= SCL_Pin;
  //   asm("nop");
  //   asm("nop");
  //   asm("nop");
  //   asm("nop");
  //   asm("nop");
  //   GPIO_BOP(GPIOB) &= SCL_Pin;
  // }
  // /* connect PB6 to I2C0_SCL */
  // /* connect PB7 to I2C0_SDA */
  // gpio_init(SDA_GPIO_Port, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, SDA_Pin | SCL_Pin);
}

uint8_t getButtonA() {
  uint8_t val = gpio_read(KEY_A_Pin);
  return val;
}
uint8_t getButtonB() {
  uint8_t val = gpio_read(KEY_B_Pin);
  return val;
}

void reboot() { hal_system_reset(); }

void delay_ms(uint16_t count) {
  // delay_1ms(count);
  BL702_Delay_MS(count);
}

uint32_t __get_IPSR(void) {
  return 0; // To shut-up CMSIS
}

bool isTipDisconnected() {

  uint16_t tipDisconnectedThres = TipThermoModel::getTipMaxInC() - 5;
  uint32_t tipTemp              = TipThermoModel::getTipInC();
  return tipTemp > tipDisconnectedThres;
}

void setStatusLED(const enum StatusLED state) {
  // Dont have one
}
void setBuzzer(bool on) {}

uint8_t       lastTipResistance        = 0; // default to unknown
const uint8_t numTipResistanceReadings = 3;
uint32_t      tipResistanceReadings[3] = {0, 0, 0};
uint8_t       tipResistanceReadingSlot = 0;
uint8_t       getTipResistanceX10() {
  // Return tip resistance in x10 ohms
  // We can measure this using the op-amp
  uint8_t user_selected_tip = getUserSelectedTipResistance();
  if (user_selected_tip == 0) {
    return lastTipResistance; // Auto mode
  }
  return user_selected_tip;
}

uint16_t getTipThermalMass() { return 120; }
uint16_t getTipInertia() { return 750; }
// We want to calculate lastTipResistance
// If tip is connected, and the tip is cold and the tip is not being heated
// We can use the GPIO to inject a small current into the tip and measure this
// The gpio is 5.1k -> diode -> tip -> gnd
// Source is 3.3V-0.5V
// Which is around 0.54mA this will induce:
// 6 ohm tip -> 3.24mV (Real world ~= 3320)
// 8 ohm tip -> 4.32mV (Real world ~= 4500)
// Which is definitely measurable
// Taking shortcuts here as we know we only really have to pick apart 6 and 8 ohm tips
// These are reported as 60 and 75 respectively
void performTipResistanceSampleReading() {
  // 0 = read then turn on pullup, 1 = read then turn off pullup, 2 = read then turn on pullup, 3 = final read + turn off pullup
  tipResistanceReadings[tipResistanceReadingSlot] = TipThermoModel::convertTipRawADCTouV(getTipRawTemp(0));
  gpio_write(TIP_RESISTANCE_SENSE, tipResistanceReadingSlot == 0);
  tipResistanceReadingSlot++;
}
bool tipShorted = false;
void FinishMeasureTipResistance() {

  // Otherwise we now have the 4 samples;
  //  _^_ order, 2 delta's, combine these

  int32_t calculatedSkew = tipResistanceReadings[0] - tipResistanceReadings[2]; // If positive tip is cooling
  calculatedSkew /= 2;                                                          // divide by two to get offset per time constant

  int32_t reading = (((tipResistanceReadings[1] - tipResistanceReadings[0]) + calculatedSkew) // jump 1 - skew
                     +                                                                        // +
                     ((tipResistanceReadings[1] - tipResistanceReadings[2]) + calculatedSkew) // jump 2 - skew
                     )                                                                        //
                    / 2;                                                                      // Take average
  // As we are only detecting three resistances; we just bin to nearest
  uint8_t newRes = 0;
  if (reading > 8000) {
    // Let resistance be cleared to 0
  } else if (reading < 500) {
    tipShorted = true;
  } else if (reading < 2600) {
    newRes = 40;
  } else if (reading < 4000) {
    newRes = 62;
  } else {
    newRes = 80;
  }
  lastTipResistance = newRes;
}
volatile bool       tipMeasurementOccuring = true;
volatile TickType_t nextTipMeasurement     = 100;
bool                isTipShorted() { return tipShorted; }
void                performTipMeasurementStep() {

  // Wait 100ms for settle time
  if (xTaskGetTickCount() < (nextTipMeasurement)) {
    return;
  }
  nextTipMeasurement = xTaskGetTickCount() + TICKS_100MS;
  if (tipResistanceReadingSlot < numTipResistanceReadings) {
    performTipResistanceSampleReading();
    return;
  }

  // We are sensing the resistance
  FinishMeasureTipResistance();

  tipMeasurementOccuring = false;
}

uint8_t preStartChecks() {
  performTipMeasurementStep();
  return preStartChecksDone();
}
// If we are still measuring the tip; or tip is shorted; prevent heating
uint8_t preStartChecksDone() { return (lastTipResistance == 0 || tipResistanceReadingSlot < numTipResistanceReadings || tipMeasurementOccuring || tipShorted) ? 0 : 1; }

// Return hardware unique ID if possible
uint64_t getDeviceID() {
  // uint32_t tmp  = 0;
  // uint32_t tmp2 = 0;
  // EF_Ctrl_Read_Sw_Usage(0, &tmp);
  // EF_Ctrl_Read_Sw_Usage(1, &tmp2);

  // return tmp | (((uint64_t)tmp2) << 32);
  uint64_t tmp = 0;
  EF_Ctrl_Read_Chip_ID((uint8_t *)&tmp);

  return __builtin_bswap64(tmp);
}
auto crc32Table = CRC32Table<>();

uint32_t gethash() {
  static uint32_t computedHash = 0;
  if (computedHash != 0) {
    return computedHash;
  }

  uint32_t       deviceKey        = EF_Ctrl_Get_Key_Slot_w0();
  const uint32_t crcInitialVector = 0xCAFEF00D;
  uint8_t        crcPayload[]     = {(uint8_t)(deviceKey), (uint8_t)(deviceKey >> 8), (uint8_t)(deviceKey >> 16), (uint8_t)(deviceKey >> 24), 0, 0, 0, 0, 0, 0, 0, 0};
  EF_Ctrl_Read_Chip_ID(crcPayload + sizeof(deviceKey)); // Load device key into second half

  computedHash = crc32Table.computeCRC32(crcInitialVector, crcPayload, sizeof(crcPayload));
  return computedHash;
}
uint32_t getDeviceValidation() {
  // 4 byte user data burned in at factory
  return EF_Ctrl_Get_Key_Slot_w1();
}

uint8_t getDeviceValidationStatus() {
  uint32_t programmedHash = EF_Ctrl_Get_Key_Slot_w1();
  uint32_t computedHash   = gethash();
  return programmedHash == computedHash ? 0 : 1;
}

void showBootLogo(void) {
  uint8_t scratch[1024];
  flash_read(FLASH_LOGOADDR - 0x23000000, scratch, 1024);

  BootLogo::handleShowingLogo(scratch);
}

TemperatureType_t getCustomTipMaxInC() {
  // have to lookup the max temp while being aware of the coe scaling value
  float max_reading = ADC_MAX_READING - 1.0;

  if (adcGainCoeffCal.adcGainCoeffEnable) {
    max_reading /= adcGainCoeffCal.coe;
  }

  TemperatureType_t maximumTipTemp = TipThermoModel::convertTipRawADCToDegC(max_reading);
  maximumTipTemp += getHandleTemperature(0) / 10; // Add handle offset
  return maximumTipTemp - 1;
}