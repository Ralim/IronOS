/**
 * \file ecp_invasive.h
 *
 * \brief ECP module: interfaces for invasive testing only.
 *
 * The interfaces in this file are intended for testing purposes only.
 * They SHOULD NOT be made available in library integrations except when
 * building the library for testing.
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
#ifndef MBEDTLS_ECP_INVASIVE_H
#define MBEDTLS_ECP_INVASIVE_H

#include "common.h"
#include "mbedtls/ecp.h"

#if defined(MBEDTLS_TEST_HOOKS) && defined(MBEDTLS_ECP_C)

#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED) || \
    defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) || \
    defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
/* Preconditions:
        *   - bits is a multiple of 64 or is 224
        *   - c is -1 or -2
        *   - 0 <= N < 2^bits
        *   - N has room for bits plus one limb
        *
        * Behavior:
        * Set N to c * 2^bits + old_value_of_N.
        */
void mbedtls_ecp_fix_negative(mbedtls_mpi *N, signed char c, size_t bits);
#endif

#endif /* MBEDTLS_TEST_HOOKS && MBEDTLS_ECP_C */

#endif /* MBEDTLS_ECP_INVASIVE_H */
