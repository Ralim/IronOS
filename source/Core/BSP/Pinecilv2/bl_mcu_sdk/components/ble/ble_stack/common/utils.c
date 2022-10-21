/*****************************************************************************************
*
* @file utils.c
*
* @brief entry
*
* Copyright (C) Bouffalo Lab 2019
*
* History: 2019-11 crealted by Lanlan Gong @ Shanghai
*
*****************************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

void reverse_bytearray(uint8_t *src, uint8_t *result, int array_size)
{
    for (int i = 0; i < array_size; i++) {
        result[array_size - i - 1] = src[i];
    }
}

unsigned int find_msb_set(uint32_t data)
{
    uint32_t count = 0;
    uint32_t mask = 0x80000000;

    if (!data) {
        return 0;
    }
    while ((data & mask) == 0) {
        count += 1u;
        mask = mask >> 1u;
    }
    return (32 - count);
}

unsigned int find_lsb_set(uint32_t data)
{
    uint32_t count = 0;
    uint32_t mask = 0x00000001;

    if (!data) {
        return 0;
    }
    while ((data & mask) == 0) {
        count += 1u;
        mask = mask << 1u;
    }
    return (1 + count);
}

int char2hex(char c, uint8_t *x)
{
    if (c >= '0' && c <= '9') {
        *x = c - '0';
    } else if (c >= 'a' && c <= 'f') {
        *x = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        *x = c - 'A' + 10;
    } else {
        return -1;
    }

    return 0;
}

int hex2char(uint8_t x, char *c)
{
    if (x <= 9) {
        *c = x + '0';
    } else if (x <= 15) {
        *c = x - 10 + 'a';
    } else {
        return -1;
    }

    return 0;
}

size_t bin2hex(const uint8_t *buf, size_t buflen, char *hex, size_t hexlen)
{
    if ((hexlen + 1) < buflen * 2) {
        return 0;
    }

    for (size_t i = 0; i < buflen; i++) {
        if (hex2char(buf[i] >> 4, &hex[2 * i]) < 0) {
            return 0;
        }
        if (hex2char(buf[i] & 0xf, &hex[2 * i + 1]) < 0) {
            return 0;
        }
    }

    hex[2 * buflen] = '\0';
    return 2 * buflen;
}

size_t hex2bin(const char *hex, size_t hexlen, uint8_t *buf, size_t buflen)
{
    uint8_t dec;

    if (buflen < hexlen / 2 + hexlen % 2) {
        return 0;
    }

    /* if hexlen is uneven, insert leading zero nibble */
    if (hexlen % 2) {
        if (char2hex(hex[0], &dec) < 0) {
            return 0;
        }
        buf[0] = dec;
        hex++;
        buf++;
    }

    /* regular hex conversion */
    for (size_t i = 0; i < hexlen / 2; i++) {
        if (char2hex(hex[2 * i], &dec) < 0) {
            return 0;
        }
        buf[i] = dec << 4;

        if (char2hex(hex[2 * i + 1], &dec) < 0) {
            return 0;
        }
        buf[i] += dec;
    }

    return hexlen / 2 + hexlen % 2;
}
