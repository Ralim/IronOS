#include "ts21.h"

#ifndef TS21_HOST_TEST
static uint32_t ts21_mmio_read(uint32_t address) { return *(volatile uint32_t *)(uintptr_t)address; }
static void     ts21_mmio_write(uint32_t address, uint32_t value) { *(volatile uint32_t *)(uintptr_t)address = value; }
#endif

static bool                uart_ready;
static bool                adc_ready;
static int16_t             adc_offset;
static struct TS21PDParser pd_parser;

static uint32_t irq_save(void) {
#if defined(TS21_IRONOS) && !defined(TS21_HOST_TEST)
  uint32_t state;
  __asm volatile("csrrci %0, mstatus, 8" : "=r"(state)::"memory");
  return state;
#else
  return 0;
#endif
}

static void irq_restore(uint32_t state) {
#if defined(TS21_IRONOS) && !defined(TS21_HOST_TEST)
  __asm volatile("csrw mstatus, %0" ::"r"(state) : "memory");
#else
  (void)state;
#endif
}

#ifdef TS21_HEATER_TEST
static bool     heater_sample_ready;
static bool     heater_active;
static uint32_t heater_sample_us;
static uint32_t heater_command_us;
static uint32_t heater_sample_error;
static uint32_t heater_fault_error;
static uint32_t heater_full_power_x10;
static uint16_t heater_max_compare;
static uint8_t  heater_fault_samples;
static int16_t  heater_temperature;
static uint16_t heater_power_x10;
#endif

static void modify(uint32_t address, uint32_t clear, uint32_t set) { ts21_mmio_write(address, (ts21_mmio_read(address) & ~clear) | set); }

static void gpio_mode(uint32_t port, unsigned pin, uint32_t mode) {
  const unsigned shift = (pin % 8) * 4;
  modify(port + (pin < 8 ? 0U : 4U), UINT32_C(15) << shift, mode << shift);
}

#ifdef TS21_IRONOS
void setWorklight(bool enabled) { ts21_mmio_write(TS21_GPIOA + (enabled ? 0x10U : 0x14U), TS21_WORKLIGHT); }
#endif

static bool wait_bits(uint32_t address, uint32_t mask, uint32_t expected) {
  for (unsigned remaining = 100000; remaining; --remaining) {
    if ((ts21_mmio_read(address) & mask) == expected) {
      return true;
    }
    ts21_uart_poll();
  }
  return false;
}

void ts21_heater_inhibit(void) {
  const uint32_t state = irq_save();
  modify(TS21_RCC + 0x18, 0, 4);
  // Stock firmware clears PA10 before sampling the thermocouple.
  ts21_mmio_write(TS21_GPIOA + 0x14, TS21_HEATER_EN);
  gpio_mode(TS21_GPIOA, 10, 2);
#ifdef TS21_HEATER_TEST
  modify(TS21_RCC + 0x18, 0, 1U << 11);
  ts21_mmio_write(TS21_TIM1 + 0x44, 0);
  ts21_mmio_write(TS21_TIM1 + 0x20, 0);
  ts21_mmio_write(TS21_TIM1, 0);
  heater_sample_ready = false;
  heater_active       = false;
  heater_power_x10    = 0;
#endif
  modify(TS21_RCC + 0x1C, 0, 2);
  ts21_mmio_write(TS21_TIM3, 0);
  ts21_mmio_write(TS21_TIM3 + 0x20, 0);
  ts21_mmio_write(TS21_TIM3 + 0x34, 0);
  ts21_mmio_write(TS21_GPIOA + 0x14, TS21_HEATER_IN);
  gpio_mode(TS21_GPIOA, 6, 2);
  irq_restore(state);
}

void ts21_watchdog_refresh(void) { ts21_mmio_write(TS21_IWDG, 0xAAAA); }

#ifdef TS21_HEATER_TEST
void ts21_heater_trip(uint32_t error) {
  ts21_heater_inhibit();
  ts21_status.errors |= error;
  ts21_heater_failed();
}

