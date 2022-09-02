#ifndef _USB_UTIL_H
#define _USB_UTIL_H

#include "stdbool.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "usb_slist.h"
#include "bflb_platform.h"

#ifndef __packed
#define __packed __attribute__((__packed__))
#endif
#ifndef __aligned
#define __aligned(x) __attribute__((__aligned__(x)))
#endif
#define __may_alias __attribute__((__may_alias__))
#ifndef __printf_like
#define __printf_like(f, a) __attribute__((format(printf, f, a)))
#endif
#define __used __attribute__((__used__))
#ifndef __deprecated
#define __deprecated __attribute__((deprecated))
#endif
#define ARG_UNUSED(x) (void)(x)

// #define likely(x)   __builtin_expect((bool)!!(x), true)
// #define unlikely(x) __builtin_expect((bool)!!(x), false)

#define popcount(x) __builtin_popcount(x)

#ifndef __no_optimization
#define __no_optimization __attribute__((optimize("-O0")))
#endif

#ifndef __weak
#define __weak __attribute__((__weak__))
#endif
#define __unused __attribute__((__unused__))

#ifndef __ALIGN_BEGIN
#define __ALIGN_BEGIN
#endif
#ifndef __ALIGN_END
#define __ALIGN_END __attribute__((aligned(4)))
#endif

#ifndef LO_BYTE
#define LO_BYTE(x) ((uint8_t)(x & 0x00FF))
#endif

#ifndef HI_BYTE
#define HI_BYTE(x) ((uint8_t)((x & 0xFF00) >> 8))
#endif

/**
 * @def MAX
 * @brief The larger value between @p a and @p b.
 * @note Arguments are evaluated twice.
 */
#ifndef MAX
/* Use Z_MAX for a GCC-only, single evaluation version */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

/**
 * @def MIN
 * @brief The smaller value between @p a and @p b.
 * @note Arguments are evaluated twice.
 */
#ifndef MIN
/* Use Z_MIN for a GCC-only, single evaluation version */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef BCD
#define BCD(x) ((((x) / 10) << 4) | ((x) % 10))
#endif

#ifdef BIT
#undef BIT
#define BIT(n) (1UL << (n))
#else
#define BIT(n) (1UL << (n))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) \
    ((int)((sizeof(array) / sizeof((array)[0]))))
#endif

#ifndef BSWAP16
#define BSWAP16(u16) (__builtin_bswap16(u16))
#endif
#ifndef BSWAP32
#define BSWAP32(u32) (__builtin_bswap32(u32))
#endif

#define GET_BE16(field) \
    (((uint16_t)(field)[0] << 8) | ((uint16_t)(field)[1]))

#define GET_BE32(field) \
    (((uint32_t)(field)[0] << 24) | ((uint32_t)(field)[1] << 16) | ((uint32_t)(field)[2] << 8) | ((uint32_t)(field)[3] << 0))

#define SET_BE16(field, value)                \
    do {                                      \
        (field)[0] = (uint8_t)((value) >> 8); \
        (field)[1] = (uint8_t)((value) >> 0); \
    } while (0)

#define SET_BE24(field, value)                 \
    do {                                       \
        (field)[0] = (uint8_t)((value) >> 16); \
        (field)[1] = (uint8_t)((value) >> 8);  \
        (field)[2] = (uint8_t)((value) >> 0);  \
    } while (0)

#define SET_BE32(field, value)                 \
    do {                                       \
        (field)[0] = (uint8_t)((value) >> 24); \
        (field)[1] = (uint8_t)((value) >> 16); \
        (field)[2] = (uint8_t)((value) >> 8);  \
        (field)[3] = (uint8_t)((value) >> 0);  \
    } while (0)

#define REQTYPE_GET_DIR(x)   (((x) >> 7) & 0x01)
#define REQTYPE_GET_TYPE(x)  (((x) >> 5) & 0x03U)
#define REQTYPE_GET_RECIP(x) ((x)&0x1F)

#define GET_DESC_TYPE(x)  (((x) >> 8) & 0xFFU)
#define GET_DESC_INDEX(x) ((x)&0xFFU)

#define WBVAL(x) (x & 0xFF), ((x >> 8) & 0xFF)
#define DBVAL(x) (x & 0xFF), ((x >> 8) & 0xFF), ((x >> 16) & 0xFF), ((x >> 24) & 0xFF)

#define USB_DESC_SECTION __attribute__((section("usb_desc"))) __used __aligned(1)

#if 0
#define USBD_LOG_WRN(a, ...) bflb_platform_printf(a, ##__VA_ARGS__)
#define USBD_LOG_DBG(a, ...) bflb_platform_printf(a, ##__VA_ARGS__)
#define USBD_LOG_ERR(a, ...) bflb_platform_printf(a, ##__VA_ARGS__)
#else
#define USBD_LOG_INFO(a, ...) bflb_platform_printf(a, ##__VA_ARGS__)
#define USBD_LOG_DBG(a, ...)
#define USBD_LOG_WRN(a, ...) bflb_platform_printf(a, ##__VA_ARGS__)
#define USBD_LOG_ERR(a, ...)  bflb_platform_printf(a, ##__VA_ARGS__)
#endif

#endif