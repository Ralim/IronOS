/**
 * \file md2.h
 *
 * \brief MD2 message digest algorithm (hash function)
 *
 * \warning MD2 is considered a weak message digest and its use constitutes a
 *          security risk. We recommend considering stronger message digests
 *          instead.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#ifndef MBEDTLS_MD2_H
#define MBEDTLS_MD2_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(MBEDTLS_MD2_ALT)
// Regular implementation
//

/**
 * \brief          MD2 context structure
 *
 * \warning        MD2 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
typedef struct mbedtls_md2_context {
    unsigned char cksum[16];  /*!< checksum of the data block */
    unsigned char state[48];  /*!< intermediate digest state  */
    unsigned char buffer[16]; /*!< data block being processed */
    size_t left;              /*!< amount of data in buffer   */
} mbedtls_md2_context;

#else /* MBEDTLS_MD2_ALT */
#include "md2_alt.h"
#endif /* MBEDTLS_MD2_ALT */

/**
 * \brief          Initialize MD2 context
 *
 * \param ctx      MD2 context to be initialized
 *
 * \warning        MD2 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
void mbedtls_md2_init(mbedtls_md2_context *ctx);

/**
 * \brief          Clear MD2 context
 *
 * \param ctx      MD2 context to be cleared
 *
 * \warning        MD2 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
void mbedtls_md2_free(mbedtls_md2_context *ctx);

/**
 * \brief          Clone (the state of) an MD2 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 *
 * \warning        MD2 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
void mbedtls_md2_clone(mbedtls_md2_context *dst,
                       const mbedtls_md2_context *src);

/**
 * \brief          MD2 context setup
 *
 * \param ctx      context to be initialized
 *
 * \return         0 if successful
 *
 * \warning        MD2 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
int mbedtls_md2_starts_ret(mbedtls_md2_context *ctx);

/**
 * \brief          MD2 process buffer
 *
 * \param ctx      MD2 context
 * \param input    buffer holding the data
 * \param ilen     length of the input data
 *
 * \return         0 if successful
 *
 * \warning        MD2 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
int mbedtls_md2_update_ret(mbedtls_md2_context *ctx,
                           const unsigned char *input,
                           size_t ilen);

/**
 * \brief          MD2 final digest
 *
 * \param ctx      MD2 context
 * \param output   MD2 checksum result
 *
 * \return         0 if successful
 *
 * \warning        MD2 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
int mbedtls_md2_finish_ret(mbedtls_md2_context *ctx,
                           unsigned char output[16]);

/**
 * \brief          MD2 process data block (internal use only)
 *
 * \param ctx      MD2 context
 *
 * \return         0 if successful
 *
 * \warning        MD2 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
int mbedtls_internal_md2_process(mbedtls_md2_context *ctx);

/**
 * \brief          Output = MD2( input buffer )
 *
 * \param input    buffer holding the data
 * \param ilen     length of the input data
 * \param output   MD2 checksum result
 *
 * \warning        MD2 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
int mbedtls_md2_ret(const unsigned char *input,
                    size_t ilen,
                    unsigned char output[16]);

#if defined(MBEDTLS_SELF_TEST)

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 *
 * \warning        MD2 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
int mbedtls_md2_self_test(int verbose);

#endif /* MBEDTLS_SELF_TEST */

#ifdef __cplusplus
}
#endif

#endif /* mbedtls_md2.h */
