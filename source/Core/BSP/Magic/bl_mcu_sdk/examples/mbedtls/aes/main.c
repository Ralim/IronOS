/**
 * @file main.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "bflb_platform.h"
#include "hal_uart.h"
#include "mbedtls/aes.h"
#include "mbedtls/ccm.h"
#include "mbedtls/cipher.h"
#include "cipher_wrap.h"

#define USE_MBEDTLS_DATA (0)

#if USE_MBEDTLS_DATA
#define NB_TESTS                (3)
#define CCM_SELFTEST_PT_MAX_LEN (24)
#define CCM_SELFTEST_CT_MAX_LEN (32)
/*
 * The data is the same for all tests, only the used length changes
 * data from mbedtls test case
 */
static const unsigned char key_test_data[] = {
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
};

static const unsigned char iv_test_data[] = {
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b
};

static const unsigned char add_test_data[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13
};

static const unsigned char msg_test_data[CCM_SELFTEST_PT_MAX_LEN] = {
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2a,
    0x2b,
    0x2c,
    0x2d,
    0x2e,
    0x2f,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
};

static const size_t iv_len_test_data[NB_TESTS] = { 7, 8, 12 };
static const size_t add_len_test_data[NB_TESTS] = { 8, 16, 20 };
static const size_t msg_len_test_data[NB_TESTS] = { 4, 16, 24 };
static const size_t tag_len_test_data[NB_TESTS] = { 4, 6, 8 };

static const unsigned char res_test_data[NB_TESTS][CCM_SELFTEST_CT_MAX_LEN] = {
    { 0x71, 0x62, 0x01, 0x5b, 0x4d, 0xac, 0x25, 0x5d },
    { 0xd2, 0xa1, 0xf0, 0xe0, 0x51, 0xea, 0x5f, 0x62,
      0x08, 0x1a, 0x77, 0x92, 0x07, 0x3d, 0x59, 0x3d,
      0x1f, 0xc6, 0x4f, 0xbf, 0xac, 0xcd },
    { 0xe3, 0xb2, 0x01, 0xa9, 0xf5, 0xb7, 0x1a, 0x7a,
      0x9b, 0x1c, 0xea, 0xec, 0xcd, 0x97, 0xe7, 0x0b,
      0x61, 0x76, 0xaa, 0xd9, 0xa4, 0x42, 0x8a, 0xa5,
      0x48, 0x43, 0x92, 0xfb, 0xc1, 0xb0, 0x99, 0x51 }
};

unsigned char plaintext[CCM_SELFTEST_PT_MAX_LEN];
unsigned char ciphertext[CCM_SELFTEST_CT_MAX_LEN];

#else
/* data from rfc3610 vector22 */
// rfc3610.txt
/*
   =============== Packet Vector #22 ==================
   AES Key =  D7 82 8D 13  B2 B0 BD C3  25 A7 62 36  DF 93 CC 6B
   Nonce =    00 5B 8C CB  CD 9A F8 3C  96 96 76 6C  FA
   Total packet length = 31. [Input with 12 cleartext header octets]
    Adata -> [EC 46 BB 63  B0 25 20 C3  3C 49 FD 70][B9 6B 49 E2 <- Plaintext
              1D 62 17 41  63 28 75 DB  7F 6C 92 43  D2 D7 C2]
   CBC IV in: 61[00 5B 8C  CB CD 9A F8  3C 96 96 76  6C FA]00 13
   CBC IV out:91 14 AD 06  B6 CC 02 35  76 9A B6 14  C4 82 95 03
   After xor: 91 18 41 40  0D AF B2 10  56 59 8A 5D  39 F2 95 03   [hdr]
   After AES: 29 BD 7C 27  83 E3 E8 D3  C3 5C 01 F4  4C EC BB FA
   After xor: 90 D6 35 C5  9E 81 FF 92  A0 74 74 2F  33 80 29 B9   [msg]
   After AES: 4E DA F4 0D  21 0B D4 5F  FE 97 90 B9  AA EC 34 4C
   After xor: 9C 0D 36 0D  21 0B D4 5F  FE 97 90 B9  AA EC 34 4C   [msg]
   After AES: 21 9E F8 90  EA 64 C2 11  A5 37 88 83  E1 BA 22 0D
   CBC-MAC  : 21 9E F8 90  EA 64 C2 11  A5 37
   CTR Start: 01 00 5B 8C  CB CD 9A F8  3C 96 96 76  6C FA 00 01
   CTR[0001]: 88 BC 19 42  80 C1 FA 3E  BE FC EF FB  4D C6 2D 54
   CTR[0002]: 3E 59 7D A5  AE 21 CC A4  00 9E 4C 0C  91 F6 22 49
   CTR[MAC ]: 5C BC 30 98  66 02 A9 F4  64 A0
   Total packet length = 41. [Authenticated and Encrypted Output]
              EC 46 BB 63  B0 25 20 C3  3C 49 FD 70  31 D7 50 A0
              9D A3 ED 7F  DD D4 9A 20  32 AA BF 17  EC 8E BF 7D
              22 C8 08 8C  66 6B E5 C1  97
*/
#define IV_DATA_LEN  13
#define ADD_DATA_LEN 12
#define MSG_DATA_LEN 19
#define TAG_DATA_LEN 10
#define KEY_DATA_LEN 16

