#ifndef BSP_TS21_BRINGUP_H
#define BSP_TS21_BRINGUP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TS21_RCC       UINT32_C(0x40021000)
#define TS21_GPIOA     UINT32_C(0x40010800)
#define TS21_GPIOB     UINT32_C(0x40010C00)
#define TS21_TIM1      UINT32_C(0x40012C00)
#define TS21_TIM3      UINT32_C(0x40000400)
#define TS21_ADC1      UINT32_C(0x40012400)
#define TS21_USART2    UINT32_C(0x40004400)
#define TS21_IWDG      UINT32_C(0x40003000)
#define TS21_SYSTICK   UINT32_C(0xE000F000)
#ifdef TS21_IRONOS
#define TS21_CLOCK_HZ UINT32_C(64000000)
#define TS21_APB1_HZ  (TS21_CLOCK_HZ / 2)
#else
#define TS21_CLOCK_HZ UINT32_C(8000000)
#define TS21_APB1_HZ  TS21_CLOCK_HZ
#endif
#define TS21_TIMER_HZ           (TS21_CLOCK_HZ / 8)
#define TS21_TIMER_TICKS_PER_US (TS21_TIMER_HZ / 1000000)
#define TS21_HEATER_EN (UINT32_C(1) << 10)
#define TS21_WORKLIGHT          (UINT32_C(1) << 9)
#define TS21_HEATER_IN (UINT32_C(1) << 6)
#define TS21_SCL       (UINT32_C(1) << 10)
#define TS21_SDA       (UINT32_C(1) << 11)

#ifdef TS21_HEATER_TEST
#ifndef TS21_IRONOS
#error "The standalone TS21 diagnostic must never enable heating"
#endif
#define TS21_HEATER_POWER_X10     1000
#define TS21_DEFAULT_TEMP_C       320
#define TS21_HEATER_CUTOFF_C      500
#define TS21_HEATER_COMMAND_US    UINT32_C(150000)
#define TS21_HEATER_WINDOW_MS     120U
#define TS21_HEATER_PERIOD        (TS21_CLOCK_HZ / 100000U)
#define TS21_HEATER_MIN_R_X10     10U
#define TS21_HEATER_MAX_R_X10     100U
#define TS21_HEATER_MIN_INPUT_X10 45U
// Covers the hardware's 24 V DC rating plus the tolerance and load-release overshoot of a 20 V supply.
#define TS21_HEATER_MAX_INPUT_X10 240U
// Consecutive bad samples before a hardware fault latches; transient readings only skip a command.
#define TS21_HEATER_FAULT_SAMPLES 4U
#endif

enum TS21Error {
  TS21_ERROR_CLOCK        = 1U << 0,
  TS21_ERROR_ADC          = 1U << 1,
  TS21_ERROR_I2C_BUS      = 1U << 2,
  TS21_ERROR_I2C_NACK     = 1U << 3,
  TS21_ERROR_UART         = 1U << 4,
  TS21_ERROR_ARGUMENT     = 1U << 5,
  TS21_ERROR_TRAP         = 1U << 6,
  TS21_ERROR_FLASH        = 1U << 7,
  TS21_ERROR_SENSOR       = 1U << 8,
  TS21_ERROR_HEATER_SHORT = 1U << 9,
  // Bits 10, 11 and 13 were bring-up watchdogs that this firmware no longer raises; the numbering is
  // kept so that a code read off an older build still decodes to the same meaning.
  TS21_ERROR_HEATER_STALE   = 1U << 10,
  TS21_ERROR_HEATER_TIMEOUT = 1U << 11,
  TS21_ERROR_HEATER_TEMP    = 1U << 12,
  TS21_ERROR_HEATER_NO_RISE = 1U << 13,
  TS21_ERROR_HEATER_SUPPLY  = 1U << 14,
  TS21_ERROR_HEATER_OPEN    = 1U << 15,
  TS21_ERROR_HEATER_MONITOR = 1U << 16,
};