bool ts21_heater_reset_faults(void) {
  const uint32_t state = irq_save();
  const bool     ready = heater_sample_ready;
  ts21_heater_inhibit();
  heater_fault_samples = 0;
  heater_fault_error   = 0;
  if (!ready || heater_sample_error) {
    ts21_status.errors |= heater_sample_error ? heater_sample_error : TS21_ERROR_SENSOR;
  } else {
    ts21_status.errors &= ~TS21_HEATER_RECOVERABLE_ERRORS;
  }
  const bool recovered = !(ts21_status.errors & TS21_HEATER_INHIBIT_ERRORS);
  irq_restore(state);
  return recovered;
}

void ts21_heater_check(void) {
  const uint32_t state = irq_save();
  const uint32_t now   = ts21_micros();
  if (heater_active && ((ts21_status.errors & TS21_HEATER_INHIBIT_ERRORS) || !ts21_heater_permitted())) {
    ts21_heater_inhibit();
  }
  if (heater_active && (uint32_t)(now - heater_command_us) >= TS21_HEATER_COMMAND_US) {
    ts21_heater_inhibit();
  }
  irq_restore(state);
}

static void heater_record_sample(const struct TS21Sample *sample) {
  int16_t        tip, handle;
  const uint8_t  resistance = ts21_tip_resistance_x10(sample);
  const uint16_t voltage    = ts21_input_voltage_x10(sample->input);
  heater_sample_error       = 0;
  if (!ts21_tip_delta_c(sample->tip, &tip) || !ts21_handle_c_x10(sample->handle, &handle)) {
    heater_sample_error = TS21_ERROR_SENSOR;
  } else if (sample->tip_probe >= 4090 || sample->tip >= 4090 || sample->tip == 0 || sample->tip_probe <= sample->tip || resistance > TS21_HEATER_MAX_R_X10) {
    // ts21_tip_resistance_x10() reports 0 both for a genuine short and for a sample whose probe
    // phase never rose above the temperature phase. Only the measurable case is allowed to latch.
    heater_sample_error = TS21_ERROR_HEATER_OPEN;
  } else if (resistance < TS21_HEATER_MIN_R_X10) {
    heater_sample_error = TS21_ERROR_HEATER_SHORT;
  } else if (voltage < TS21_HEATER_MIN_INPUT_X10 || voltage > TS21_HEATER_MAX_INPUT_X10) {
    heater_sample_error = TS21_ERROR_HEATER_SUPPLY;
  } else {
    // The handle warms up in normal use; the thermistor curve itself is the only bound that matters.
    heater_temperature = tip + handle / 10;
    if (heater_temperature >= TS21_HEATER_CUTOFF_C) {
      heater_sample_error = TS21_ERROR_HEATER_TEMP;
    }
  }
  if (!heater_sample_error) {
    // Round resistance down and voltage up; saved voltage calibration cannot raise this cap.
    const uint32_t upper_voltage = voltage + 1U;
    heater_full_power_x10        = upper_voltage * upper_voltage / resistance;
    heater_max_compare           = (uint16_t)(TS21_HEATER_PERIOD * TS21_HEATER_POWER_X10 * resistance / (upper_voltage * upper_voltage));
    heater_sample_us             = ts21_micros();
    heater_sample_ready          = true;
    heater_fault_samples         = 0;
    heater_fault_error           = 0;
    return;
  }
  // A single reading never stops the iron: withhold power now and only latch once the same class of
  // hardware fault has been seen on consecutive samples.
  ts21_heater_inhibit();
  if (heater_fault_error != heater_sample_error) {
    heater_fault_error   = heater_sample_error;
    heater_fault_samples = 0;
  }
  if (heater_fault_samples < TS21_HEATER_FAULT_SAMPLES) {
    heater_fault_samples++;
  }
  if ((heater_sample_error & TS21_HEATER_LATCHING_ERRORS) && heater_fault_samples >= TS21_HEATER_FAULT_SAMPLES && ts21_heater_permitted()) {
    ts21_heater_trip(heater_sample_error);
  }
}

