#pragma once

#include <stdint.h>

#define DEFAULT_POLY 0x973afb51

template <uint32_t polynomial = DEFAULT_POLY> struct CRC32Table {
  constexpr CRC32Table() : table() {
    for (uint32_t i = 0; i < 256; i++) {
      uint32_t c = i;
      for (auto j = 0; j < 8; j++) {
        if (c & 1) {
          c = polynomial ^ (c >> 1);
        } else {
          c >>= 1;
        }
      }
      table[i] = c;
    }
  }
  uint32_t table[256];
  uint32_t computeCRC32(uint32_t initial, const uint8_t *buf, int len) {
    uint32_t c = initial ^ 0xFFFFFFFF;
    for (auto i = 0; i < len; ++i) {
      c = table[(c ^ buf[i]) & 0xFF] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFF;
  }
};