#ifdef TS21_HEATER_TEST
// Passive UART telemetry is not a heater input; retain its diagnostic flag without inhibiting output.
#define TS21_HEATER_INHIBIT_ERRORS (UINT32_MAX ^ TS21_ERROR_UART)
#define TS21_HEATER_RECOVERABLE_ERRORS                                                                                                                                                                 \
  (TS21_ERROR_ADC | TS21_ERROR_SENSOR | TS21_ERROR_HEATER_SHORT | TS21_ERROR_HEATER_STALE | TS21_ERROR_HEATER_TIMEOUT | TS21_ERROR_HEATER_TEMP | TS21_ERROR_HEATER_NO_RISE |                           \
   TS21_ERROR_HEATER_SUPPLY | TS21_ERROR_HEATER_OPEN | TS21_ERROR_HEATER_MONITOR)
// Only an unreadable sensor or a genuinely shorted tip stops the iron. An open tip, a supply excursion
// and a post-pulse overtemperature reading are self-correcting, so they withhold power without latching.
#define TS21_HEATER_LATCHING_ERRORS (TS21_ERROR_ADC | TS21_ERROR_SENSOR | TS21_ERROR_HEATER_SHORT)
#endif

struct TS21PDParser {
  uint8_t bytes[4];
  uint8_t used;
};

struct TS21PDFrame {
  uint8_t voltage;
  uint8_t status;
};

struct TS21Status {
  uint32_t signature;
  uint32_t errors;
  uint32_t iterations;
  uint32_t pd_frames;
  uint32_t pd_last_us;
  uint16_t raw_adc[4];
  uint8_t  adc_valid_mask;
  uint8_t  buttons;
  uint8_t  pd_voltage;
  uint8_t  pd_status;
  uint8_t  page;
};

struct TS21Sample {
  uint16_t tip_probe;
  uint16_t tip;
  uint16_t handle;
  uint16_t input;
  uint16_t auxiliary;
};

extern volatile struct TS21Status ts21_status;

uint8_t ts21_crc8(const uint8_t *data, size_t size);
bool    ts21_pd_feed(struct TS21PDParser *parser, uint8_t byte, struct TS21PDFrame *frame);

void     ts21_heater_inhibit(void);
void     setTipPWM(uint8_t pulse, bool fast);
#ifdef TS21_IRONOS
void setWorklight(bool enabled);
#endif
void     ts21_watchdog_refresh(void);
bool     ts21_board_init(void);
uint64_t ts21_timer_now(void);
uint32_t ts21_micros(void);
bool     ts21_delay_us(uint32_t duration);
uint8_t  ts21_buttons(void);
bool     ts21_adc_read(uint8_t channel, uint16_t *value);
bool     ts21_sample_sensors(struct TS21Sample *sample);
bool     ts21_tip_delta_c(uint16_t raw, int16_t *value);
uint8_t  ts21_tip_resistance_x10(const struct TS21Sample *sample);
bool     ts21_handle_c_x10(uint16_t raw, int16_t *value);
uint16_t ts21_input_voltage_x10(uint16_t raw);
void     ts21_uart_poll(void);
bool     ts21_oled_write(uint8_t control, const uint8_t *data, size_t size);
void     ts21_fault(uint32_t error) __attribute__((noreturn));

#ifdef TS21_HEATER_TEST
bool     ts21_heater_permitted(void);
void     ts21_heater_failed(void);
void     ts21_heater_acknowledge(void);
bool     ts21_heater_reset_faults(void);
void     ts21_heater_check(void);
void     ts21_heater_trip(uint32_t error);
uint32_t getTipX10Watts(void);
#endif

#ifdef TS21_HOST_TEST
uint32_t ts21_mmio_read(uint32_t address);
void     ts21_mmio_write(uint32_t address, uint32_t value);
#endif

#ifdef __cplusplus
}
#endif

#endif
