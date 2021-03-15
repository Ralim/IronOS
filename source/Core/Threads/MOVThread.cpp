/*
 * MOVThread.cpp
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BMA223.hpp"
#include "BSP.h"
#include "FreeRTOS.h"
#include "I2C_Wrapper.hpp"
#include "LIS2DH12.hpp"
#include "MMA8652FC.hpp"
#include "MSA301.h"
#include "Model_Config.h"
#include "QC3.h"
#include "SC7A20.hpp"
#include "Settings.h"
#include "TipThermoModel.h"
#include "cmsis_os.h"
#include "history.hpp"
#include "main.hpp"
#include "power.hpp"
#include "stdlib.h"
#include "task.h"
#define MOVFilter 8
uint8_t    accelInit        = 0;
TickType_t lastMovementTime = 0;

void detectAccelerometerVersion() {
  DetectedAccelerometerVersion = ACCELEROMETERS_SCANNING;
#ifdef ACCEL_MMA
  if (MMA8652FC::detect()) {
    if (MMA8652FC::initalize()) {
      DetectedAccelerometerVersion = 1;
    }
  } else
#endif
#ifdef ACCEL_LIS
      if (LIS2DH12::detect()) {
    // Setup the ST Accelerometer
    if (LIS2DH12::initalize()) {
      DetectedAccelerometerVersion = 2;
    }
  } else
#endif
#ifdef ACCEL_BMA
      if (BMA223::detect()) {
    // Setup the BMA223 Accelerometer
    if (BMA223::initalize()) {
      DetectedAccelerometerVersion = 3;
    }
  } else
#endif
#ifdef ACCEL_MSA
      if (MSA301::detect()) {
    // Setup the MSA301 Accelerometer
    if (MSA301::initalize()) {
      DetectedAccelerometerVersion = 4;
    }
  } else
#endif
#ifdef ACCEL_SC7
      if (SC7A20::detect()) {
    // Setup the SC7A20 Accelerometer
    if (SC7A20::initalize()) {
      DetectedAccelerometerVersion = 5;
    }
  } else
#endif
  {
    // disable imu sensitivity
    systemSettings.sensitivity   = 0;
    DetectedAccelerometerVersion = NO_DETECTED_ACCELEROMETER;
  }
}
inline void readAccelerometer(int16_t &tx, int16_t &ty, int16_t &tz, Orientation &rotation) {
#ifdef ACCEL_LIS
  if (DetectedAccelerometerVersion == 2) {
    LIS2DH12::getAxisReadings(tx, ty, tz);
    rotation = LIS2DH12::getOrientation();
  } else
#endif
#ifdef ACCEL_MMA
      if (DetectedAccelerometerVersion == 1) {
    MMA8652FC::getAxisReadings(tx, ty, tz);
    rotation = MMA8652FC::getOrientation();
  } else
#endif
#ifdef ACCEL_BMA
      if (DetectedAccelerometerVersion == 3) {
    BMA223::getAxisReadings(tx, ty, tz);
    rotation = BMA223::getOrientation();
  } else
#endif
#ifdef ACCEL_MSA
      if (DetectedAccelerometerVersion == 4) {
    MSA301::getAxisReadings(tx, ty, tz);
    rotation = MSA301::getOrientation();
  } else
#endif
#ifdef ACCEL_SC7
      if (DetectedAccelerometerVersion == 5) {
    SC7A20::getAxisReadings(tx, ty, tz);
    rotation = SC7A20::getOrientation();
  } else
#endif
  {
    // do nothing :(
  }
}
void startMOVTask(void const *argument __unused) {
  osDelay(TICKS_100MS / 5); // This is here as the BMA doesnt start up instantly and can wedge the I2C bus if probed too fast after boot
  detectAccelerometerVersion();
  osDelay(TICKS_100MS / 2); // wait ~50ms for setup of accel to finalise
  lastMovementTime = 0;
  // Mask 2 seconds if we are in autostart so that if user is plugging in and
  // then putting in stand it doesnt wake instantly
  if (systemSettings.autoStartMode)
    osDelay(2 * TICKS_SECOND);

  int16_t datax[MOVFilter] = {0};
  int16_t datay[MOVFilter] = {0};
  int16_t dataz[MOVFilter] = {0};
  uint8_t currentPointer   = 0;
  int16_t tx = 0, ty = 0, tz = 0;
  int32_t avgx, avgy, avgz;
  if (systemSettings.sensitivity > 9)
    systemSettings.sensitivity = 9;
  Orientation rotation = ORIENTATION_FLAT;
  for (;;) {
    int32_t threshold = 1500 + (9 * 200);
    threshold -= systemSettings.sensitivity * 200; // 200 is the step size
    readAccelerometer(tx, ty, tz, rotation);
    if (systemSettings.OrientationMode == 2) {
      if (rotation != ORIENTATION_FLAT) {
        OLED::setRotation(rotation == ORIENTATION_LEFT_HAND); // link the data through
      }
    }
    datax[currentPointer] = (int32_t)tx;
    datay[currentPointer] = (int32_t)ty;
    dataz[currentPointer] = (int32_t)tz;
    if (!accelInit) {
      for (uint8_t i = currentPointer + 1; i < MOVFilter; i++) {
        datax[i] = (int32_t)tx;
        datay[i] = (int32_t)ty;
        dataz[i] = (int32_t)tz;
      }
      accelInit = 1;
    }
    currentPointer = (currentPointer + 1) % MOVFilter;
    avgx = avgy = avgz = 0;
    // calculate averages
    for (uint8_t i = 0; i < MOVFilter; i++) {
      avgx += datax[i];
      avgy += datay[i];
      avgz += dataz[i];
    }
    avgx /= MOVFilter;
    avgy /= MOVFilter;
    avgz /= MOVFilter;

    // Sum the deltas
    int32_t error = (abs(avgx - tx) + abs(avgy - ty) + abs(avgz - tz));
    // So now we have averages, we want to look if these are different by more
    // than the threshold

    // If movement has occurred then we update the tick timer
    if (error > threshold) {
      lastMovementTime = xTaskGetTickCount();
    }

    vTaskDelay(TICKS_100MS); // Slow down update rate
  }
}
