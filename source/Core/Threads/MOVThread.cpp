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
#include "Pins.h"
#include "QC3.h"
#include "SC7A20.hpp"
#include "Settings.h"
#include "TipThermoModel.h"
#include "cmsis_os.h"
#include "configuration.h"
#include "history.hpp"
#include "main.hpp"
#include "power.hpp"
#include "stdlib.h"
#include "task.h"

#define MOVFilter 8
uint8_t      accelInit        = 0;
TickType_t   lastMovementTime = 0;
volatile bool freefallDetected = false;
static int32_t slowGravRefX    = 0;
static int32_t slowGravRefY    = 0;
static int32_t slowGravRefZ    = 0;
// Order matters for probe order, some Acceleromters do NOT like bad reads; and we have a bunch of overlap of addresses
void detectAccelerometerVersion() {
#ifdef ACCEL_MMA
  if (MMA8652FC::detect()) {
    if (MMA8652FC::initalize()) {
      DetectedAccelerometerVersion = AccelType::MMA;
      return;
    }
  }
#endif
#ifdef ACCEL_LIS
  if (LIS2DH12::detect()) {
    // Setup the ST Accelerometer
    if (LIS2DH12::initalize()) {
      if (LIS2DH12::isClone()) {
        DetectedAccelerometerVersion = AccelType::LIS_CLONE;
      } else {
        DetectedAccelerometerVersion = AccelType::LIS;
      }
      return;
    }
  }
#endif
#ifdef ACCEL_BMA
  if (BMA223::detect()) {
    // Setup the BMA223 Accelerometer
    if (BMA223::initalize()) {
      DetectedAccelerometerVersion = AccelType::BMA;
      return;
    }
  }
#endif
#ifdef ACCEL_SC7
  if (SC7A20::detect()) {
    // Setup the SC7A20 Accelerometer
    if (SC7A20::initalize()) {
      DetectedAccelerometerVersion = AccelType::SC7;
      return;
    }
  }
#endif
#ifdef ACCEL_MSA
  if (MSA301::detect()) {
    // Setup the MSA301 Accelerometer
    if (MSA301::initalize()) {
      DetectedAccelerometerVersion = AccelType::MSA;
      return;
    }
  }
#endif
#ifdef GPIO_VIBRATION
  if (true) {
    DetectedAccelerometerVersion = AccelType::GPIO;
    return;
  }
#endif
  {
    // disable imu sensitivity
    setSettingValue(SettingsOptions::Sensitivity, 0);
    DetectedAccelerometerVersion = AccelType::None;
  }
}
inline void readAccelerometer(int16_t &tx, int16_t &ty, int16_t &tz, Orientation &rotation) {
#ifdef ACCEL_MMA
  if (DetectedAccelerometerVersion == AccelType::MMA) {
    MMA8652FC::getAxisReadings(tx, ty, tz);
    rotation = MMA8652FC::getOrientation();
  } else
#endif
#ifdef ACCEL_LIS
      if (DetectedAccelerometerVersion == AccelType::LIS || DetectedAccelerometerVersion == AccelType::LIS_CLONE) {
    LIS2DH12::getAxisReadings(tx, ty, tz);
    rotation = LIS2DH12::getOrientation();
  } else
#endif
#ifdef ACCEL_BMA
      if (DetectedAccelerometerVersion == AccelType::BMA) {
    BMA223::getAxisReadings(tx, ty, tz);
    rotation = BMA223::getOrientation();
  } else
#endif
#ifdef ACCEL_MSA
      if (DetectedAccelerometerVersion == AccelType::MSA) {
    MSA301::getAxisReadings(tx, ty, tz);
    rotation = MSA301::getOrientation();
  } else
#endif
#ifdef ACCEL_SC7
      if (DetectedAccelerometerVersion == AccelType::SC7) {
    SC7A20::getAxisReadings(tx, ty, tz);
    rotation = SC7A20::getOrientation();
  } else
#endif
#ifdef GPIO_VIBRATION
      if (DetectedAccelerometerVersion == AccelType::GPIO) {
    // TODO
    if (HAL_GPIO_ReadPin(MOVEMENT_GPIO_Port, MOVEMENT_Pin) == GPIO_PIN_SET) {
      // Movement
      tx = ty = tz = 5000;
    } else {
      // No Movement
      tx = ty = tz = 0;
    }
    rotation = Orientation::ORIENTATION_FLAT;
  } else
#endif
  {
    // do nothing :(
  }
}
void startMOVTask(void const *argument __unused) {
#ifdef NO_ACCEL
  DetectedAccelerometerVersion = AccelType::None;
  for (;;) {
    osDelay(2 * TICKS_SECOND);
  }
#endif

  osDelay(TICKS_100MS / 5); // This is here as the BMA doesnt start up instantly and can wedge the I2C bus if probed too fast after boot
  detectAccelerometerVersion();
  osDelay(TICKS_100MS / 2); // wait ~50ms for setup of accel to finalise
  lastMovementTime = 0;
  // Mask 2 seconds if we are in autostart so that if user is plugging in and
  // then putting in stand it doesnt wake instantly
  if (getSettingValue(SettingsOptions::AutoStartMode)) {
    osDelay(2 * TICKS_SECOND);
  }

  int16_t     datax[MOVFilter] = {0};
  int16_t     datay[MOVFilter] = {0};
  int16_t     dataz[MOVFilter] = {0};
  uint8_t     currentPointer   = 0;
  int16_t     tx = 0, ty = 0, tz = 0;
  int32_t     avgx, avgy, avgz;
  Orientation rotation        = ORIENTATION_FLAT;
  uint8_t     freefallCounter = 0;
#ifdef ACCEL_EXITS_ON_MOVEMENT
  uint16_t tripCounter = 0;
#endif
  for (;;) {
    int32_t threshold = 1500 + (9 * 200);
    threshold -= getSettingValue(SettingsOptions::Sensitivity) * 200; // 200 is the step size
    readAccelerometer(tx, ty, tz, rotation);
    if (getSettingValue(SettingsOptions::OrientationMode) == 2) {
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
      accelInit    = 1;
      slowGravRefX = tx;
      slowGravRefY = ty;
      slowGravRefZ = tz;
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

    // Update the slow gravity reference (IIR, ~3.2 second time constant)
    // This is resistant to short falls and provides a stable 1g baseline for detection
    slowGravRefX = (slowGravRefX * 31 + avgx) / 32;
    slowGravRefY = (slowGravRefY * 31 + avgy) / 32;
    slowGravRefZ = (slowGravRefZ * 31 + avgz) / 32;

    // Freefall and sustained downward motion detection (only when enabled in settings)
    if (getSettingValue(SettingsOptions::FreefallDetection)) {
      // Divide by 4 before squaring to prevent int32 overflow (max 32767/4=8191, 8191^2*3 ~201M)
      int32_t gx        = slowGravRefX / 4;
      int32_t gy        = slowGravRefY / 4;
      int32_t gz        = slowGravRefZ / 4;
      int32_t refMagSq  = gx * gx + gy * gy + gz * gz;
      int32_t cx        = (int32_t)tx / 4;
      int32_t cy        = (int32_t)ty / 4;
      int32_t cz        = (int32_t)tz / 4;
      int32_t curMagSq  = cx * cx + cy * cy + cz * cz;

      if (refMagSq > 0) {
        if (curMagSq < refMagSq / 100) {
          // Magnitude < 10% of gravity: genuine freefall
          // Require 5 consecutive samples (~500ms) to reject brief bumps and normal motion
          freefallCounter++;
          if (freefallCounter >= 5) {
            freefallDetected = true;
          }
        } else {
          freefallCounter = 0;
        }
      }
    } else {
      freefallCounter  = 0;
      freefallDetected = false;
    }

    // Sum the deltas
    int32_t error = (abs(avgx - tx) + abs(avgy - ty) + abs(avgz - tz));
    // So now we have averages, we want to look if these are different by more
    // than the threshold

    // If movement has occurred then we update the tick timer
    bool overThreshold = error > threshold;
#ifdef ACCEL_EXITS_ON_MOVEMENT
    if (overThreshold) {
      tripCounter++;
      if (tripCounter > 2) {
        lastMovementTime = xTaskGetTickCount();
      }
    } else if (tripCounter > 0) {
      tripCounter = 0;
    }
#else
    if (overThreshold) {
      lastMovementTime = xTaskGetTickCount();
    }

#endif

    vTaskDelay(TICKS_100MS); // Slow down update rate
  }
}
