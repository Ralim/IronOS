#include "ts21.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

volatile struct TS21Status ts21_status;

struct Register {
  uint32_t address;
  uint32_t value;
};

static struct Register registers[128];
static struct Register writes[4096];
static unsigned        register_count;
static unsigned        write_count;
static uint64_t        ticks;
static bool            clock_ready;
static bool            clock_running;
static bool            hsi_selected;
static bool            pll_ready;
static bool            pll_selected;
static bool            pll_stuck;
static bool            calibration_complete;
static bool            conversion_complete;
static uint32_t        input_a;
static bool            scl_stuck;
static bool            sda_stuck;
static bool            acknowledge;
static bool            i2c_active;
static unsigned        i2c_pulses;
static uint64_t        i2c_rise;
static uint64_t        i2c_fall;
static uint8_t         i2c_byte;
static uint8_t         i2c_bytes[130];
static unsigned        i2c_count;
static uint8_t         uart_bytes[32];
static unsigned        uart_size;
static unsigned        uart_position;
static uint32_t        uart_error;
static uint16_t        adc_value;
#ifdef TS21_HEATER_TEST
static bool              heater_permitted;
static unsigned          heater_failures;
static bool              use_adc_fixture;
static struct TS21Sample adc_fixture;
static uint64_t          gate_started_ticks;

bool ts21_heater_permitted(void) { return heater_permitted; }
void ts21_heater_failed(void) {
  ++heater_failures;
  heater_permitted = false;
}
#endif

static uint32_t *reg(uint32_t address) {
  for (unsigned i = 0; i < register_count; ++i) {
    if (registers[i].address == address) {
      return &registers[i].value;
    }
  }
  assert(register_count < sizeof(registers) / sizeof(registers[0]));
  registers[register_count].address = address;
  registers[register_count].value   = 0;
  return &registers[register_count++].value;
}

#ifdef TS21_HEATER_TEST
static void advance_gate(void) {
  if (*reg(TS21_TIM1) & 1U) {
    const uint64_t count = (ticks - gate_started_ticks) * 8U / (*reg(TS21_TIM1 + 0x28) + 1U);
    if (count > *reg(TS21_TIM1 + 0x2C) && (*reg(TS21_TIM1) & 8U)) {
      *reg(TS21_TIM1) &= ~1U;
      *reg(TS21_TIM1 + 0x24) = 0;
    } else {
      *reg(TS21_TIM1 + 0x24) = (uint32_t)count;
    }
  }
}

static bool gate_high(void) {
  advance_gate();
  return ((*reg(TS21_GPIOA + 4) >> 8) & 15) == 0xB && (*reg(TS21_TIM1 + 0x44) & 0x8000) && (*reg(TS21_TIM1 + 0x20) & 0x100) && (*reg(TS21_TIM1 + 0x24) >= *reg(TS21_TIM1 + 0x3C));
}
#endif

uint32_t ts21_mmio_read(uint32_t address) {
#ifdef TS21_HEATER_TEST
  if (address >= TS21_TIM1 && address < TS21_TIM1 + 0x50) {
    advance_gate();
  }
#endif
  if (address == TS21_RCC) {
    uint32_t result = *reg(address) & ~(2U | (1U << 25));
    if (clock_ready) {
      result |= 2U;
    }
    if (pll_stuck || (pll_ready && (result & (1U << 24)))) {
      result |= 1U << 25;
    }
    return result;
  }
  if (address == TS21_RCC + 4) {
    const uint32_t requested = *reg(address) & 3;
    if ((requested == 0 && hsi_selected) || (requested == 2 && pll_selected)) {
      *reg(address) = (*reg(address) & ~12U) | (requested << 2);
    }
    return *reg(address);
  }
  if (address == TS21_SYSTICK + 8) {
    if (clock_running && (*reg(TS21_SYSTICK) & 1U)) {
      ticks += TS21_TIMER_TICKS_PER_US;
    }
    return (uint32_t)ticks;
  }
  if (address == TS21_SYSTICK + 12) {
    return (uint32_t)(ticks >> 32);
  }
  if (address == TS21_ADC1 + 8 && calibration_complete) {
    if (*reg(address) & 4U) {
      *reg(TS21_ADC1 + 0x4C) = 1024;
    }
    *reg(address) &= ~12U;
  }
  if (address == TS21_GPIOA + 8) {
    return input_a;
  }
  if (address == TS21_GPIOB + 8) {
    uint32_t result = *reg(TS21_GPIOB + 12);
    if (scl_stuck) {
      result &= ~TS21_SCL;
    }
    if (sda_stuck || (i2c_active && i2c_pulses && i2c_pulses % 9 == 0 && acknowledge)) {
      result &= ~TS21_SDA;
    }
    return result;
  }
  if (address == TS21_USART2) {
    return uart_error | (uart_position < uart_size ? 0x20U : 0U);
  }
  if (address == TS21_USART2 + 4) {
    uart_error = 0;
    return uart_position < uart_size ? uart_bytes[uart_position++] : 0;
  }
  return *reg(address);
}