static const unsigned char key_test_data[KEY_DATA_LEN] = {
    0xD7, 0x82, 0x8D, 0x13, 0xB2, 0xB0, 0xBD, 0xC3,
    0x25, 0xA7, 0x62, 0x36, 0xDF, 0x93, 0xCC, 0x6B
};

static const unsigned char iv_test_data[IV_DATA_LEN] = {
    0x00, 0x5B, 0x8C, 0xCB, 0xCD, 0x9A, 0xF8, 0x3C,
    0x96, 0x96, 0x76, 0x6C, 0xFA
};

static const unsigned char msg_test_data[MSG_DATA_LEN] = {
    0xB9, 0x6B, 0x49, 0xE2, 0x1D, 0x62, 0x17, 0x41,
    0x63, 0x28, 0x75, 0xDB, 0x7F, 0x6C, 0x92, 0x43,
    0xD2, 0xD7, 0xC2
};

static const unsigned char add_test_data[ADD_DATA_LEN] = {
    0xEC,
    0x46,
    0xBB,
    0x63,
    0xB0,
    0x25,
    0x20,
    0xC3,
    0x3C,
    0x49,
    0xFD,
    0x70,
};

unsigned char plaintext[MSG_DATA_LEN];
unsigned char ciphertext[MSG_DATA_LEN + TAG_DATA_LEN];
#endif

int main(void)
{
    bflb_platform_init(0);
    int ret = 0;

#if USE_MBEDTLS_DATA
    int i = 0;
    mbedtls_ccm_context ccm_ctx = { 0 };
    mbedtls_ccm_init(&ccm_ctx);
    mbedtls_ccm_setkey(&ccm_ctx, MBEDTLS_CIPHER_ID_AES, key_test_data, 128);

    for (i = 0; i < NB_TESTS; i++) {
        arch_memset(plaintext, 0, CCM_SELFTEST_PT_MAX_LEN);
        arch_memset(ciphertext, 0, CCM_SELFTEST_CT_MAX_LEN);
        arch_memcpy(plaintext, msg_test_data, msg_len_test_data[i]);
        ret = mbedtls_ccm_encrypt_and_tag(&ccm_ctx, msg_len_test_data[i],
                                          iv_test_data, iv_len_test_data[i],
                                          add_test_data, add_len_test_data[i],
                                          plaintext, ciphertext,
                                          ciphertext + msg_len_test_data[i],
                                          tag_len_test_data[i]);

        if (ret != 0 || arch_memcmp(ciphertext, res_test_data[i], msg_len_test_data[i] + tag_len_test_data[i]) != 0) {
            MSG("CCM enc fail \r\n");
            BL_CASE_FAIL
        }

        arch_memset(plaintext, 0, CCM_SELFTEST_PT_MAX_LEN);

        ret = mbedtls_ccm_auth_decrypt(&ccm_ctx, msg_len_test_data[i],
                                       iv_test_data, iv_len_test_data[i],
                                       add_test_data, add_len_test_data[i],
                                       ciphertext, plaintext,
                                       ciphertext + msg_len_test_data[i],
                                       tag_len_test_data[i]);

        if (ret != 0 || arch_memcmp(plaintext, msg_test_data, msg_len_test_data[i]) != 0) {
            MSG("CCM dec fail \r\n");
            BL_CASE_FAIL
        }
    }

#else
    // mbedtls_aes_context aes_ctx={0};
    mbedtls_ccm_context ccm_ctx = { 0 };
    // mbedtls_aes_init(&aes_ctx);
    mbedtls_ccm_init(&ccm_ctx);
    mbedtls_ccm_setkey(&ccm_ctx, MBEDTLS_CIPHER_ID_AES, key_test_data, 128);
    arch_memset(plaintext, 0, MSG_DATA_LEN);
    arch_memset(ciphertext, 0, MSG_DATA_LEN + TAG_DATA_LEN);
    arch_memcpy(plaintext, msg_test_data, MSG_DATA_LEN);
    mbedtls_ccm_encrypt_and_tag(&ccm_ctx, MSG_DATA_LEN, iv_test_data, IV_DATA_LEN, add_test_data,
                                ADD_DATA_LEN, plaintext, ciphertext, ciphertext + MSG_DATA_LEN, TAG_DATA_LEN);

    arch_memset(plaintext, 0, MSG_DATA_LEN);
    ret = mbedtls_ccm_auth_decrypt(&ccm_ctx, MSG_DATA_LEN, iv_test_data, IV_DATA_LEN, add_test_data,
                                   ADD_DATA_LEN, ciphertext, plaintext, ciphertext + MSG_DATA_LEN, TAG_DATA_LEN);

    if (ret != 0) {
        MSG("CCM dec fail \r\n");
        BL_CASE_FAIL;
    }

#endif

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
