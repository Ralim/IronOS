#ifndef CORE_DRIVERS_ACCELEROMTERS_COMMON_H_
#define CORE_DRIVERS_ACCELEROMTERS_COMMON_H_
#include "configuration.h"
#if defined(ACCEL_I2CBB2)
#include "I2CBB2.hpp"
#define ACCEL_I2C_CLASS I2CBB2
#elif defined(ACCEL_I2CBB1)
#include "I2CBB1.hpp"
#define ACCEL_I2C_CLASS I2CBB1
#else
#include "I2C_Wrapper.hpp"
#define ACCEL_I2C_CLASS FRToSI2C
#endif

#endif