void ts21_mmio_write(uint32_t address, uint32_t value) {
  assert(write_count < sizeof(writes) / sizeof(writes[0]));
  writes[write_count++] = (struct Register){address, value};
  *reg(address)         = value;
#ifdef TS21_HEATER_TEST
  if ((address == TS21_RCC + 0x0C && (value & (1U << 11))) || (address == TS21_RCC + 0x10 && (value & 2))) {
    const uint32_t base = address == TS21_RCC + 0x0C ? TS21_TIM1 : TS21_TIM3;
    for (unsigned i = 0; i < register_count; ++i) {
      if (registers[i].address >= base && registers[i].address < base + 0x50) {
        registers[i].value = 0;
      }
    }
  }
  if (address == TS21_TIM1 && (value & 1U)) {
    gate_started_ticks = ticks;
  }
#endif
  if (address == TS21_GPIOA + 0x10) {
    *reg(TS21_GPIOA + 12) |= value;
  } else if (address == TS21_GPIOA + 0x14) {
    *reg(TS21_GPIOA + 12) &= ~value;
  } else if (address == TS21_GPIOB + 0x10 || address == TS21_GPIOB + 0x14) {
    const uint32_t old    = *reg(TS21_GPIOB + 12);
    const uint32_t next   = address == TS21_GPIOB + 0x10 ? old | value : old & ~value;
    *reg(TS21_GPIOB + 12) = next;
    if ((old & TS21_SCL) && (next & TS21_SCL)) {
      if ((old & TS21_SDA) && !(next & TS21_SDA)) {
        i2c_active = true;
        i2c_pulses = 0;
        i2c_byte   = 0;
      } else if (!(old & TS21_SDA) && (next & TS21_SDA)) {
        i2c_active = false;
      }
    }
    if (i2c_active && (old & TS21_SCL) && !(next & TS21_SCL)) {
      if (i2c_pulses) {
        assert(ticks - i2c_rise >= 2 * TS21_TIMER_TICKS_PER_US);
      }
      i2c_fall = ticks;
    }
    if (i2c_active && !(old & TS21_SCL) && (next & TS21_SCL)) {
      assert(ticks - i2c_fall >= 2 * TS21_TIMER_TICKS_PER_US);
      i2c_rise = ticks;
      ++i2c_pulses;
      if (i2c_pulses % 9) {
        i2c_byte = (uint8_t)((i2c_byte << 1) | ((next & TS21_SDA) ? 1U : 0U));
      } else {
        assert(i2c_count < sizeof(i2c_bytes));
        i2c_bytes[i2c_count++] = i2c_byte;
        i2c_byte               = 0;
      }
    }
  } else if (address == TS21_ADC1 + 8 && (value & 0x400000U) && conversion_complete) {
    *reg(TS21_ADC1) |= 2;
    *reg(TS21_ADC1 + 0x4C) = adc_value;
#ifdef TS21_HEATER_TEST
    if (use_adc_fixture) {
      assert(!gate_high());
      assert(*reg(TS21_TIM3) == 0);
      const unsigned channel = *reg(TS21_ADC1 + 0x34);
      switch (channel) {
      case 1:
        *reg(TS21_ADC1 + 0x4C) = (*reg(TS21_GPIOA + 12) & 1U) ? adc_fixture.tip_probe : adc_fixture.tip;
        break;
      case 2:
        *reg(TS21_ADC1 + 0x4C) = adc_fixture.handle;
        break;
      case 4:
        *reg(TS21_ADC1 + 0x4C) = adc_fixture.auxiliary;
        break;
      case 9:
        *reg(TS21_ADC1 + 0x4C) = adc_fixture.input;
        break;
      default:
        assert(false);
      }
    }
#endif
  } else if (address == TS21_SYSTICK + 8 || address == TS21_SYSTICK + 12) {
    ticks = ((uint64_t)*reg(TS21_SYSTICK + 12) << 32) | *reg(TS21_SYSTICK + 8);
  }
}

static void reset(void) {
  memset(registers, 0, sizeof(registers));
  memset(writes, 0, sizeof(writes));
  ts21_status    = (struct TS21Status){0};
  register_count = write_count = 0;
  ticks                        = 0;
  clock_ready = clock_running = calibration_complete = conversion_complete = true;
  hsi_selected = pll_ready = pll_selected = true;
  pll_stuck                               = false;
  input_a                                 = (1U << 5) | (1U << 7);
  scl_stuck = sda_stuck = i2c_active = false;
  acknowledge                        = true;
  i2c_pulses = i2c_count = 0;
  i2c_rise = i2c_fall = 0;
  uart_size = uart_position = 0;
  uart_error                = 0;
  adc_value                 = 0xFABC;
#ifdef TS21_HEATER_TEST
  heater_permitted = use_adc_fixture = false;
  heater_failures                    = 0;
  gate_started_ticks                 = 0;
#endif
  *reg(TS21_GPIOA + 12)  = TS21_HEATER_EN | TS21_HEATER_IN;
  *reg(TS21_TIM3)        = 1;
  *reg(TS21_TIM3 + 0x20) = 1;
  *reg(TS21_TIM3 + 0x34) = 959;
}

static void require_inhibited(void) {
  assert(!(*reg(TS21_GPIOA + 12) & (TS21_HEATER_EN | TS21_HEATER_IN)));
  assert(*reg(TS21_TIM3) == 0);
  assert(*reg(TS21_TIM3 + 0x20) == 0);
  assert(*reg(TS21_TIM3 + 0x34) == 0);
  for (unsigned i = 0; i < write_count; ++i) {
    const uint32_t address = writes[i].address;
    const uint32_t value   = writes[i].value;
    if (address == TS21_GPIOA + 0x10) {
      assert(!(value & (TS21_HEATER_EN | TS21_HEATER_IN)));
    }
    if (address == TS21_TIM3 || address == TS21_TIM3 + 0x20 || address == TS21_TIM3 + 0x34) {
      assert(value == 0);
    }
    if (address == TS21_USART2 + 12) {
      assert(!(value & 8U));
    }
    assert(address != TS21_USART2 + 4);
    assert(address < 0x08000000 || address >= 0x08010000);
    assert(address < 0x40022000 || address >= 0x40022400);
  }
}

static void append_frame(uint8_t voltage, uint8_t status) {
  assert(uart_size + 4 <= sizeof(uart_bytes));
  uint8_t *frame = uart_bytes + uart_size;
  frame[0]       = 0xFF;
  frame[1]       = voltage;
  frame[2]       = status;
  frame[3]       = ts21_crc8(frame, 3);
  uart_size += 4;
}

static void test_protocol(void) {
  const uint8_t check[] = "123456789";
  assert(ts21_crc8(check, 9) == 0xA1);
  for (unsigned voltage = 0; voltage < 256; ++voltage) {
    for (unsigned status = 0; status < 256; ++status) {
      struct TS21PDParser parser   = {{0}, 0};
      struct TS21PDFrame  frame    = {0, 0};
      uint8_t             packet[] = {0xFF, (uint8_t)voltage, (uint8_t)status, 0};
      packet[3]                    = ts21_crc8(packet, 3);
      assert(!ts21_pd_feed(&parser, 0, &frame));
      for (unsigned i = 0; i < 3; ++i) {
        assert(!ts21_pd_feed(&parser, packet[i], &frame));
      }
      assert(ts21_pd_feed(&parser, packet[3], &frame));
      assert(frame.voltage == voltage && frame.status == status && parser.used == 0);
    }
  }
  struct TS21PDParser parser   = {{0}, 0};
  struct TS21PDFrame  frame    = {0, 0};
  uint8_t             packet[] = {0xFF, 20, 1, 0};
  packet[3]                    = ts21_crc8(packet, 3);
  const uint8_t corrupt[]      = {0xFF, 0x12, 0x34};
  for (unsigned i = 0; i < sizeof(corrupt); ++i) {
    assert(!ts21_pd_feed(&parser, corrupt[i], &frame));
  }
  for (unsigned i = 0; i < 3; ++i) {
    assert(!ts21_pd_feed(&parser, packet[i], &frame));
  }
  assert(ts21_pd_feed(&parser, packet[3], &frame));
  assert(frame.voltage == 20 && frame.status == 1);
}

