#include "ts21.h"

uint8_t ts21_crc8(const uint8_t *data, size_t size) {
  uint8_t crc = 0;
  for (size_t i = 0; i < size; ++i) {
    crc ^= data[i];
    for (unsigned bit = 0; bit < 8; ++bit) {
      crc = (uint8_t)((crc >> 1) ^ ((crc & 1U) ? 0x8CU : 0U));
    }
  }
  return crc;
}

bool ts21_pd_feed(struct TS21PDParser *parser, uint8_t byte, struct TS21PDFrame *frame) {
  if (parser->used == 0 && byte != 0xFF) {
    return false;
  }
  parser->bytes[parser->used++] = byte;
  if (parser->used < sizeof(parser->bytes)) {
    return false;
  }
  if (ts21_crc8(parser->bytes, 3) == parser->bytes[3]) {
    frame->voltage = parser->bytes[1];
    frame->status  = parser->bytes[2];
    parser->used   = 0;
    return true;
  }

  // Keep a possible overlapping frame after a corrupt packet.
  uint8_t start = 1;
  while (start < sizeof(parser->bytes) && parser->bytes[start] != 0xFF) {
    ++start;
  }
  parser->used = (uint8_t)(sizeof(parser->bytes) - start);
  for (uint8_t i = 0; i < parser->used; ++i) {
    parser->bytes[i] = parser->bytes[start + i];
  }
  return false;
}