uint32_t getTipX10Watts(void) { return heater_active && (ts21_mmio_read(TS21_TIM1) & 1U) ? heater_power_x10 : 0; }

static uint32_t heater_compare(uint8_t pulse) {
  uint32_t compare = (uint32_t)pulse * TS21_HEATER_PERIOD / 255U;
  if (compare > heater_max_compare) {
    compare = heater_max_compare;
  }
  if (compare > TS21_HEATER_PERIOD * 85 / 100) {
    compare = TS21_HEATER_PERIOD * 85 / 100;
  }
  return compare;
}
#endif

void setTipPWM(uint8_t pulse, bool fast) {
  (void)fast;
#ifdef TS21_HEATER_TEST
  const uint32_t state = irq_save();
  ts21_heater_check();
  const bool     ready = heater_sample_ready;
  const uint32_t now   = ts21_micros();
  const uint32_t error = ts21_status.errors & TS21_HEATER_INHIBIT_ERRORS;
  ts21_heater_inhibit();
  // Anything unexpected simply withholds this command; recording the sample decides what latches.
  if (!pulse || !ts21_heater_permitted() || error || heater_sample_error || !ready || heater_temperature >= TS21_HEATER_CUTOFF_C) {
    irq_restore(state);
    return;
  }
  const uint32_t compare = heater_compare(pulse);
  if (compare) {
    heater_command_us = now;
    heater_active     = true;
    heater_power_x10  = (uint16_t)(compare * heater_full_power_x10 / TS21_HEATER_PERIOD);

    // Reset inherited timer/remap state. Only TIM3 CH1 (PA6) and TIM1 CH3 (PA10) are enabled.
    modify(TS21_RCC + 0x18, 0, 1);
    modify(0x40010004, 0xCC0, 0);
    modify(TS21_RCC + 0x0C, 0, 1U << 11);
    modify(TS21_RCC + 0x0C, 1U << 11, 0);
    modify(TS21_RCC + 0x10, 0, 2);
    modify(TS21_RCC + 0x10, 2, 0);

    ts21_mmio_write(TS21_TIM3 + 0x28, 0);
    ts21_mmio_write(TS21_TIM3 + 0x2C, TS21_HEATER_PERIOD - 1);
    ts21_mmio_write(TS21_TIM3 + 0x34, compare);
    ts21_mmio_write(TS21_TIM3 + 0x18, 0x68);
    ts21_mmio_write(TS21_TIM3 + 0x14, 1);
    ts21_mmio_write(TS21_TIM3 + 0x20, 1);
    gpio_mode(TS21_GPIOA, 6, 0xB);
    ts21_mmio_write(TS21_TIM3, 0x81);

    // PA10 expires in hardware even if the PID, interrupts or watchdog servicing stall.
    // PWM2 starts low, is high at counts 1..120, and returns low on the one-pulse update.
    ts21_mmio_write(TS21_TIM1 + 0x28, TS21_CLOCK_HZ / 1000U - 1);
    ts21_mmio_write(TS21_TIM1 + 0x2C, TS21_HEATER_WINDOW_MS);
    ts21_mmio_write(TS21_TIM1 + 0x3C, 1);
    ts21_mmio_write(TS21_TIM1 + 0x1C, 0x78);
    ts21_mmio_write(TS21_TIM1 + 0x14, 1);
    ts21_mmio_write(TS21_TIM1 + 0x20, 0x100);
    ts21_mmio_write(TS21_TIM1 + 0x44, 0x8000);
    gpio_mode(TS21_GPIOA, 10, 0xB);
    ts21_mmio_write(TS21_TIM1, 0x89);
  }
  irq_restore(state);
#else
  (void)pulse;
  ts21_heater_inhibit();
#endif
}