static void test_startup_and_inputs(void) {
  reset();
  assert(ts21_board_init());
  assert(ts21_status.errors == 0);
  for (unsigned bank = 0; bank < 8; ++bank) {
    assert(*reg(0xE000E180 + bank * 4) == UINT32_MAX);
    assert(*reg(0xE000E280 + bank * 4) == UINT32_MAX);
  }
  require_inhibited();
  unsigned clear_index = 0;
  unsigned mode_index  = 0;
  for (unsigned i = 0; i < write_count; ++i) {
    if (!clear_index && writes[i].address == TS21_GPIOA + 0x14 && writes[i].value == TS21_HEATER_EN) {
      clear_index = i;
    }
    if (!mode_index && writes[i].address == TS21_GPIOA + 4) {
      mode_index = i;
    }
  }
  assert(clear_index && mode_index && clear_index < mode_index);
#ifdef TS21_IRONOS
  const uint8_t top = 2; // Top button: IronOS B / settings.
#else
  const uint8_t top = 1; // Preserve the standalone diagnostic's raw GPIO labels.
#endif
  assert(ts21_buttons() == 0);
  input_a &= ~(1U << 5);
  assert(ts21_buttons() == top);
  input_a &= ~(1U << 7);
  assert(ts21_buttons() == 3);
  input_a |= 1U << 5;
  assert(ts21_buttons() == (top ^ 3));
  const uint8_t channels[] = {1, 2, 4, 9};
  for (unsigned i = 0; i < sizeof(channels); ++i) {
    uint16_t sample = 0;
    assert(ts21_adc_read(channels[i], &sample));
    assert(sample == 0xABC);
    assert(*reg(TS21_ADC1 + 0x34) == channels[i]);
  }
  uint16_t unchanged = 0xBEEF;
  assert(!ts21_adc_read(3, &unchanged));
  assert(unchanged == 0xBEEF);
  assert(ts21_status.errors & TS21_ERROR_ARGUMENT);
  require_inhibited();
}

static void test_clocks(void) {
  reset();
  *reg(TS21_RCC)     = (1U << 24) | 1;
  *reg(TS21_RCC + 4) = 0x001D8C0A;
  assert(ts21_board_init());
#ifdef TS21_IRONOS
  assert(TS21_CLOCK_HZ == 64000000);
  assert((ts21_mmio_read(TS21_RCC + 4) & 0x3FFFFF) == 0x18C40A);
  assert(*reg(0x40023800) & 0x10);
  assert(ts21_mmio_read(TS21_RCC) & (1U << 25));
  assert(TS21_TIMER_HZ == 8000000);
  assert(TS21_APB1_HZ == 32000000);
  assert(*reg(TS21_USART2 + 8) == 3333);
#else
  assert(TS21_CLOCK_HZ == 8000000);
  assert((ts21_mmio_read(TS21_RCC + 4) & 0xFFFF) == 0);
  assert(!(ts21_mmio_read(TS21_RCC) & (1U << 25)));
  assert(*reg(TS21_USART2 + 8) == 833);
#endif
  assert(*reg(TS21_SYSTICK) == 1);
  require_inhibited();

  reset();
  *reg(TS21_RCC + 4) = 8;
  hsi_selected       = false;
  assert(!ts21_board_init());
  assert(ts21_status.errors & TS21_ERROR_CLOCK);
  require_inhibited();

  reset();
  pll_stuck = true;
  assert(!ts21_board_init());
  assert(ts21_status.errors & TS21_ERROR_CLOCK);
  require_inhibited();

#ifdef TS21_IRONOS
  reset();
  pll_ready = false;
  assert(!ts21_board_init());
  assert(ts21_status.errors & TS21_ERROR_CLOCK);
  require_inhibited();

  reset();
  pll_selected = false;
  assert(!ts21_board_init());
  assert(ts21_status.errors & TS21_ERROR_CLOCK);
  require_inhibited();
#endif
}

static void test_faults(void) {
  reset();
  clock_ready = false;
  assert(!ts21_board_init());
  assert(ts21_status.errors & TS21_ERROR_CLOCK);
  require_inhibited();

  reset();
  calibration_complete = false;
  assert(ts21_board_init());
  uint16_t value = 0xBEEF;
  assert(!ts21_adc_read(1, &value));
  assert(value == 0xBEEF && (ts21_status.errors & TS21_ERROR_ADC));
  require_inhibited();

  reset();
  assert(ts21_board_init());
  conversion_complete = false;
  assert(!ts21_adc_read(1, &value));
  assert(value == 0xBEEF && (ts21_status.errors & TS21_ERROR_ADC));
  clock_running = false;
  assert(!ts21_delay_us(10));
  assert(ts21_status.errors & TS21_ERROR_CLOCK);
  require_inhibited();
}

static void test_i2c_and_passive_uart(void) {
  reset();
  assert(ts21_board_init());
  append_frame(20, 7);
  const uint8_t commands[] = {0xAE, 0xAF};
  assert(ts21_oled_write(0, commands, sizeof(commands)));
  const uint8_t expected[] = {0x78, 0x00, 0xAE, 0xAF};
  assert(i2c_count == sizeof(expected));
  assert(memcmp(expected, i2c_bytes, sizeof(expected)) == 0);
  assert(!i2c_active);
  assert(ts21_status.pd_frames == 1);
  assert(ts21_status.pd_voltage == 20 && ts21_status.pd_status == 7);
  require_inhibited();

  acknowledge = false;
  assert(!ts21_oled_write(0, commands, sizeof(commands)));
  assert(ts21_status.errors & TS21_ERROR_I2C_NACK);
  assert(!i2c_active);
  require_inhibited();

  scl_stuck = true;
  assert(!ts21_oled_write(0, commands, sizeof(commands)));
  assert(ts21_status.errors & TS21_ERROR_I2C_BUS);
  require_inhibited();

  reset();
  assert(ts21_board_init());
  sda_stuck = true;
  assert(!ts21_oled_write(0, commands, sizeof(commands)));
  assert(ts21_status.errors & TS21_ERROR_I2C_BUS);
  require_inhibited();

  reset();
  assert(ts21_board_init());
  uart_error = 8;
  ts21_uart_poll();
  assert(ts21_status.errors & TS21_ERROR_UART);
  assert(ts21_status.pd_frames == 0);
  assert(!ts21_oled_write(1, commands, sizeof(commands)));
  assert(ts21_status.errors & TS21_ERROR_ARGUMENT);
  require_inhibited();
}

