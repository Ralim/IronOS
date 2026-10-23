#include "font.h"
#include "ts21.h"

volatile struct TS21Status ts21_status;
static uint8_t             pixels[512];

void ts21_fault(uint32_t error) {
  ts21_status.errors |= error;
  for (;;) {
    ts21_heater_inhibit();
    ts21_watchdog_refresh();
  }
}

static void text(unsigned row, const char *value) {
  unsigned x = 4;
  while (*value && x + 12 <= 128) {
    const unsigned character = (unsigned char)*value++;
    if (character < 32 || character > 90) {
      ts21_fault(TS21_ERROR_ARGUMENT);
    }
    const uint8_t *glyph = ts21_font[character - 32];
    for (unsigned column = 0; column < 12; ++column) {
      pixels[row * 256 + x + column]       = glyph[column];
      pixels[row * 256 + 128 + x + column] = glyph[column + 12];
    }
    x += 12;
  }
}

static void decimal4(char *destination, uint16_t value) {
  for (unsigned i = 4; i; --i) {
    destination[i - 1] = (char)('0' + value % 10);
    value /= 10;
  }
}

static bool refresh(void) {
  for (unsigned page = 0; page < 4; ++page) {
    const uint8_t address[] = {(uint8_t)(0xB0 + page), 0x00, 0x10};
    if (!ts21_oled_write(0, address, sizeof(address)) || !ts21_oled_write(0x40, &pixels[page * 128], 128)) {
      return false;
    }
    ts21_heater_inhibit();
    ts21_watchdog_refresh();
  }
  return true;
}

static void draw(void) {
  static const uint8_t channels[] = {1, 2, 4, 9};
  for (unsigned i = 0; i < sizeof(pixels); ++i) {
    pixels[i] = 0;
  }
  text(0, "NO HEATING");
  char           line[] = "0:0000";
  const unsigned page   = ts21_status.page;
  if (page == 0) {
    char keys[] = "5:0 7:0";
    keys[2]     = (ts21_status.buttons & 1U) ? '1' : '0';
    keys[6]     = (ts21_status.buttons & 2U) ? '1' : '0';
    text(1, keys);
  } else if (page <= 4) {
    line[0] = (char)('0' + channels[page - 1]);
    decimal4(line + 2, ts21_status.raw_adc[page - 1]);
    text(1, (ts21_status.adc_valid_mask & (1U << (page - 1))) ? line : "ADC ERROR");
  } else if (page == 5) {
    line[0] = 'U';
    decimal4(line + 2, ts21_status.pd_voltage);
    text(1, ts21_status.pd_frames ? line : "UART WAIT");
  } else if (page == 6) {
    line[0] = 'X';
    decimal4(line + 2, ts21_status.pd_status);
    text(1, ts21_status.pd_frames ? line : "UART WAIT");
  } else {
    line[0] = 'E';
    decimal4(line + 2, (uint16_t)ts21_status.errors);
    text(1, line);
  }
}

int main(void) {
  static const uint8_t channels[]     = {1, 2, 4, 9};
  static const uint8_t display_init[] = {
      0xAE, 0xD5, 0x80, 0xA8, 0x1F, 0xD3, 0x00, 0x40, 0x8D, 0x14, 0x20, 0x02, 0xA1, 0xC8, 0xDA, 0x02, 0x81, 0x19, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF,
  };
  ts21_status.signature = 0x54533231;
  if (!ts21_board_init()) {
    ts21_fault(TS21_ERROR_CLOCK);
  }
  if (!ts21_oled_write(0, display_init, sizeof(display_init))) {
    ts21_fault(TS21_ERROR_I2C_BUS);
  }

  uint8_t  previous  = 0;
  uint8_t  candidate = 0;
  unsigned stable    = 0;
  for (;;) {
    ts21_heater_inhibit();
    ts21_watchdog_refresh();
    ts21_uart_poll();
    const uint8_t keys  = ts21_buttons();
    ts21_status.buttons = keys;
    if (keys != candidate) {
      candidate = keys;
      stable    = 0;
    } else if (stable < 2) {
      ++stable;
    } else if (keys != previous) {
      if ((keys & 1U) && !(previous & 1U)) {
        ts21_status.page = (uint8_t)((ts21_status.page + 1) % 8);
      } else if ((keys & 2U) && !(previous & 2U)) {
        ts21_status.page = (uint8_t)((ts21_status.page + 7) % 8);
      }
      previous = keys;
    }
    ts21_status.adc_valid_mask = 0;
    for (unsigned i = 0; i < sizeof(channels); ++i) {
      uint16_t reading;
      if (ts21_adc_read(channels[i], &reading)) {
        ts21_status.raw_adc[i] = reading;
        ts21_status.adc_valid_mask |= (uint8_t)(1U << i);
      }
    }
    draw();
    if (!refresh()) {
      ts21_fault(TS21_ERROR_I2C_BUS);
    }
    ++ts21_status.iterations;
    if (!ts21_delay_us(20000)) {
      ts21_fault(TS21_ERROR_CLOCK);
    }
  }
}
