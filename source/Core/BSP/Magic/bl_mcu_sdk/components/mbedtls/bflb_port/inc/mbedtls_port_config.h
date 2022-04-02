#ifndef __MBEDTLS_PORT_CONFIG_H__
#define __MBEDTLS_PORT_CONFIG_H__

#include "bflb_platform.h"
#include "bl702_sec_eng.h"

#define MBEDTLS_USE_BFLB_SHA
#define MBEDTLS_USE_BFLB_AES
#define MBEDTLS_USE_BFLB_TRNG
#define MBEDTLS_USE_BFLB_PKA
#define MBEDTLS_USE_BFLB_GMAC

#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
#define MBEDTLS_PLATFORM_PRINTF_MACRO MSG
#define MBEDTLS_PLATFORM_SNPRINTF_ALT
//#define MBEDTLS_PLATFORM_STD_SNPRINTF    snprintf
#define MBEDTLS_PLATFORM_MEMORY
//#if defined(bflb_platform_calloc) && defined(bflb_platform_free)
//#define MBEDTLS_PLATFORM_CALLOC_MACRO    bflb_platform_calloc
//#define MBEDTLS_PLATFORM_FREE_MACRO      bflb_platform_free
//#endif

#ifndef SIZE_MAX
#define SIZE_MAX 0xffffffff
#endif

#endif