static void test_full_oled_page_and_bounds(void) {
  reset();
  assert(ts21_board_init());
  uint8_t page[128];
  for (unsigned i = 0; i < sizeof(page); ++i) {
    page[i] = (uint8_t)i;
  }
  const uint64_t started = ticks;
  assert(ts21_oled_write(0x40, page, sizeof(page)));
#ifdef TS21_IRONOS
  assert((ticks - started) / TS21_TIMER_TICKS_PER_US < 9000);
#else
  assert((ticks - started) / TS21_TIMER_TICKS_PER_US < 13000);
#endif
  assert(i2c_count == sizeof(page) + 2);
  assert(i2c_bytes[0] == 0x78 && i2c_bytes[1] == 0x40);
  assert(memcmp(page, i2c_bytes + 2, sizeof(page)) == 0);
  assert(!i2c_active);

  const unsigned before = write_count;
  assert(!ts21_oled_write(0x40, NULL, sizeof(page)));
  assert(!ts21_oled_write(0x40, page, 0));
  assert(!ts21_oled_write(0x40, page, sizeof(page) + 1));
  assert(write_count == before);
  assert(ts21_status.errors & TS21_ERROR_ARGUMENT);
  require_inhibited();
}

static void test_timebase_wrap_and_bounds(void) {
  reset();
  assert(ts21_board_init());
  ticks = UINT32_MAX - 3;
  assert(ts21_delay_us(10));
  assert(ticks > UINT32_MAX && ticks < (uint64_t)UINT32_MAX + 16 * TS21_TIMER_TICKS_PER_US);
  assert(ts21_status.errors == 0);
  const uint64_t before = ticks;
  assert(!ts21_delay_us(100001));
  assert(ticks == before);
  assert(ts21_status.errors & TS21_ERROR_ARGUMENT);
  clock_running = false;
  ticks         = (uint64_t)UINT32_MAX * TS21_TIMER_TICKS_PER_US;
  assert(ts21_micros() == UINT32_MAX);
  ticks += TS21_TIMER_TICKS_PER_US;
  assert(ts21_micros() == 0);
  clock_running = true;
  ticks         = UINT32_MAX - TS21_TIMER_TICKS_PER_US + 1;
  assert(ts21_timer_now() > UINT32_MAX);
  require_inhibited();
}

static void test_sensor_sequence(void) {
  reset();
  assert(ts21_board_init());
  struct TS21Sample sample = {0};
  assert(ts21_sample_sensors(&sample));
  assert(sample.tip_probe == 0xABC && sample.tip == 0xABC && sample.handle == 0xABC && sample.input == 0xABC && sample.auxiliary == 0xABC);
  assert(!(*reg(TS21_GPIOA + 12) & 1));
  assert(!(*reg(TS21_GPIOB + 12) & (1U << 15)));
  assert(ticks >= 1600 * TS21_TIMER_TICKS_PER_US);
  require_inhibited();

  conversion_complete = false;
  assert(!ts21_sample_sensors(&sample));
  assert(!(*reg(TS21_GPIOA + 12) & 1));
  assert(!(*reg(TS21_GPIOB + 12) & (1U << 15)));
  require_inhibited();
  assert(!ts21_sample_sensors(NULL));
}

static void test_all_pwm_requests_inhibited(void) {
  for (unsigned fast = 0; fast < 2; ++fast) {
    for (unsigned pulse = 0; pulse < 256; ++pulse) {
      reset();
      setTipPWM((uint8_t)pulse, fast != 0);
      require_inhibited();
    }
  }
}

static void test_sensor_conversion(void) {
  int16_t value = 1234, previous = INT16_MIN;
  assert(!ts21_tip_delta_c(0, &value) && value == 1234);
  assert(!ts21_tip_delta_c(4095, &value) && value == 1234);
  assert(!ts21_tip_delta_c(1000, NULL));
  for (uint16_t raw = 1; raw < 4095; ++raw) {
    assert(ts21_tip_delta_c(raw, &value));
    assert(value >= previous);
    const double x        = raw - 563.0;
    const double expected = -0.00001 * x * x + 0.2006 * x - 3.1236;
    assert(value - expected > -1.001 && value - expected < 1.001);
    previous = value;
  }
  assert(ts21_handle_c_x10(2864, &value) && value == 250);
  assert(ts21_handle_c_x10(3654, &value) && value == 0);
  assert(ts21_handle_c_x10(4015, &value) && value == -300);
  assert(ts21_handle_c_x10(230, &value) && value == 1250);
  assert(!ts21_handle_c_x10(229, &value));
  assert(!ts21_handle_c_x10(4016, &value));
  assert(!ts21_handle_c_x10(2800, NULL));
  previous = INT16_MAX;
  for (uint16_t raw = 230; raw <= 4015; ++raw) {
    assert(ts21_handle_c_x10(raw, &value) && value <= previous);
    previous = value;
  }
  assert(ts21_input_voltage_x10(0) == 0);
  assert(ts21_input_voltage_x10(1) == 0);
  assert(ts21_input_voltage_x10(1451) == 100);
  struct TS21Sample sample = {.tip_probe = 934, .tip = 580};
  assert(ts21_tip_resistance_x10(&sample) == 20);
  for (unsigned delta = 0; delta <= 3500; ++delta) {
    sample.tip_probe = (uint16_t)(580 + delta);
    assert(ts21_tip_resistance_x10(&sample) == delta * 10 / 177);
  }
  sample.tip_probe = 4095;
  assert(ts21_tip_resistance_x10(&sample) == 0);
  sample.tip_probe = 579;
  assert(ts21_tip_resistance_x10(&sample) == 0);
  sample.tip = 0;
  assert(ts21_tip_resistance_x10(&sample) == 0);
  assert(ts21_tip_resistance_x10(NULL) == 0);
}

