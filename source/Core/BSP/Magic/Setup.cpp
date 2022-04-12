/*
 * Setup.c
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Setup.h"
#include "BSP.h"
#include "Debug.h"
#include "FreeRTOSConfig.h"
#include "Pins.h"
extern "C" {
#include "bflb_platform.h"
#include "bl702_i2c.h"
}
#include "history.hpp"
#include <string.h>
#define ADC_NORM_SAMPLES 16
#define ADC_FILTER_LEN   4
uint16_t ADCReadings[ADC_NORM_SAMPLES]; // room for 32 lots of the pair of readings

// Functions

void hardware_init() {
  gpio_set_mode(OLED_RESET_Pin, GPIO_OUTPUT_MODE);
  // gpio_set_mode(KEY_A_Pin, GPIO_INPUT_PD_MODE);
  // gpio_set_mode(KEY_B_Pin, GPIO_INPUT_PD_MODE);
}

uint16_t getADCHandleTemp(uint8_t sample) {
  static history<uint16_t, ADC_FILTER_LEN> filter = {{0}, 0, 0};
  if (sample) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < ADC_NORM_SAMPLES; i++) {
      sum += ADCReadings[i];
    }
    filter.update(sum);
  }
  return filter.average() >> 1;
}

uint16_t getADCVin(uint8_t sample) {
  static history<uint16_t, ADC_FILTER_LEN> filter = {{0}, 0, 0};
  if (sample) {
    uint16_t latestADC = 0;

    // latestADC += adc_inserted_data_read(ADC1, 0);
    // latestADC += adc_inserted_data_read(ADC1, 1);
    // latestADC += adc_inserted_data_read(ADC1, 2);
    // latestADC += adc_inserted_data_read(ADC1, 3);
    latestADC <<= 1;
    filter.update(latestADC);
  }
  return filter.average();
}

// Returns either average or instant value. When sample is set the samples from the injected ADC are copied to the filter and then the raw reading is returned
uint16_t getTipRawTemp(uint8_t sample) {
  static history<uint16_t, ADC_FILTER_LEN> filter = {{0}, 0, 0};
  if (sample) {
    uint16_t latestADC = 0;

    // latestADC += adc_inserted_data_read(ADC0, 0);
    // latestADC += adc_inserted_data_read(ADC0, 1);
    // latestADC += adc_inserted_data_read(ADC0, 2);
    // latestADC += adc_inserted_data_read(ADC0, 3);
    latestADC <<= 1;
    filter.update(latestADC);
    return latestADC;
  }
  return filter.average();
}

void setupFUSBIRQ() {
  // #TODO
}

void vAssertCalled(void) {
  MSG((char *)"vAssertCalled\r\n");

  while (1)
    ;
}

void vPortEnterCritical(void) {}
void vPortExitCritical(void) {}
