//
// Created by lujun on 19-6-21.
//
#include "riscv_math.h"
#include <stdint.h>
#include <stdlib.h>
#include "array.h"
#include "../common.h"

#include "../HelperFunctions/math_helper.c"
#include "../HelperFunctions/ref_helper.c"

#include <stdio.h>
#define DELTAF32 (0.05f)
#define DELTAQ31 (63)
#define DELTAQ15 (1)
#define DELTAQ7 (1)

#define ENABLE_ALL
#define AND
#define OR
#define NOT
#define XOR

int test_flag_error = 0;

int main()
{
    int i;

    BENCH_INIT;
#if defined AND || defined ENABLE_ALL
    BENCH_START(riscv_and_u16);
    riscv_and_u16(u16_a_array, u16_b_array, u16_out_array, ARRAY_SIZE);
    BENCH_END(riscv_and_u16);
    ref_and_u16(u16_a_array, u16_b_array, u16_out_array_ref, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(u16_out_array[i] - u16_out_array_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_and_u16);
            printf("index: %d, expect: %d, actual: %d\n", i,
                   u16_out_array_ref[i], u16_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_and_u16);

    BENCH_START(riscv_and_u32);
    riscv_and_u32(u32_a_array, u32_b_array, u32_out_array, ARRAY_SIZE);
    BENCH_END(riscv_and_u32);
    ref_and_u32(u32_a_array, u32_b_array, u32_out_array_ref, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(u32_out_array[i] - u32_out_array_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_and_u32);
            printf("index: %d, expect: %d, actual: %d\n", i,
                   u32_out_array_ref[i], u32_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_and_u32);

    BENCH_START(riscv_and_u8);
    riscv_and_u8(u8_a_array, u8_b_array, u8_out_array, ARRAY_SIZE);
    BENCH_END(riscv_and_u8);
    ref_and_u8(u8_a_array, u8_b_array, u8_out_array_ref, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(u8_out_array[i] - u8_out_array_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_and_u8);
            printf("index: %d, expect: %d, actual: %d\n", i,
                   u8_out_array_ref[i], u8_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_and_u8);
#endif /*defined AND || defined ENABLE_ALL*/

#if defined OR || defined ENABLE_ALL
    BENCH_START(riscv_or_u16);
    riscv_or_u16(u16_a_array, u16_b_array, u16_out_array, ARRAY_SIZE);
    BENCH_END(riscv_or_u16);
    ref_or_u16(u16_a_array, u16_b_array, u16_out_array_ref, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(u16_out_array[i] - u16_out_array_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_or_u16);
            printf("index: %d, expect: %d, actual: %d\n", i,
                   u16_out_array_ref[i], u16_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_or_u16);

    BENCH_START(riscv_or_u32);
    riscv_or_u32(u32_a_array, u32_b_array, u32_out_array, ARRAY_SIZE);
    BENCH_END(riscv_or_u32);
    ref_or_u32(u32_a_array, u32_b_array, u32_out_array_ref, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(u32_out_array[i] - u32_out_array_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_or_u32);
            printf("index: %d, expect: %d, actual: %d\n", i,
                   u32_out_array_ref[i], u32_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_or_u32);

    BENCH_START(riscv_or_u8);
    riscv_or_u8(u8_a_array, u8_b_array, u8_out_array, ARRAY_SIZE);
    BENCH_END(riscv_or_u8);
    ref_or_u8(u8_a_array, u8_b_array, u8_out_array_ref, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(u8_out_array[i] - u8_out_array_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_or_u8);
            printf("index: %d, expect: %d, actual: %d\n", i,
                   u8_out_array_ref[i], u8_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_or_u8);
#endif /*defined OR || defined ENABLE_ALL*/

#if defined NOT || defined ENABLE_ALL
    BENCH_START(riscv_not_u16);
    riscv_not_u16(u16_a_array, u16_out_array, ARRAY_SIZE);
    BENCH_END(riscv_not_u16);
    ref_not_u16(u16_a_array, u16_out_array_ref, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(u16_out_array[i] - u16_out_array_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_not_u16);
            printf("index: %d, expect: %d, actual: %d\n", i,
                   u16_out_array_ref[i], u16_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_not_u16);

    BENCH_START(riscv_not_u32);
    riscv_not_u32(u32_a_array, u32_out_array, ARRAY_SIZE);
    BENCH_END(riscv_not_u32);
    ref_not_u32(u32_a_array, u32_out_array_ref, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(u32_out_array[i] - u32_out_array_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_not_u32);
            printf("index: %d, expect: %d, actual: %d\n", i,
                   u32_out_array_ref[i], u32_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_not_u32);

    BENCH_START(riscv_not_u8);
    riscv_not_u8(u8_a_array, u8_out_array, ARRAY_SIZE);
    BENCH_END(riscv_not_u8);
    ref_not_u8(u8_a_array, u8_out_array_ref, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(u8_out_array[i] - u8_out_array_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_not_u8);
            printf("index: %d, expect: %d, actual: %d\n", i,
                   u8_out_array_ref[i], u8_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_not_u8);
#endif /*defined NOT || defined ENABLE_ALL*/

#if defined XOR || defined ENABLE_ALL
    BENCH_START(riscv_xor_u16);
    riscv_xor_u16(u16_a_array, u16_b_array, u16_out_array, ARRAY_SIZE);
    BENCH_END(riscv_xor_u16);
    ref_xor_u16(u16_a_array, u16_b_array, u16_out_array_ref, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(u16_out_array[i] - u16_out_array_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_xor_u16);
            printf("index: %d, expect: %d, actual: %d\n", i,
                   u16_out_array_ref[i], u16_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_xor_u16);

    BENCH_START(riscv_xor_u32);
    riscv_xor_u32(u32_a_array, u32_b_array, u32_out_array, ARRAY_SIZE);
    BENCH_END(riscv_xor_u32);
    ref_xor_u32(u32_a_array, u32_b_array, u32_out_array_ref, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(u32_out_array[i] - u32_out_array_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_xor_u32);
            printf("index: %d, expect: %d, actual: %d\n", i,
                   u32_out_array_ref[i], u32_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_xor_u32);

    BENCH_START(riscv_xor_u8);
    riscv_xor_u8(u8_a_array, u8_b_array, u8_out_array, ARRAY_SIZE);
    BENCH_END(riscv_xor_u8);
    ref_xor_u8(u8_a_array, u8_b_array, u8_out_array_ref, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(u8_out_array[i] - u8_out_array_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_xor_u8);
            printf("index: %d, expect: %d, actual: %d\n", i,
                   u8_out_array_ref[i], u8_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_xor_u8);
#endif /*defined XOR || defined ENABLE_ALL*/
    BENCH_FINISH;
    
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
};