#ifdef TS21_HEATER_TEST
_Static_assert(TS21_HEATER_POWER_X10 == 1000U, "The requested ceiling is 100 W");
_Static_assert(TS21_HEATER_MAX_INPUT_X10 == 240U, "Accept the hardware's full 24 V DC rating");
_Static_assert((TS21_HEATER_LATCHING_ERRORS & (TS21_ERROR_HEATER_OPEN | TS21_ERROR_HEATER_SUPPLY | TS21_ERROR_HEATER_TEMP)) == 0, "Self-correcting readings must not stop the iron");
_Static_assert(TS21_DEFAULT_TEMP_C == 320 && TS21_HEATER_CUTOFF_C == 500, "Use normal setpoints with a separate overtemperature cutoff");

static void prepare_heater(void) {
  reset();
  assert(ts21_board_init());
  assert(*reg(TS21_IWDG + 4) == 3 && *reg(TS21_IWDG + 8) == 624);
  use_adc_fixture  = true;
  adc_fixture      = (struct TS21Sample){.tip_probe = 934, .tip = 580, .handle = 2864, .input = 725, .auxiliary = 100};
  heater_permitted = true;
  write_count      = 0;
}

static void sample_heater(void) {
  struct TS21Sample sample;
  assert(ts21_sample_sensors(&sample));
}

static void assert_heater_off(void) {
  assert(!gate_high());
  assert(*reg(TS21_TIM3) == 0);
  assert(*reg(TS21_TIM3 + 0x20) == 0);
  assert(getTipX10Watts() == 0);
}

static void test_worklight_during_heating(void) {
  prepare_heater();
  setWorklight(true);
  sample_heater();
  setTipPWM(255, false);
  ticks += 1000U * TS21_TIMER_TICKS_PER_US;
  assert(gate_high());
  const uint32_t timer1        = *reg(TS21_TIM1);
  const uint32_t timer3        = *reg(TS21_TIM3);
  const uint32_t compare       = *reg(TS21_TIM3 + 0x34);
  const uint32_t configuration = *reg(TS21_GPIOA + 4);
  setWorklight(false);
  assert(!(*reg(TS21_GPIOA + 12) & TS21_WORKLIGHT));
  assert(gate_high() && getTipX10Watts() > 0);
  setWorklight(true);
  assert(*reg(TS21_GPIOA + 12) & TS21_WORKLIGHT);
  assert(*reg(TS21_TIM1) == timer1 && *reg(TS21_TIM3) == timer3);
  assert(*reg(TS21_TIM3 + 0x34) == compare);
  assert(*reg(TS21_GPIOA + 4) == configuration);
  sample_heater();
  assert(*reg(TS21_GPIOA + 12) & TS21_WORKLIGHT);
  assert((*reg(TS21_GPIOA + 4) >> 4 & 15) == 3);
  ts21_heater_inhibit();
  assert_heater_off();
  assert(*reg(TS21_GPIOA + 12) & TS21_WORKLIGHT);
}

static void test_bounded_heater(void) {
  for (unsigned fast = 0; fast < 2; ++fast) {
    for (unsigned pulse = 0; pulse < 256; ++pulse) {
      prepare_heater();
      sample_heater();
      setTipPWM((uint8_t)pulse, fast != 0);
      assert(ts21_status.errors == 0 && heater_failures == 0);
      const uint32_t compare = *reg(TS21_TIM3 + 0x34);
      assert(compare <= pulse * TS21_HEATER_PERIOD / 255U);
      assert(compare * 50U * 50U <= TS21_HEATER_POWER_X10 * 20U * TS21_HEATER_PERIOD);
      if (compare) {
        assert(getTipX10Watts() == compare * 125U / TS21_HEATER_PERIOD);
        assert(*reg(TS21_TIM3 + 0x28) == 0);
        assert(*reg(TS21_TIM3 + 0x2C) == 639);
        assert(*reg(TS21_TIM3 + 0x18) == 0x68);
        assert(*reg(TS21_TIM1 + 0x28) == 63999);
        assert(*reg(TS21_TIM1 + 0x2C) == TS21_HEATER_WINDOW_MS);
        assert(*reg(TS21_TIM1 + 0x3C) == 1);
        assert(*reg(TS21_TIM1 + 0x1C) == 0x78);
        assert(*reg(TS21_TIM1 + 0x20) == 0x100);
        assert(*reg(TS21_TIM1 + 0x44) == 0x8000);
        assert(*reg(TS21_TIM1) == 0x89);
        assert(!gate_high());
        ticks += 1000U * TS21_TIMER_TICKS_PER_US;
        assert(gate_high());
        ticks += TS21_HEATER_WINDOW_MS * 1000U * TS21_TIMER_TICKS_PER_US;
        // No firmware call is needed for the one-pulse gate to return low.
        assert(!gate_high());
        assert(!(*reg(TS21_TIM1) & 1U));
        assert(getTipX10Watts() == 0);
      }
      setTipPWM(0, false);
      assert_heater_off();
    }
  }

  const uint16_t voltages[]    = {654, 725, 1451, 2900, 3045};
  const unsigned resistances[] = {10, 20, 40, 100};
  for (unsigned v = 0; v < sizeof(voltages) / sizeof(voltages[0]); ++v) {
    for (unsigned r = 0; r < sizeof(resistances) / sizeof(resistances[0]); ++r) {
      prepare_heater();
      adc_fixture.input     = voltages[v];
      adc_fixture.tip_probe = adc_fixture.tip + (uint16_t)((resistances[r] * 177U + 9U) / 10U);
      sample_heater();
      setTipPWM(255, false);
      assert(ts21_status.errors == 0);
      const uint32_t voltage = ts21_input_voltage_x10(adc_fixture.input) + 1U;
      const uint32_t compare = *reg(TS21_TIM3 + 0x34);
      assert(compare > 0 && compare <= TS21_HEATER_PERIOD * 85 / 100);
      assert(compare * voltage * voltage <= TS21_HEATER_POWER_X10 * ts21_tip_resistance_x10(&adc_fixture) * TS21_HEATER_PERIOD);
      assert(getTipX10Watts() > 0 && getTipX10Watts() <= TS21_HEATER_POWER_X10);
    }
  }

  prepare_heater();
  sample_heater();
  setTipPWM(255, false);
  assert(*reg(TS21_TIM3 + 0x34) == 544);
  assert(getTipX10Watts() == 106); // A 5 V source cannot supply 100 W into this fixture.

  prepare_heater();
  adc_fixture.input = 2900;
  sample_heater();
  setTipPWM(255, false);
  assert(*reg(TS21_TIM3 + 0x34) == 316);
  assert(getTipX10Watts() == 997); // 20 V: retain tenths-watt values above the old 8-bit range.
  ticks += 1000U * TS21_TIMER_TICKS_PER_US;
  assert(gate_high());
  ticks += TS21_HEATER_WINDOW_MS * 1000U * TS21_TIMER_TICKS_PER_US;
  assert(!gate_high() && !(*reg(TS21_TIM1) & 1U));
  assert(getTipX10Watts() == 0);
}