uint64_t ts21_timer_now(void) {
  uint32_t high, low;
  do {
    high = ts21_mmio_read(TS21_SYSTICK + 12);
    low  = ts21_mmio_read(TS21_SYSTICK + 8);
  } while (high != ts21_mmio_read(TS21_SYSTICK + 12));
  return ((uint64_t)high << 32) | low;
}

uint32_t ts21_micros(void) {
#if TS21_TIMER_TICKS_PER_US == 1
  return ts21_mmio_read(TS21_SYSTICK + 8);
#else
  return (uint32_t)(ts21_timer_now() / TS21_TIMER_TICKS_PER_US);
#endif
}

bool ts21_delay_us(uint32_t duration) {
  if (duration > 100000) {
    ts21_status.errors |= TS21_ERROR_ARGUMENT;
    return false;
  }
  const uint32_t start     = ts21_mmio_read(TS21_SYSTICK + 8);
  uint32_t       remaining = duration * 16 + 128;
  while ((uint32_t)(ts21_mmio_read(TS21_SYSTICK + 8) - start) < duration * TS21_TIMER_TICKS_PER_US) {
    if (!remaining--) {
      ts21_status.errors |= TS21_ERROR_CLOCK;
      return false;
    }
    ts21_uart_poll();
  }
  return true;
}

static bool clock_init(void) {
  modify(TS21_RCC, 0, 1);
  if (!wait_bits(TS21_RCC, 2, 2)) {
    return false;
  }
  modify(TS21_RCC + 4, 3, 0);
  if (!wait_bits(TS21_RCC + 4, 12, 0)) {
    return false;
  }
  modify(TS21_RCC, UINT32_C(1) << 24, 0);
  if (!wait_bits(TS21_RCC, UINT32_C(1) << 25, 0)) {
    return false;
  }
#ifdef TS21_IRONOS
  // WCH HSI x8: HCLK 64 MHz, APB1 32 MHz, APB2 64 MHz, ADC 8 MHz.
  modify(0x40023800, 0, 0x10);
  modify(TS21_RCC + 4, 0x3FFFF0, 0x18C400);
  modify(TS21_RCC, 0, UINT32_C(1) << 24);
  if (!wait_bits(TS21_RCC, UINT32_C(1) << 25, UINT32_C(1) << 25)) {
    return false;
  }
  modify(TS21_RCC + 4, 3, 2);
  if (!wait_bits(TS21_RCC + 4, 12, 8)) {
    return false;
  }
#else
  modify(TS21_RCC + 4, 0xFFF0, 0);
#endif
  ts21_mmio_write(TS21_SYSTICK, 0);
  ts21_mmio_write(TS21_SYSTICK + 4, 0);
  ts21_mmio_write(TS21_SYSTICK + 8, 0);
  ts21_mmio_write(TS21_SYSTICK + 12, 0);
  ts21_mmio_write(TS21_SYSTICK + 16, UINT32_MAX);
  ts21_mmio_write(TS21_SYSTICK + 20, UINT32_MAX);
  ts21_mmio_write(TS21_SYSTICK, 1); // Free-running HCLK/8; no interrupts yet.
  return true;
}

static void uart_init(void) {
  modify(TS21_RCC + 0x1C, 0, UINT32_C(1) << 17);
  gpio_mode(TS21_GPIOA, 3, 4);
  ts21_mmio_write(TS21_USART2 + 12, 0);
  ts21_mmio_write(TS21_USART2 + 16, 0);
  ts21_mmio_write(TS21_USART2 + 20, 0);
  ts21_mmio_write(TS21_USART2 + 8, (TS21_APB1_HZ + 4800) / 9600);
  ts21_mmio_write(TS21_USART2 + 12, 0x2004); // UE + RE only. Never enable TX.
  pd_parser.used = 0;
  uart_ready     = true;
}

