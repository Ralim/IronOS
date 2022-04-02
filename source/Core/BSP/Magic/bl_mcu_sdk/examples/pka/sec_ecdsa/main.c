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
#include "hal_sec_ecdsa.h"

/** @addtogroup  BFLB_Peripheral_Case
 *  @{
 */

/** @addtogroup  SEC_ECDSA
 *  @{
 */

/** @defgroup  SEC_ECDSA_Private_Macros
 *  @{
 */

/*@} end of group SEC_ECDSA_Private_Macros */

/** @defgroup  SEC_ECDSA_Private_Types
 *  @{
 */

/*@} end of group SEC_ECDSA_Private_Types */

/** @defgroup  SEC_ECDSA_Private_Variables
 *  @{
 */

/*@} end of group SEC_ECDSA_Private_Variables */

/** @defgroup  SEC_ECDSA_Global_Variables
 *  @{
 */

/*@} end of group SEC_ECDSA_Global_Variables */

/** @defgroup  SEC_ECDSA_Private_Fun_Declaration
 *  @{
 */

/*@} end of group SEC_ECDSA_Private_Fun_Declaration */

/** @defgroup  SEC_ECDSA_Private_Functions
 *  @{
 */

int main(void)
{
    /* We use following test vector to show how BFLB_ECDSA works��
     * this can be found in https://tools.ietf.org/html/rfc6979#page-33 */
#if 0
    A.2.5.  ECDSA, 256 Bits (Prime Field)

    Key pair:

    curve: NIST P-256

    q = FFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551
    (qlen = 256 bits)

    private key:

    x = C9AFA9D845BA75166B5C215767B1D6934E50C3DB36E89B127B8A622B120F6721

    public key: U = xG

    Ux = 60FED4BA255A9D31C961EB74C6356D68C049B8923B61FA6CE669622E60F29FB6

    Uy = 7903FE1008B8BC99A41AE9E95628BC64F2F1B20C2D7E9F5177A3C294D4462299

    Signatures:

    With SHA-1, message = "sample":
    k = 882905F1227FD620FBF2ABF21244F0BA83D0DC3A9103DBBEE43A1FB858109DB4
    r = 61340C88C3AAEBEB4F6D667F672CA9759A6CCAA9FA8811313039EE4A35471D32
    s = 6D7F147DAC089441BB2E2FE8F7A3FA264B9C475098FDCF6E00D7C996E1B8B7EB

    With SHA-224, message = "sample":
    k = 103F90EE9DC52E5E7FB5132B7033C63066D194321491862059967C715985D473
    r = 53B2FFF5D1752B2C689DF257C04C40A587FABABB3F6FC2702F1343AF7CA9AA3F
    s = B9AFB64FDC03DC1A131C7D2386D11E349F070AA432A4ACC918BEA988BF75C74C

    With SHA-256, message = "sample":
    k = A6E3C57DD01ABE90086538398355DD4C3B17AA873382B0F24D6129493D8AAD60
    r = EFD48B2AACB6A8FD1140DD9CD45E81D69D2C877B56AAF991C34D0EA84EAF3716
    s = F7CB1C942D657C41D436C7A1B6E29F65F3E900DBB9AFF4064DC4AB2F843ACDA8

    With SHA-384, message = "sample":
    k = 09F634B188CEFD98E7EC88B1AA9852D734D0BC272F7D2A47DECC6EBEB375AAD4
    r = 0EAFEA039B20E9B42309FB1D89E213057CBF973DC0CFC8F129EDDDC800EF7719
    s = 4861F0491E6998B9455193E34E7B0D284DDD7149A74B95B9261F13ABDE940954

    With SHA-512, message = "sample":
    k = 5FA81C63109BADB88C1F367B47DA606DA28CAD69AA22C4FE6AD7DF73A7173AA5
    r = 8496A60B5E9B47C825488827E0495B0E3FA109EC4568FD3F8D1097678EB97F00
    s = 2362AB1ADBE2B8ADF9CB9EDAB740EA6049C028114F2460F96554F61FAE3302FE
#endif
    uint32_t i = 0;
    uint8_t *p;

    uint8_t ecdsa_hash[32] = { 0xAF, 0x2B, 0xDB, 0xE1, 0xAA, 0x9B, 0x6E, 0xC1, 0xE2, 0xAD, 0xE1, 0xD6, 0x94, 0xF4, 0x1F, 0xC7,
                               0x1A, 0x83, 0x1D, 0x02, 0x68, 0xE9, 0x89, 0x15, 0x62, 0x11, 0x3D, 0x8A, 0x62, 0xAD, 0xD1, 0xBF };
    uint8_t ecdsa_private_key[32] = { 0xC9, 0xAF, 0xA9, 0xD8, 0x45, 0xBA, 0x75, 0x16, 0x6B, 0x5C, 0x21, 0x57, 0x67, 0xB1, 0xD6, 0x93,
                                      0x4E, 0x50, 0xC3, 0xDB, 0x36, 0xE8, 0x9B, 0x12, 0x7B, 0x8A, 0x62, 0x2B, 0x12, 0x0F, 0x67, 0x21 };
    uint8_t ecdsa_public_keyx[32] = { 0x60, 0xFE, 0xD4, 0xBA, 0x25, 0x5A, 0x9D, 0x31, 0xC9, 0x61, 0xEB, 0x74, 0xC6, 0x35, 0x6D, 0x68,
                                      0xC0, 0x49, 0xB8, 0x92, 0x3B, 0x61, 0xFA, 0x6C, 0xE6, 0x69, 0x62, 0x2E, 0x60, 0xF2, 0x9F, 0xB6 };
    uint8_t ecdsa_public_keyy[32] = { 0x79, 0x03, 0xFE, 0x10, 0x08, 0xB8, 0xBC, 0x99, 0xA4, 0x1A, 0xE9, 0xE9, 0x56, 0x28, 0xBC, 0x64,
                                      0xF2, 0xF1, 0xB2, 0x0C, 0x2D, 0x7E, 0x9F, 0x51, 0x77, 0xA3, 0xC2, 0x94, 0xD4, 0x46, 0x22, 0x99 };
    uint8_t ecdsa_k[32] = { 0xA6, 0xE3, 0xC5, 0x7D, 0xD0, 0x1A, 0xBE, 0x90, 0x08, 0x65, 0x38, 0x39, 0x83, 0x55, 0xDD, 0x4C,
                            0x3B, 0x17, 0xAA, 0x87, 0x33, 0x82, 0xB0, 0xF2, 0x4D, 0x61, 0x29, 0x49, 0x3D, 0x8A, 0xAD, 0x60 };
    uint8_t ecdsa_r[32] = { 0xEF, 0xD4, 0x8B, 0x2A, 0xAC, 0xB6, 0xA8, 0xFD, 0x11, 0x40, 0xDD, 0x9C, 0xD4, 0x5E, 0x81, 0xD6,
                            0x9D, 0x2C, 0x87, 0x7B, 0x56, 0xAA, 0xF9, 0x91, 0xC3, 0x4D, 0x0E, 0xA8, 0x4E, 0xAF, 0x37, 0x16 };
    uint8_t ecdsa_s[32] = { 0xF7, 0xCB, 0x1C, 0x94, 0x2D, 0x65, 0x7C, 0x41, 0xD4, 0x36, 0xC7, 0xA1, 0xB6, 0xE2, 0x9F, 0x65,
                            0xF3, 0xE9, 0x00, 0xDB, 0xB9, 0xAF, 0xF4, 0x06, 0x4D, 0xC4, 0xAB, 0x2F, 0x84, 0x3A, 0xCD, 0xA8 };

    uint32_t tmp_keyx[8];
    uint32_t tmp_keyy[8];
    sec_ecdsa_handle_t ecdsa_handle;

    bflb_platform_init(2000000);
    MSG("ECDSA Case\r\n");

    sec_ecdsa_init(&ecdsa_handle, ECP_SECP256R1);
    ecdsa_handle.privateKey = (uint32_t *)ecdsa_private_key;
    ecdsa_handle.publicKeyx = (uint32_t *)ecdsa_public_keyx;
    ecdsa_handle.publicKeyy = (uint32_t *)ecdsa_public_keyy;

#if 0
    bflb_platform_clear_time();
    sec_ecdsa_get_public_key(&ecdsa_handle, (uint32_t *)ecdsa_private_key, tmp_keyx, tmp_keyy);
    MSG("Get public key time=%dms\r\n", (unsigned int)bflb_platform_get_time_ms());

    p = (uint8_t *)tmp_keyx;

    for(i = 0; i < 32; i++)
    {
        if(ecdsa_public_keyx[i] != p[i])
        {
            MSG("ECDSA Compare fail\r\n");
            bflb_platform_delay_ms(1000);
        }
    }

    p = (uint8_t *)tmp_keyy;

    for(i = 0; i < 32; i++)
    {
        if(ecdsa_public_keyy[i] != p[i])
        {
            MSG("ECDSA Compare fail\r\n");
            bflb_platform_delay_ms(1000);
        }
    }

#endif

    bflb_platform_clear_time();

    if (0 != sec_ecdsa_sign(&ecdsa_handle, (uint32_t *)ecdsa_k, (uint32_t *)ecdsa_hash, sizeof(ecdsa_hash) / 4, tmp_keyx, tmp_keyy)) {
        MSG("Sign Fail\r\n");
        BL_CASE_FAIL;
    }

    MSG("ECDSA sign time=%dms\r\n", (unsigned int)bflb_platform_get_time_ms());

    MSG("Check sign\r\n");
    p = (uint8_t *)tmp_keyx;

    for (i = 0; i < 32; i++) {
        if (ecdsa_r[i] != p[i]) {
            MSG("ECDSA Compare fail\r\n");
            bflb_platform_delay_ms(1000);
        }
    }

    p = (uint8_t *)tmp_keyy;

    for (i = 0; i < 32; i++) {
        if (ecdsa_s[i] != p[i]) {
            MSG("ECDSA Compare fail\r\n");
            bflb_platform_delay_ms(1000);
        }
    }

    bflb_platform_clear_time();

    if (0 != sec_ecdsa_verify(&ecdsa_handle, (uint32_t *)ecdsa_hash, sizeof(ecdsa_hash) / 4, tmp_keyx, tmp_keyy)) {
        MSG("Verify Fail\r\n");
    } else {
        MSG("ECDSA verify time=%dms\r\n", (unsigned int)bflb_platform_get_time_ms());
        MSG("Verify Success\r\n");
    }

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}

/*@} end of group SEC_ECDSA_Private_Functions */

/*@} end of group SEC_ECDSA */

/*@} end of group BFLB_Peripheral_Case */