static void test_heater_faults(void) {
  prepare_heater();
  sample_heater();
  setTipPWM(255, false);
  ts21_heater_trip(TS21_ERROR_HEATER_MONITOR);
  assert(ts21_status.errors == TS21_ERROR_HEATER_MONITOR);
  assert(heater_failures == 1 && !heater_permitted);
  assert_heater_off();
  heater_permitted = true;
  sample_heater();
  setTipPWM(255, false);
  assert(ts21_status.errors == TS21_ERROR_HEATER_MONITOR);
  assert_heater_off();

  prepare_heater();
  adc_fixture.tip       = 961; // 100 C must no longer act as a test cutoff.
  adc_fixture.tip_probe = 1315;
  sample_heater();
  setTipPWM(255, false);
  assert(ts21_status.errors == 0);
  assert(getTipX10Watts() == 106);
  adc_fixture.tip       = 2180; // 320 C.
  adc_fixture.tip_probe = 2534;
  sample_heater();
  setTipPWM(255, false);
  assert(ts21_status.errors == 0 && getTipX10Watts() == 106);
  adc_fixture.tip       = 3321; // 499 C.
  adc_fixture.tip_probe = 3675;
  sample_heater();
  setTipPWM(255, false);
  assert(ts21_status.errors == 0 && getTipX10Watts() == 106);
  adc_fixture.tip       = 3328; // Exactly 500 C: power is withheld without stopping the iron.
  adc_fixture.tip_probe = 3682;
  sample_heater();
  setTipPWM(255, false);
  assert(ts21_status.errors == 0 && heater_failures == 0);
  assert_heater_off();
  adc_fixture.tip       = 3321; // Back under the cutoff after the reading settles.
  adc_fixture.tip_probe = 3675;
  sample_heater();
  setTipPWM(255, false);
  assert(ts21_status.errors == 0 && getTipX10Watts() == 106);

  prepare_heater();
  sample_heater();
  setTipPWM(255, false);
  assert(!ts21_sample_sensors(NULL));
  assert(ts21_status.errors & TS21_ERROR_ARGUMENT);
  assert_heater_off();

  prepare_heater();
  sample_heater();
  heater_permitted = false;
  setTipPWM(255, false);
  assert_heater_off();
  require_inhibited();

  prepare_heater();
  sample_heater();
  ticks += 25000U * TS21_TIMER_TICKS_PER_US;
  setTipPWM(255, false);
  assert(ts21_status.errors == 0 && getTipX10Watts() > 0);

  prepare_heater();
  sample_heater();
  setTipPWM(255, false);
  setTipPWM(255, false); // A command cannot reuse the preceding sample, but that is not a fault.
  assert(ts21_status.errors == 0 && heater_failures == 0);
  assert_heater_off();

  prepare_heater();
  sample_heater();
  setTipPWM(255, false);
  ticks += TS21_HEATER_COMMAND_US * TS21_TIMER_TICKS_PER_US;
  ts21_heater_check();
  assert(ts21_status.errors == 0 && heater_failures == 0);
  assert_heater_off();
  ticks += 20000000ULL * TS21_TIMER_TICKS_PER_US;
  sample_heater();
  setTipPWM(255, false);
  assert(ts21_status.errors == 0 && getTipX10Watts() > 0);

  prepare_heater();
  sample_heater();
  setTipPWM(255, false);
  ts21_status.errors |= TS21_ERROR_FLASH;
  ts21_heater_check();
  assert_heater_off();
  sample_heater();
  setTipPWM(255, false);
  assert_heater_off();

  prepare_heater();
  sample_heater();
  setTipPWM(255, false);
  heater_permitted = false;
  ts21_heater_check();
  assert_heater_off();

  // A genuine short or an unreadable sensor stops the iron, but only after consecutive bad samples.
  const struct TS21Sample latching[] = {
      {.tip_probe = 600, .tip = 580, .handle = 2864, .input = 725},
      {.tip_probe = 934, .tip = 580,    .handle = 0, .input = 725},
  };
  const uint32_t latching_errors[] = {TS21_ERROR_HEATER_SHORT, TS21_ERROR_SENSOR};
  for (unsigned i = 0; i < sizeof(latching) / sizeof(latching[0]); ++i) {
    prepare_heater();
    sample_heater();
    setTipPWM(255, false);
    adc_fixture = latching[i];
    for (unsigned repeat = 1; repeat < TS21_HEATER_FAULT_SAMPLES; ++repeat) {
      sample_heater();
      assert(ts21_status.errors == 0 && heater_failures == 0);
      assert_heater_off();
    }
    sample_heater();
    assert(ts21_status.errors & latching_errors[i]);
    assert(heater_failures != 0);
    assert_heater_off();
    adc_fixture = (struct TS21Sample){.tip_probe = 934, .tip = 580, .handle = 2864, .input = 725};
    sample_heater();
    heater_permitted = true;
    setTipPWM(255, false);
    assert_heater_off();
  }

  // An open tip, a supply excursion and an inflated post-pulse reading withhold power without stopping.
  const struct TS21Sample transient[] = {
      {.tip_probe = 4095,  .tip = 580, .handle = 2864,  .input = 725},
      { .tip_probe = 934,  .tip = 580, .handle = 2864,  .input = 500},
      { .tip_probe = 934,  .tip = 580, .handle = 2864, .input = 3500},
      {.tip_probe = 3682, .tip = 3328, .handle = 2864,  .input = 725},
  };
  for (unsigned i = 0; i < sizeof(transient) / sizeof(transient[0]); ++i) {
    prepare_heater();
    sample_heater();
    setTipPWM(255, false);
    adc_fixture = transient[i];
    for (unsigned repeat = 0; repeat < TS21_HEATER_FAULT_SAMPLES * 4; ++repeat) {
      ticks += 125000ULL * TS21_TIMER_TICKS_PER_US;
      sample_heater();
      setTipPWM(255, false);
      assert(ts21_status.errors == 0 && heater_failures == 0);
      assert_heater_off();
    }
    adc_fixture = (struct TS21Sample){.tip_probe = 934, .tip = 580, .handle = 2864, .input = 725};
    sample_heater();
    setTipPWM(255, false);
    assert(ts21_status.errors == 0 && getTipX10Watts() > 0); // Recovers on its own.
  }

  // A long run of benign transient readings must not pre-charge the debounce for a different fault:
  // an unplugged tip reads open indefinitely, and one sensor glitch after that is still not a stop.
  prepare_heater();
  sample_heater();
  setTipPWM(255, false);
  adc_fixture = (struct TS21Sample){.tip_probe = 4095, .tip = 580, .handle = 2864, .input = 725};
  for (unsigned repeat = 0; repeat < TS21_HEATER_FAULT_SAMPLES * 4; ++repeat) {
    sample_heater();
  }
  assert(ts21_status.errors == 0 && heater_failures == 0);
  adc_fixture.handle = 0; // A single invalid sensor conversion.
  sample_heater();
  assert(ts21_status.errors == 0 && heater_failures == 0);
  assert_heater_off();

  // The handle warms up in normal use; that is not an overtemperature tip.
  prepare_heater();
  adc_fixture.handle = 500;
  sample_heater();
  setTipPWM(255, false);
  assert(ts21_status.errors == 0 && getTipX10Watts() > 0);

  prepare_heater();
  sample_heater();
  setTipPWM(255, false);
  conversion_complete = false;
  struct TS21Sample sample;
  assert(!ts21_sample_sensors(&sample));
  assert(ts21_status.errors & TS21_ERROR_ADC);
  assert_heater_off();

  // A tip that refuses to climb is a weak supply or a big joint, not a reason to stop.
  prepare_heater();
  sample_heater();
  setTipPWM(255, false);
  for (unsigned i = 0; i < 2400; ++i) { // Five minutes of full effort at a fixed temperature.
    ticks += 125000ULL * TS21_TIMER_TICKS_PER_US;
    write_count = 0;
    sample_heater();
    setTipPWM(255, false);
    if (ts21_status.errors) {
      fprintf(stderr, "Full-effort heating stopped at sample %u with errors=0x%08lx\n", i, (unsigned long)ts21_status.errors);
    }
    assert(ts21_status.errors == 0 && heater_failures == 0);
    assert(getTipX10Watts() > 0);
  }
}