static void uart_poll(void) {
  if (!uart_ready) {
    return;
  }
  for (unsigned remaining = 8; remaining; --remaining) {
    const uint32_t status = ts21_mmio_read(TS21_USART2);
    if (status & 15U) {
      (void)ts21_mmio_read(TS21_USART2 + 4);
      pd_parser.used = 0;
      ts21_status.errors |= TS21_ERROR_UART;
      continue;
    }
    if (!(status & 0x20U)) {
      return;
    }
    const uint8_t      byte = (uint8_t)ts21_mmio_read(TS21_USART2 + 4);
    struct TS21PDFrame frame;
    if (ts21_pd_feed(&pd_parser, byte, &frame)) {
      ts21_status.pd_voltage = frame.voltage;
      ts21_status.pd_status  = frame.status;
      ts21_status.pd_last_us = ts21_micros();
      ++ts21_status.pd_frames;
    }
  }
}

void ts21_uart_poll(void) {
  const uint32_t state = irq_save();
  uart_poll();
  irq_restore(state);
}

static bool adc_init(void) {
  gpio_mode(TS21_GPIOA, 1, 0);
  gpio_mode(TS21_GPIOA, 2, 0);
  gpio_mode(TS21_GPIOA, 4, 0);
  gpio_mode(TS21_GPIOB, 1, 0);
  modify(TS21_RCC + 0x18, 0, UINT32_C(1) << 9);
  modify(TS21_RCC + 0x0C, 0, UINT32_C(1) << 9);
  modify(TS21_RCC + 0x0C, UINT32_C(1) << 9, 0);
  ts21_mmio_write(TS21_ADC1 + 4, 0);
  ts21_mmio_write(TS21_ADC1 + 0x2C, 0);
  ts21_mmio_write(TS21_ADC1 + 0x30, 0);
  ts21_mmio_write(TS21_ADC1 + 8, 0xE0001);
  if (!ts21_delay_us(10)) {
    return false;
  }
  uint16_t calibration[10];
  for (unsigned i = 0; i < 10; ++i) {
    modify(TS21_ADC1 + 8, 0, 8);
    if (!wait_bits(TS21_ADC1 + 8, 8, 0)) {
      return false;
    }
    modify(TS21_ADC1 + 8, 0, 4);
    if (!wait_bits(TS21_ADC1 + 8, 4, 0)) {
      return false;
    }
    calibration[i] = (uint16_t)ts21_mmio_read(TS21_ADC1 + 0x4C);
    for (unsigned j = i; j && calibration[j] < calibration[j - 1]; --j) {
      const uint16_t previous = calibration[j - 1];
      calibration[j - 1]      = calibration[j];
      calibration[j]          = previous;
    }
  }
  // CH32V203 D6 calibration alternates 1024/2048 reference levels; discard the outer two pairs.
  unsigned sums[2] = {0, 0}, counts[2] = {0, 0};
  for (unsigned i = 2; i < 8; ++i) {
    if (calibration[i] > 4095) {
      return false;
    }
    const unsigned group = calibration[i] > 1536;
    sums[group] += calibration[i];
    ++counts[group];
  }
  int32_t correction = 0;
  for (unsigned group = 0; group < 2; ++group) {
    if (counts[group]) {
      correction += (int32_t)((group + 1) * 1024) - (int32_t)((sums[group] * 2 + counts[group]) / (counts[group] * 2));
    }
  }
  adc_offset = (int16_t)(correction / 2 + correction % 2);
  return true;
}

