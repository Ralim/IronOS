/*
 * BriefLZ - small fast Lempel-Ziv
 *
 * C/C++ header file
 *
 * Copyright (c) 2002-2020 Joergen Ibsen
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must
 *      not claim that you wrote the original software. If you use this
 *      software in a product, an acknowledgment in the product
 *      documentation would be appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must
 *      not be misrepresented as being the original software.
 *
 *   3. This notice may not be removed or altered from any source
 *      distribution.
 */

#ifndef BRIEFLZ_H_INCLUDED
#define BRIEFLZ_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BLZ_VER_MAJOR 1        /**< Major version number */
#define BLZ_VER_MINOR 3        /**< Minor version number */
#define BLZ_VER_PATCH 0        /**< Patch version number */
#define BLZ_VER_STRING "1.3.0" /**< Version number as a string */

#ifdef BLZ_DLL
#  if defined(_WIN32) || defined(__CYGWIN__)
#    ifdef BLZ_DLL_EXPORTS
#      define BLZ_API __declspec(dllexport)
#    else
#      define BLZ_API __declspec(dllimport)
#    endif
#    define BLZ_LOCAL
#  else
#    if __GNUC__ >= 4
#      define BLZ_API __attribute__ ((visibility ("default")))
#      define BLZ_LOCAL __attribute__ ((visibility ("hidden")))
#    else
#      define BLZ_API
#      define BLZ_LOCAL
#    endif
#  endif
#else
#  define BLZ_API
#  define BLZ_LOCAL
#endif

/**
 * Return value on error.
 *
 * @see blz_depack_safe
 */
#ifndef BLZ_ERROR
#  define BLZ_ERROR ((unsigned long) (-1))
#endif

/**
 * Get bound on compressed data size.
 *
 * @see blz_pack
 *
 * @param src_size number of bytes to compress
 * @return maximum size of compressed data
 */
BLZ_API size_t
blz_max_packed_size(size_t src_size);

/**
 * Get required size of `workmem` buffer.
 *
 * @see blz_pack
 *
 * @param src_size number of bytes to compress
 * @return required size in bytes of `workmem` buffer
 */
BLZ_API size_t
blz_workmem_size(size_t src_size);

/**
 * Compress `src_size` bytes of data from `src` to `dst`.
 *
 * @param src pointer to data
 * @param dst pointer to where to place compressed data
 * @param src_size number of bytes to compress
 * @param workmem pointer to memory for temporary use
 * @return size of compressed data
 */
BLZ_API unsigned long
blz_pack(const void *src, void *dst, unsigned long src_size, void *workmem);

/**
 * Get required size of `workmem` buffer.
 *
 * @see blz_pack_level
 *
 * @param src_size number of bytes to compress
 * @param level compression level
 * @return required size in bytes of `workmem` buffer
 */
BLZ_API size_t
blz_workmem_size_level(size_t src_size, int level);

/**
 * Compress `src_size` bytes of data from `src` to `dst`.
 *
 * Compression levels between 1 and 9 offer a trade-off between
 * time/space and ratio. Level 10 is optimal but very slow.
 *
 * @param src pointer to data
 * @param dst pointer to where to place compressed data
 * @param src_size number of bytes to compress
 * @param workmem pointer to memory for temporary use
 * @param level compression level
 * @return size of compressed data
 */
BLZ_API unsigned long
blz_pack_level(const void *src, void *dst, unsigned long src_size,
               void *workmem, int level);

/**
 * Decompress `depacked_size` bytes of data from `src` to `dst`.
 *
 * @param src pointer to compressed data
 * @param dst pointer to where to place decompressed data
 * @param depacked_size size of decompressed data
 * @return size of decompressed data
 */
BLZ_API unsigned long
blz_depack(const void *src, void *dst, unsigned long depacked_size);

/**
 * Decompress `depacked_size` bytes of data from `src` to `dst`.
 *
 * Reads at most `src_size` bytes from `src`.
 * Writes at most `depacked_size` bytes to `dst`.
 *
 * @param src pointer to compressed data
 * @param src_size size of compressed data
 * @param dst pointer to where to place decompressed data
 * @param depacked_size size of decompressed data
 * @return size of decompressed data, `BLZ_ERROR` on error
 */
BLZ_API unsigned long
blz_depack_safe(const void *src, unsigned long src_size,
                void *dst, unsigned long depacked_size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BRIEFLZ_H_INCLUDED */