static void test_regulated_hold_below_target(void) {
  prepare_heater();
  adc_fixture.input = 2900;
  sample_heater();
  setTipPWM(255, false);
  adc_fixture.tip       = 2150; // 315 C with a 320 C target.
  adc_fixture.tip_probe = 2504;
  for (unsigned i = 0; i < 480; ++i) {
    ticks += 125000ULL * TS21_TIMER_TICKS_PER_US;
    write_count = 0;
    sample_heater();
    setTipPWM(8, false); // Regulated holding power, well below the 100 W ceiling.
    if (ts21_status.errors) {
      fprintf(stderr, "Stable 315 C hold stopped at sample %u with errors=0x%08lx\n", i, (unsigned long)ts21_status.errors);
    }
    assert(ts21_status.errors == 0);
  }

  // Neither a long idle gap nor a resumed full-effort demand may invent a fault.
  sample_heater();
  setTipPWM(255, false);
  ticks += 20000000ULL * TS21_TIMER_TICKS_PER_US;
  sample_heater();
  assert(ts21_status.errors == 0);
  setTipPWM(255, false);
  assert(ts21_status.errors == 0 && getTipX10Watts() > 0);

  prepare_heater();
  sample_heater();
  setTipPWM(255, false);
  setTipPWM(0, false);
  ticks += 20000000ULL * TS21_TIMER_TICKS_PER_US;
  sample_heater();
  assert(ts21_status.errors == 0);
  setTipPWM(255, false);
  assert(ts21_status.errors == 0 && getTipX10Watts() > 0);
}

static void test_manual_heater_recovery(void) {
  const uint32_t recoverable[] = {TS21_ERROR_ADC,         TS21_ERROR_SENSOR,         TS21_ERROR_HEATER_SHORT,  TS21_ERROR_HEATER_STALE, TS21_ERROR_HEATER_TIMEOUT,
                                  TS21_ERROR_HEATER_TEMP, TS21_ERROR_HEATER_NO_RISE, TS21_ERROR_HEATER_SUPPLY, TS21_ERROR_HEATER_OPEN,  TS21_ERROR_HEATER_MONITOR};
  for (unsigned i = 0; i < sizeof(recoverable) / sizeof(recoverable[0]); ++i) {
    prepare_heater();
    setWorklight(true);
    sample_heater();
    setTipPWM(255, false);
    ts21_heater_trip(recoverable[i]);
    assert(!heater_permitted);
    assert(!ts21_heater_reset_faults()); // No reset using a missing/consumed sample.
    sample_heater();
    assert(ts21_heater_reset_faults());
    assert(ts21_status.errors == 0 && !heater_permitted);
    assert_heater_off();
    assert(*reg(TS21_GPIOA + 12) & TS21_WORKLIGHT);
    setTipPWM(255, false);
    assert_heater_off();
    heater_permitted = true;
    sample_heater();
    setTipPWM(255, false);
    assert(ts21_status.errors == 0 && getTipX10Watts() > 0);
    ticks += 20000000ULL * TS21_TIMER_TICKS_PER_US;
    sample_heater();
    setTipPWM(255, false);
    assert(ts21_status.errors == 0 && getTipX10Watts() > 0);
  }

  prepare_heater();
  sample_heater();
  setTipPWM(255, false);
  ts21_heater_trip(TS21_ERROR_SENSOR);
  adc_fixture.tip_probe = adc_fixture.tip;
  sample_heater();
  assert(!ts21_heater_reset_faults());
  assert(ts21_status.errors & TS21_ERROR_HEATER_SHORT);
  assert(!heater_permitted);
  assert_heater_off();
  adc_fixture.tip_probe += 354;
  sample_heater();
  assert(ts21_heater_reset_faults());
  assert_heater_off();

  const uint32_t nonrecoverable[] = {TS21_ERROR_CLOCK, TS21_ERROR_FLASH, TS21_ERROR_TRAP, 1U << 31};
  for (unsigned i = 0; i < sizeof(nonrecoverable) / sizeof(nonrecoverable[0]); ++i) {
    prepare_heater();
    ts21_heater_trip(nonrecoverable[i] | TS21_ERROR_HEATER_NO_RISE | TS21_ERROR_UART);
    sample_heater();
    assert(!ts21_heater_reset_faults());
    assert(ts21_status.errors == (nonrecoverable[i] | TS21_ERROR_UART));
    assert_heater_off();
  }
}