bool ts21_adc_read(uint8_t channel, uint16_t *value) {
  if (value == NULL || (channel != 1 && channel != 2 && channel != 4 && channel != 9)) {
    ts21_status.errors |= TS21_ERROR_ARGUMENT;
    return false;
  }
  if (!adc_ready) {
    ts21_status.errors |= TS21_ERROR_ADC;
    return false;
  }
  const unsigned shift = channel * 3;
  modify(TS21_ADC1 + 0x10, UINT32_C(7) << shift, UINT32_C(7) << shift);
  ts21_mmio_write(TS21_ADC1 + 0x34, channel);
  (void)ts21_mmio_read(TS21_ADC1 + 0x4C);
  ts21_mmio_write(TS21_ADC1, 0);
  modify(TS21_ADC1 + 8, 0, 0x500000);
  if (!wait_bits(TS21_ADC1, 2, 2)) {
    ts21_status.errors |= TS21_ERROR_ADC;
    return false;
  }
  const uint16_t raw       = (uint16_t)(ts21_mmio_read(TS21_ADC1 + 0x4C) & 0xFFFU);
  const int32_t  corrected = (int32_t)raw + adc_offset;
  *value                   = raw == 0 || corrected < 0 ? 0 : raw == 4095 || corrected > 4095 ? 4095 : (uint16_t)corrected;
  return true;
}

bool ts21_sample_sensors(struct TS21Sample *sample) {
  ts21_heater_inhibit();
  if (sample == NULL) {
    ts21_status.errors |= TS21_ERROR_ARGUMENT;
    return false;
  }
  ts21_mmio_write(TS21_GPIOA + 0x14, 1);
  gpio_mode(TS21_GPIOA, 0, 2);
  ts21_mmio_write(TS21_GPIOB + 0x10, 1U << 15);
  gpio_mode(TS21_GPIOB, 15, 2);
  bool ok = ts21_delay_us(200);
  ts21_mmio_write(TS21_GPIOA + 0x10, 1);
  ok = ok && ts21_delay_us(600) && ts21_adc_read(1, &sample->tip_probe);
  ts21_mmio_write(TS21_GPIOA + 0x14, 1);
  ok = ok && ts21_delay_us(800) && ts21_adc_read(1, &sample->tip) && ts21_adc_read(2, &sample->handle) && ts21_adc_read(9, &sample->input);
  ts21_mmio_write(TS21_GPIOB + 0x14, 1U << 15);
  ok = ok && ts21_adc_read(4, &sample->auxiliary);
  ts21_heater_inhibit();
#ifdef TS21_HEATER_TEST
  if (ok) {
    heater_record_sample(sample);
  }
#endif
  return ok;
}

uint8_t ts21_buttons(void) {
  const uint32_t pins = ts21_mmio_read(TS21_GPIOA + 8);
#ifdef TS21_IRONOS
  return (uint8_t)(((pins & (1U << 7)) ? 0U : 1U) | ((pins & (1U << 5)) ? 0U : 2U));
#else
  return (uint8_t)(((pins & (1U << 5)) ? 0U : 1U) | ((pins & (1U << 7)) ? 0U : 2U));
#endif
}

static void scl_low(void) { ts21_mmio_write(TS21_GPIOB + 0x14, TS21_SCL); }
static void sda_low(void) { ts21_mmio_write(TS21_GPIOB + 0x14, TS21_SDA); }
static void sda_release(void) { ts21_mmio_write(TS21_GPIOB + 0x10, TS21_SDA); }

static bool scl_release(void) {
  ts21_mmio_write(TS21_GPIOB + 0x10, TS21_SCL);
  if (!wait_bits(TS21_GPIOB + 8, TS21_SCL, TS21_SCL)) {
    ts21_status.errors |= TS21_ERROR_I2C_BUS;
    return false;
  }
  return true;
}

static bool i2c_stop(void) {
  scl_low();
  sda_low();
  if (!ts21_delay_us(4) || !scl_release() || !ts21_delay_us(4)) {
    sda_release();
    return false;
  }
  sda_release();
  return ts21_delay_us(4);
}

static bool i2c_start(void) {
  sda_release();
  if (!scl_release() || !ts21_delay_us(4)) {
    return false;
  }
  if (!(ts21_mmio_read(TS21_GPIOB + 8) & TS21_SDA)) {
    ts21_status.errors |= TS21_ERROR_I2C_BUS;
    return false;
  }
  sda_low();
  if (!ts21_delay_us(4)) {
    return false;
  }
  scl_low();
  return true;
}

