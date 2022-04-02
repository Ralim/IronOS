/*
 *  Context structure declaration of the Mbed TLS software-based PSA drivers
 *  called through the PSA Crypto driver dispatch layer.
 *  This file contains the context structures of those algorithms which do not
 *  rely on other algorithms, i.e. are 'primitive' algorithms.
 *
 * \note This file may not be included directly. Applications must
 * include psa/crypto.h.
 *
 * \note This header and its content is not part of the Mbed TLS API and
 * applications must not depend on it. Its main purpose is to define the
 * multi-part state objects of the Mbed TLS software-based PSA drivers. The
 * definition of these objects are then used by crypto_struct.h to define the
 * implementation-defined types of PSA multi-part state objects.
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
 */

#ifndef PSA_CRYPTO_BUILTIN_PRIMITIVES_H
#define PSA_CRYPTO_BUILTIN_PRIMITIVES_H

#include <psa/crypto_driver_common.h>

/*
 * Hash multi-part operation definitions.
 */

#include "mbedtls/md2.h"
#include "mbedtls/md4.h"
#include "mbedtls/md5.h"
#include "mbedtls/ripemd160.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"

#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD2) ||       \
    defined(MBEDTLS_PSA_BUILTIN_ALG_MD4) ||       \
    defined(MBEDTLS_PSA_BUILTIN_ALG_MD5) ||       \
    defined(MBEDTLS_PSA_BUILTIN_ALG_RIPEMD160) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_1) ||     \
    defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_224) ||   \
    defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_256) ||   \
    defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_384) ||   \
    defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_512)
#define MBEDTLS_PSA_BUILTIN_HASH
#endif

typedef struct
{
    psa_algorithm_t alg;
    union {
        unsigned dummy; /* Make the union non-empty even with no supported algorithms. */
#if defined(MBEDTLS_MD2_C)
        mbedtls_md2_context md2;
#endif
#if defined(MBEDTLS_MD4_C)
        mbedtls_md4_context md4;
#endif
#if defined(MBEDTLS_MD5_C)
        mbedtls_md5_context md5;
#endif
#if defined(MBEDTLS_RIPEMD160_C)
        mbedtls_ripemd160_context ripemd160;
#endif
#if defined(MBEDTLS_SHA1_C)
        mbedtls_sha1_context sha1;
#endif
#if defined(MBEDTLS_SHA256_C)
        mbedtls_sha256_context sha256;
#endif
#if defined(MBEDTLS_SHA512_C)
        mbedtls_sha512_context sha512;
#endif
    } ctx;
} mbedtls_psa_hash_operation_t;

#define MBEDTLS_PSA_HASH_OPERATION_INIT \
    {                                   \
        0,                              \
        {                               \
            0                           \
        }                               \
    }

/*
 * Cipher multi-part operation definitions.
 */

#include "mbedtls/cipher.h"

#if defined(MBEDTLS_PSA_BUILTIN_ALG_STREAM_CIPHER) ||  \
    defined(MBEDTLS_PSA_BUILTIN_ALG_CTR) ||            \
    defined(MBEDTLS_PSA_BUILTIN_ALG_CFB) ||            \
    defined(MBEDTLS_PSA_BUILTIN_ALG_OFB) ||            \
    defined(MBEDTLS_PSA_BUILTIN_ALG_XTS) ||            \
    defined(MBEDTLS_PSA_BUILTIN_ALG_ECB_NO_PADDING) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_CBC_NO_PADDING) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_CBC_PKCS7)
#define MBEDTLS_PSA_BUILTIN_CIPHER 1
#endif

typedef struct
{
    /* Context structure for the Mbed TLS cipher implementation. */
    psa_algorithm_t alg;
    uint8_t iv_length;
    uint8_t block_length;
    mbedtls_cipher_context_t cipher;
} mbedtls_psa_cipher_operation_t;

#define MBEDTLS_PSA_CIPHER_OPERATION_INIT \
    {                                     \
        0, 0, 0,                          \
        {                                 \
            0                             \
        }                                 \
    }

/*
 * BEYOND THIS POINT, TEST DRIVER DECLARATIONS ONLY.
 */
#if defined(PSA_CRYPTO_DRIVER_TEST)

typedef mbedtls_psa_hash_operation_t mbedtls_transparent_test_driver_hash_operation_t;

#define MBEDTLS_TRANSPARENT_TEST_DRIVER_HASH_OPERATION_INIT MBEDTLS_PSA_HASH_OPERATION_INIT

typedef mbedtls_psa_cipher_operation_t
    mbedtls_transparent_test_driver_cipher_operation_t;

typedef struct
{
    unsigned int initialised : 1;
    mbedtls_transparent_test_driver_cipher_operation_t ctx;
} mbedtls_opaque_test_driver_cipher_operation_t;

#define MBEDTLS_TRANSPARENT_TEST_DRIVER_CIPHER_OPERATION_INIT \
    MBEDTLS_PSA_CIPHER_OPERATION_INIT

#define MBEDTLS_OPAQUE_TEST_DRIVER_CIPHER_OPERATION_INIT         \
    {                                                            \
        0, MBEDTLS_TRANSPARENT_TEST_DRIVER_CIPHER_OPERATION_INIT \
    }

#endif /* PSA_CRYPTO_DRIVER_TEST */

#endif /* PSA_CRYPTO_BUILTIN_PRIMITIVES_H */