static void test_continuous_temperature_control(void) {
  for (unsigned wrap = 0; wrap < 2; ++wrap) {
    prepare_heater();
    if (wrap) {
      ticks = ((uint64_t)UINT32_MAX - 1000000U) * TS21_TIMER_TICKS_PER_US;
    }
    adc_fixture.tip       = 909; // At the 90 C target, lack of further rise is not a fault.
    adc_fixture.tip_probe = 1263;
    for (unsigned i = 0; i < 480; ++i) {
      ticks += 125000ULL * TS21_TIMER_TICKS_PER_US;
      write_count = 0;
      sample_heater();
      setTipPWM(255, false);
      ts21_heater_check();
      assert(ts21_status.errors == 0);
      assert(getTipX10Watts() == 106);
    }

    if (wrap) {
      ticks = ((uint64_t)UINT32_MAX - 1000000U) * TS21_TIMER_TICKS_PER_US;
    }
    adc_fixture.tip       = 580;
    adc_fixture.tip_probe = 934;
    sample_heater();
    setTipPWM(255, false);
    ticks += 19000000ULL * TS21_TIMER_TICKS_PER_US;
    adc_fixture.tip       = 610;
    adc_fixture.tip_probe = 964;
    sample_heater();
    setTipPWM(255, false);
    assert(ts21_status.errors == 0);
    ticks += 19000000ULL * TS21_TIMER_TICKS_PER_US;
    sample_heater();
    setTipPWM(255, false);
    assert(ts21_status.errors == 0);
    ticks += 1000001ULL * TS21_TIMER_TICKS_PER_US;
    sample_heater();
    setTipPWM(255, false);
    assert(ts21_status.errors == 0 && getTipX10Watts() > 0);
  }
}

static void test_passive_uart_heater(void) {
  for (uint32_t fault = 1; fault <= 8; fault <<= 1) {
    prepare_heater();
    sample_heater();
    uart_error = fault;
    ts21_uart_poll();
    assert(ts21_status.errors == TS21_ERROR_UART);
    append_frame(20, 1);
    ts21_uart_poll();
    assert(ts21_status.pd_frames == 1);
    setTipPWM(255, false);
    assert(getTipX10Watts() > 0 && heater_failures == 0);
    ts21_heater_check();
    assert(getTipX10Watts() > 0);
    assert(ts21_status.errors == TS21_ERROR_UART);
  }

  for (unsigned bit = 0; bit < 32; ++bit) {
    const uint32_t error = UINT32_C(1) << bit;
    if (error == TS21_ERROR_UART) {
      continue;
    }
    prepare_heater();
    sample_heater();
    ts21_status.errors = TS21_ERROR_UART;
    setTipPWM(255, false);
    assert(getTipX10Watts() > 0);
    ts21_status.errors |= error;
    ts21_heater_check();
    assert_heater_off();
    sample_heater();
    setTipPWM(255, false);
    assert_heater_off();
    assert(ts21_status.errors & error);
    assert(ts21_status.errors & TS21_ERROR_UART);
  }
}
#endif

#ifdef TS21_IRONOS
static void test_worklight_gpio(void) {
  _Static_assert(TS21_WORKLIGHT == (1U << 9), "Stock work light is PA9");
  _Static_assert(!(TS21_WORKLIGHT & (TS21_HEATER_IN | TS21_HEATER_EN)), "Work light must be separate from the heater");
  reset();
  assert(ts21_board_init());
  assert(!(*reg(TS21_GPIOA + 12) & TS21_WORKLIGHT));
  assert((*reg(TS21_GPIOA + 4) >> 4 & 15) == 3);
  const uint32_t output        = *reg(TS21_GPIOA + 12);
  const uint32_t port_b        = *reg(TS21_GPIOB + 12);
  const uint32_t configuration = *reg(TS21_GPIOA + 4);
  write_count                  = 0;
  setWorklight(true);
  assert(write_count == 1 && writes[0].address == TS21_GPIOA + 0x10 && writes[0].value == TS21_WORKLIGHT);
  assert(*reg(TS21_GPIOA + 12) == (output | TS21_WORKLIGHT));
  setWorklight(false);
  assert(write_count == 2 && writes[1].address == TS21_GPIOA + 0x14 && writes[1].value == TS21_WORKLIGHT);
  assert(*reg(TS21_GPIOA + 12) == output);
  assert(*reg(TS21_GPIOB + 12) == port_b);
  assert(*reg(TS21_GPIOA + 4) == configuration);
}
#endif

int main(void) {
  test_protocol();
  test_startup_and_inputs();
  test_clocks();
  test_faults();
  test_i2c_and_passive_uart();
  test_full_oled_page_and_bounds();
  test_timebase_wrap_and_bounds();
  test_sensor_sequence();
  test_sensor_conversion();
  test_all_pwm_requests_inhibited();
#ifdef TS21_IRONOS
  test_worklight_gpio();
#endif
#ifdef TS21_HEATER_TEST
  test_worklight_during_heating();
  test_bounded_heater();
  test_heater_faults();
  test_regulated_hold_below_target();
  test_manual_heater_recovery();
  test_continuous_temperature_control();
  test_passive_uart_heater();
  puts("TS21 heater tests passed: 100 W ceiling, wide power readback, low-supply limits and fault protection");
#endif
  puts("TS21 host tests passed: protocol, inhibit ordering, button mapping, clocks, ADC, UART, I2C and bounded failures");
  return 0;
}