static bool i2c_byte(uint8_t value) {
#ifdef TS21_IRONOS
  const uint32_t half_period_us = 2;
#else
  const uint32_t half_period_us = 4;
#endif
  for (unsigned bit = 0; bit < 8; ++bit) {
    if (value & 0x80U) {
      sda_release();
    } else {
      sda_low();
    }
    if (!ts21_delay_us(half_period_us) || !scl_release() || !ts21_delay_us(half_period_us)) {
      return false;
    }
    scl_low();
    value = (uint8_t)(value << 1);
  }
  sda_release();
  if (!ts21_delay_us(half_period_us) || !scl_release() || !ts21_delay_us(half_period_us)) {
    return false;
  }
  const bool acknowledged = !(ts21_mmio_read(TS21_GPIOB + 8) & TS21_SDA);
  scl_low();
  if (!acknowledged) {
    ts21_status.errors |= TS21_ERROR_I2C_NACK;
  }
  return acknowledged;
}

bool ts21_oled_write(uint8_t control, const uint8_t *data, size_t size) {
  if ((control != 0 && control != 0x40) || data == NULL || size == 0 || size > 128) {
    ts21_status.errors |= TS21_ERROR_ARGUMENT;
    return false;
  }
  bool success = i2c_start() && i2c_byte(0x78) && i2c_byte(control);
  for (size_t i = 0; success && i < size; ++i) {
    success = i2c_byte(data[i]);
  }
  const bool stopped = i2c_stop();
  return success && stopped;
}

bool ts21_board_init(void) {
  uart_ready = false;
  adc_ready  = false;
  ts21_heater_inhibit();
#ifdef TS21_IRONOS
  setWorklight(false);
  gpio_mode(TS21_GPIOA, 9, 3);
#endif
#ifdef TS21_HEATER_TEST
  heater_fault_samples = 0;
  heater_fault_error   = 0;
  heater_sample_error  = TS21_ERROR_SENSOR;
#endif
  for (unsigned bank = 0; bank < 8; ++bank) {
    ts21_mmio_write(0xE000E180 + bank * 4, UINT32_MAX);
    ts21_mmio_write(0xE000E280 + bank * 4, UINT32_MAX);
  }
  if (!clock_init()) {
    ts21_status.errors |= TS21_ERROR_CLOCK;
    return false;
  }
  modify(TS21_RCC + 0x18, 0, 8);
  ts21_mmio_write(TS21_GPIOA + 0x10, (1U << 5) | (1U << 7));
  gpio_mode(TS21_GPIOA, 5, 8);
  gpio_mode(TS21_GPIOA, 7, 8);
  ts21_mmio_write(TS21_GPIOB + 0x10, TS21_SCL | TS21_SDA);
  gpio_mode(TS21_GPIOB, 10, 6);
  gpio_mode(TS21_GPIOB, 11, 6);
  uart_init();
  adc_ready = adc_init();
  if (!adc_ready) {
    ts21_status.errors |= TS21_ERROR_ADC;
  }
#ifdef TS21_HEATER_TEST
  // Own the independent watchdog rather than relying on the bootloader's configuration.
  ts21_mmio_write(TS21_IWDG, 0x5555);
  ts21_mmio_write(TS21_IWDG + 4, 3);
  ts21_mmio_write(TS21_IWDG + 8, 624); // Nominal 500 ms at 40 kHz LSI, prescaler 32.
  if (!wait_bits(TS21_IWDG + 12, 3, 0)) {
    ts21_status.errors |= TS21_ERROR_CLOCK;
    return false;
  }
  ts21_mmio_write(TS21_IWDG, 0xCCCC);
  ts21_watchdog_refresh();
#endif
  ts21_heater_inhibit();
  return true;
}
