
#include "crc16.hpp"
#define POLYNOM 0x8005
unsigned int crc16(unsigned int crcValue, unsigned char newByte) {
  unsigned char i;

  for (i = 0; i < 8; i++) {

    if (((crcValue & 0x8000) >> 8) ^ (newByte & 0x80)) {
      crcValue = (crcValue << 1) ^ POLYNOM;
    } else {
      crcValue = (crcValue << 1);
    }

    newByte <<= 1;
  }

  return crcValue;
}