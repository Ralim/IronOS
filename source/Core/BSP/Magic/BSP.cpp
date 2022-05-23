// BSP mapping functions

#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "IRQ.h"
#include "Pins.h"
#include "Setup.h"
#include "TipThermoModel.h"
#include "configuration.h"
#include "history.hpp"
#include "main.hpp"

// These control the period's of time used for the PWM
const uint16_t powerPWM         = 255;
const uint8_t  holdoffTicks     = 5; // This is the tick delay
const uint8_t  tempMeasureTicks = 5;

uint16_t totalPWM = 255; // Total length of the cycle's ticks

void resetWatchdog() {
  //#TODO
}

#ifdef TEMP_NTC
// Lookup table for the NTC
// Stored as ADCReading,Temp in degC
static const int32_t NTCHandleLookup[] = {
    // ADC Reading , Temp in C x10

    //    64179,	-400, //
    // 64001,	-390, //
    // 63813,	-380, //
    // 63618,	-370, //
    // 63415,	-360, //
    // 63204,	-350, //
    // 62983,	-340, //
    // 62757,	-330, //
    // 62518,	-320, //
    // 62271,	-310, //
    // 62012,	-300, //
    // 61748,	-290, //
    // 61473,	-280, //
    // 61186,	-270, //
    // 60891,	-260, //
    // 60585,	-250, //
    // 60268,	-240, //
    // 59941,	-230, //
    // 59604,	-220, //
    // 59255,	-210, //
    58896, -200, //
    58526, -190, //
    58145, -180, //
    57752, -170, //
    57349, -160, //
    56934, -150, //
    56510, -140, //
    56073, -130, //
    55626, -120, //
    55167, -110, //
    54699, -100, //
    54220, -90,  //
    53729, -80,  //
    53229, -70,  //
    52717, -60,  //
    52200, -50,  //
    51667, -40,  //
    51128, -30,  //
    50582, -20,  //
    50025, -10,  //
    49455, 0,    //
    48883, 10,   //
    48302, 20,   //
    47712, 30,   //
    47116, 40,   //
    46510, 50,   //
    45902, 60,   //
    45286, 70,   //
    44666, 80,   //
    44044, 90,   //
    43412, 100,  //
    42783, 110,  //
    42148, 120,  //
    41500, 130,  //
    40860, 140,  //
    40222, 150,  //
    39576, 160,  //
    38935, 170,  //
    38289, 180,  //
    37640, 190,  //
    36989, 200,  //
    36353, 210,  //
    35705, 220,  //
    35060, 230,  //
    34422, 240,  //
    33792, 250,  //
    33160, 260,  //
    32534, 270,  //
    31910, 280,  //
    31291, 290,  //
    30677, 300,  //
    30069, 310,  //
    29466, 320,  //
    28867, 330,  //
    28277, 340,  //
    27693, 350,  //
    27115, 360,  //
    26544, 370,  //
    25981, 380,  //
    25426, 390,  //
    24880, 400,  //
    24339, 410,  //
    23806, 420,  //
    23281, 430,  //
    22767, 440,  //
    22259, 450,  //
                 // 21761,	460, //
                 // 21268,	470, //
                 // 20787,	480, //
                 // 20313,	490, //
                 // 19848,	500, //
                 // 19392,	510, //
                 // 18945,	520, //
                 // 18507,	530, //
                 // 18076,	540, //
                 // 17655,	550, //
                 // 17242,	560, //
                 // 16838,	570, //
                 // 16442,	580, //
                 // 16052,	590, //
                 // 15672,	600, //

};
#endif
uint16_t getHandleTemperature(uint8_t sample) {
  int32_t result = getADCHandleTemp(sample);
  // Tip is wired up with an NTC thermistor
  // 10K NTC balanced with a 10K pulldown
  // NTCG163JF103FTDS
#ifdef TEMP_NTC
  // vout =
  //  For now not doing interpolation
  for (uint32_t i = 0; i < (sizeof(NTCHandleLookup) / (2 * sizeof(uint16_t))); i++) {
    if (result > NTCHandleLookup[(i * 2) + 0]) {
      return NTCHandleLookup[(i * 2) + 1];
    }
  }
  return 45 * 10;
#endif
#ifdef TEMP_TMP36
#warn This is not shipped in production
  result -= 10240; // remove 0.5V offset
  // 10mV per C
  // 204.7 counts per Deg C above 0C
  result *= 10;
  result /= 205;
  if (result < 0) {
    result = 0;
  }
  return result;
#endif
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

void reboot() {
  // Spin for watchdog
  for (;;) {}
}

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

uint8_t  lastTipResistance = 75; // default safe
uint32_t lastTipReadinguV  = 0;
uint8_t  getTipResitanceX10() {
  // Return tip resistance in x10 ohms
  // We can measure this using the op-amp
  return lastTipResistance;
}
void startMeasureTipResistance() {

  if (isTipDisconnected()) {
    return;
  }

  // We want to calculate lastTipResistance
  // If tip is connected, and the tip is cold and the tip is not being heated
  // We can use the GPIO to inject a small current into the tip and measure this
  // The gpio is 5.1k -> diode -> tip -> gnd
  // Which is around 0.65mA this will induce:
  // 6 ohm tip -> 3.9mV (Real world ~= 3320)
  // 8 ohm tip -> 5.2mV (Real world ~= 4500)
  // Which is definitely measureable
  // Taking shortcuts here as we know we only really have to pick apart 6 and 8 ohm tips
  // These are reported as 60 and 75 respectively
  lastTipReadinguV = TipThermoModel::convertTipRawADCTouV(getTipRawTemp(0));
  gpio_write(TIP_RESISTANCE_SENSE, 1);
  // Wait for next ADC measurement
}

void FinishMeasureTipResistance() {
  gpio_write(TIP_RESISTANCE_SENSE, 0);
  if (isTipDisconnected()) {
    return;
  }
  // read the tip uV with the current source on
  uint32_t newReading = (TipThermoModel::convertTipRawADCTouV(getTipRawTemp(0)));
  if (newReading < lastTipReadinguV) {
    return;
  }
  // newReading -= lastTipReadinguV;
  MSG("Tip Delta %lu, %lu %lu \r\n", newReading - lastTipReadinguV, newReading, lastTipReadinguV);
  newReading -= lastTipReadinguV;
  lastTipReadinguV = newReading;
}