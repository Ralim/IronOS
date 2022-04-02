#ifndef __GSL61XX__
#define __GSL61XX__

#include "hal_gpio.h"
#include <stdint.h>

#define BYTE uint8_t

#define FP_CS_PIN    GPIO_PIN_10
#define FP_RESET_PIN GPIO_PIN_6

#define FP_CS_DISABLE()           \
    do {                          \
        gpio_write(FP_CS_PIN, 1); \
    } while (0)
#define FP_CS_ENABLE()            \
    do {                          \
        gpio_write(FP_CS_PIN, 0); \
    } while (0)

#define FP_RST_HIGH()                \
    do {                             \
        gpio_write(FP_RESET_PIN, 1); \
    } while (0)
#define FP_RST_LOW()                 \
    do {                             \
        gpio_write(FP_RESET_PIN, 0); \
    } while (0)

#define SPI_DELAY 5

#define FINGER_UP   0
#define FINGER_DOWN 1

#define TUNE_IMAGE_W 32
#define TUNE_IMAGE_H 32

//#define TUNE_TEST
#ifdef TUNE_TEST
#define DAC_TUNE_OFFSET_MAX 0x80
#define DAC_TUNE_OFFSET_MIN 0x80
#define DAC_TUNE_STEP_MAX   0x08
#define DAC_TUNE_CNT_MAX    5
#else
#define DAC_TUNE_OFFSET_MAX 0x20
#define DAC_TUNE_OFFSET_MIN 0x10
#define DAC_TUNE_STEP_MAX   0x08
#define DAC_TUNE_CNT_MAX    5
#endif

typedef struct Chip_Config {
    uint32_t Addr;
    uint32_t Data;
} Chip_Config_T;

typedef struct GSL_FP_Config {
    Chip_Config_T *Config_NormalMode;
    uint32_t Config_NormalMode_Len;
} GSL_FP_Config_T;

typedef struct GSL_FP_Register {
    uint32_t RegGain12;
    uint32_t RegGain34;
    uint32_t RegDac;
} GSL_FP_Register_T;

typedef struct GSL_FP_Sensor {
    uint32_t ImageW;
    uint32_t ImageH;
    uint32_t ImageSize;
    uint8_t Gain12Ratio[8];
    uint8_t Gain34Ratio[8];
} GSL_FP_Sensor_T;

typedef struct GSL_FP_Tune {
    uint8_t DacShift;
    uint8_t DacMax;
    uint8_t DacMin;
} GSL_FP_Tune_T;

typedef struct GSL_FP_Detect {
    uint32_t DownThreshold;
    uint32_t UpThreshold;
    uint32_t DetGain12;
    uint32_t DetGain34;
    uint32_t DetDac;
} GSL_FP_Detect_T;

typedef struct GSL_FP_Capture {
    uint32_t CapGain12;
    uint32_t CapGain34;
    uint32_t CapDac;
} GSL_FP_Capture_T;

typedef struct GSL_FP_Data {
    GSL_FP_Config_T Config;
    GSL_FP_Register_T Reg;
    GSL_FP_Sensor_T Sensor;
    GSL_FP_Tune_T Tune;
    GSL_FP_Detect_T Detect;
    GSL_FP_Capture_T Capture;
} GSL_FP_Data_T;

uint8_t GSL61xx_init(void);

BYTE CaptureGSL61xx(BYTE *pImageBmp);

void GSL_FP_CaptureImage(BYTE *pImageBmp);
void GSL_FP_ReadChipID(uint32_t *ChipID);
void GSL_FP_ReadImageData(uint32_t Page, uint8_t Reg, uint8_t *Buf, uint32_t Len);
#endif