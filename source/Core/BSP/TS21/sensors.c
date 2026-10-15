#include "ts21.h"

uint8_t ts21_tip_resistance_x10(const struct TS21Sample *sample) {
  if (sample == NULL || sample->tip == 0 || sample->tip >= 4090 || sample->tip_probe >= 4090 || sample->tip_probe <= sample->tip) {
    return 0;
  }
  // Stock 0x0800BBF4: R (ohms) = (ADC1 high phase - ADC1 low phase) / 177.
  return (uint8_t)(((uint32_t)sample->tip_probe - sample->tip) * 10 / 177);
}

bool ts21_tip_delta_c(uint16_t raw, int16_t *value) {
  if (value == NULL || raw == 0 || raw >= 4095) {
    return false;
  }
  // Stock v1.5.0: x = ADC1(PA0 low) - 563; delta C = -.00001*x*x + .2006*x - 3.1236.
  const int32_t x = (int32_t)raw - 563;
  *value          = (int16_t)((-x * x + 20060 * x - 312360) / 100000);
  return true;
}

bool ts21_handle_c_x10(uint16_t raw, int16_t *value) {
  // ADC2 thermistor curve, -30 to 125 C at 5 C intervals (stock v1.5.0).
  static const uint16_t curve[] = {4015, 3985, 3946, 3896, 3831, 3752, 3654, 3536, 3398, 3238, 3059, 2864, 2655, 2437, 2215, 1996,
                                   1784, 1583, 1395, 1223, 1068, 930,  808,  701,  608,  527,  457,  397,  345,  301,  263,  230};
  if (value == NULL || raw > curve[0] || raw < curve[31]) {
    return false;
  }
  for (unsigned i = 1; i < sizeof(curve) / sizeof(curve[0]); ++i) {
    if (raw >= curve[i]) {
      *value = (int16_t)(-300 + (int32_t)(i - 1) * 50 + (curve[i - 1] - raw) * 50 / (curve[i - 1] - curve[i]));
      return true;
    }
  }
  return false;
}

uint16_t ts21_input_voltage_x10(uint16_t raw) {
  // Stock ADC9 scaling: volts = ADC * .0069 - .0088.
  return raw > 1 ? (uint16_t)(((uint32_t)raw * 69 - 88) / 1000) : 0;
}
