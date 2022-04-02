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
#define ABS
#define ADD
#define DOT_PROD
#define MULT

int test_flag_error = 0;

int main()
{
    int i;
    float32_t f32_out, f32_out_ref;
    q7_t q7_out, q7_out_ref;
    q15_t q15_out, q15_out_ref;
    q31_t q31_out, q31_out_ref;
    q63_t q63_out, q63_out_ref;
    BENCH_INIT;
#if defined ABS || defined ENABLE_ALL
    // ***********************************   abs
    // ********************************** riscv_abs_f32.c
    BENCH_START(riscv_abs_f32);
    riscv_abs_f32(f32_a_array, f32_out_array, ARRAY_SIZE);
    BENCH_END(riscv_abs_f32);
    ref_abs_f32(f32_a_array, f32_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (fabs(f32_out_array[i] - f32_out_ref_array[i]) > DELTAF32) {
            BENCH_ERROR(riscv_abs_f32);
            printf("index: %d,expect: %f, actual: %f\n", i,
                   f32_out_ref_array[i], f32_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_abs_f32);

    // riscv_abs_q7.c
    BENCH_START(riscv_abs_q7);
    riscv_abs_q7(q7_a_array, q7_out_array, ARRAY_SIZE);
    BENCH_END(riscv_abs_q7);
    ref_abs_q7(q7_a_array, q7_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q7_out_array[i] - q7_out_ref_array[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_abs_q7);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q7_out_ref_array[i], q7_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_abs_q7);

    // riscv_abs_q15.c
    BENCH_START(riscv_abs_q15);
    riscv_abs_q15(q15_a_array, q15_out_array, ARRAY_SIZE);
    BENCH_END(riscv_abs_q15);
    ref_abs_q15(q15_a_array, q15_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q15_out_array[i] - q15_out_ref_array[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_abs_q15);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q15_out_ref_array[i], q15_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_abs_q15);

    // riscv_abs_q31.c
    BENCH_START(riscv_abs_q31);
    riscv_abs_q31(q31_a_array, q31_out_array, ARRAY_SIZE);
    BENCH_END(riscv_abs_q31);
    ref_abs_q31(q31_a_array, q31_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (labs(q31_out_array[i] - q31_out_ref_array[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_abs_q31);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q31_out_ref_array[i], q31_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_abs_q31);
#endif
#if defined ADD || defined ENABLE_ALL
    // *************************************   add
    // **************************************** riscv_add_f32.c
    BENCH_START(riscv_add_f32);
    riscv_add_f32(f32_a_array, f32_b_array, f32_out_array, ARRAY_SIZE);
    BENCH_END(riscv_add_f32);
    ref_add_f32(f32_a_array, f32_b_array, f32_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (fabs(f32_out_array[i] - f32_out_ref_array[i]) > DELTAF32) {
            BENCH_ERROR(riscv_add_f32);
            printf("index: %d, expect: %f, actual: %f\n", i,
                   f32_out_ref_array[i], f32_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_add_f32);

    // riscv_add_q7.c
    BENCH_START(riscv_add_q7);
    riscv_add_q7(q7_a_array, q7_b_array, q7_out_array, ARRAY_SIZE);
    BENCH_END(riscv_add_q7);
    ref_add_q7(q7_a_array, q7_b_array, q7_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q7_out_array[i] - q7_out_ref_array[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_add_q7);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q7_out_ref_array[i], q7_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_add_q7);

    // riscv_add_q15.c
    BENCH_START(riscv_add_q15);
    riscv_add_q15(q15_a_array, q15_b_array, q15_out_array, ARRAY_SIZE);
    BENCH_END(riscv_add_q15);
    ref_add_q15(q15_a_array, q15_b_array, q15_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q15_out_array[i] - q15_out_ref_array[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_add_q15);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q15_out_ref_array[i], q15_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_add_q15);

    // riscv_add_q31.c
    BENCH_START(riscv_add_q31);
    riscv_add_q31(q31_a_array, q31_b_array, q31_out_array, ARRAY_SIZE);
    BENCH_END(riscv_add_q31);
    ref_add_q31(q31_a_array, q31_b_array, q31_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (labs(q31_out_array[i] - q31_out_ref_array[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_add_q31);
            printf("index: %d expect: %x, actual: %x\n", i,
                   q31_out_ref_array[i], q31_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_add_q31);
#endif
#if defined DOT_PROD || defined ENABLE_ALL
    //*************************************   dot prod
    //********************************************
    // f32_dot
    BENCH_START(riscv_dot_prod_f32);
    riscv_dot_prod_f32(f32_a_array, f32_b_array, ARRAY_SIZE, &f32_out);
    BENCH_END(riscv_dot_prod_f32);
    ref_dot_prod_f32(f32_a_array, f32_b_array, ARRAY_SIZE, &f32_out_ref);
    if (fabs(f32_out - f32_out_ref) > 0.5) {
        BENCH_ERROR(riscv_dot_prod_f32);
        printf("index:%d, expect: %f, actual: %f\n", i, f32_out_ref, f32_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_dot_prod_f32);

    // q7_dot
    BENCH_START(riscv_dot_prod_q7);
    riscv_dot_prod_q7(q7_a_array, q7_b_array, ARRAY_SIZE, &q31_out);
    BENCH_END(riscv_dot_prod_q7);
    ref_dot_prod_q7(q7_a_array, q7_b_array, ARRAY_SIZE, &q31_out_ref);
    if (abs(q31_out - q31_out_ref) > DELTAQ7) {
        BENCH_ERROR(riscv_dot_prod_q7);
        printf("index: %d, expect: %x, actual: %x\n", i, q31_out_ref, q31_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_dot_prod_q7);

    // q15_dot
    BENCH_START(riscv_dot_prod_q15);
    riscv_dot_prod_q15(q15_a_array, q15_b_array, ARRAY_SIZE, &q63_out);
    BENCH_END(riscv_dot_prod_q15);
    ref_dot_prod_q15(q15_a_array, q15_b_array, ARRAY_SIZE, &q63_out_ref);
    if (abs(q63_out - q63_out_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_dot_prod_q15);
        printf("index: %d, expect: %x, actual: %x\n", i, q63_out_ref, q63_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_dot_prod_q15);

    // q31_dot
    BENCH_START(riscv_dot_prod_q31);
    riscv_dot_prod_q31(q31_a_array, q31_b_array, ARRAY_SIZE, &q63_out);
    BENCH_END(riscv_dot_prod_q31);
    ref_dot_prod_q31(q31_a_array, q31_b_array, ARRAY_SIZE, &q63_out_ref);
    if (abs(q63_out - q63_out_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_dot_prod_q31);
        printf("index: %d, expect: %x, actual: %x\n", i, q63_out_ref, q63_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_dot_prod_q31);
#endif
#if defined MULT || defined ENABLE_ALL
    // ********************************   mult ********************************
    // f32_mult
    BENCH_START(riscv_mult_f32);
    riscv_mult_f32(f32_a_array, f32_b_array, f32_out_array, ARRAY_SIZE);
    BENCH_END(riscv_mult_f32);
    ref_mult_f32(f32_a_array, f32_b_array, f32_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (fabs(f32_out_array[i] - f32_out_ref_array[i]) > 0.5) {
            BENCH_ERROR(riscv_mult_f32);
            printf("index:%d, expect: %f, actual: %f\n", i,
                   f32_out_ref_array[i], f32_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mult_f32);

    // riscv_mult_q7.c
    BENCH_START(riscv_mult_q7);
    riscv_mult_q7(q7_a_array, q7_b_array, q7_out_array, ARRAY_SIZE);
    BENCH_END(riscv_mult_q7);
    ref_mult_q7(q7_a_array, q7_b_array, q7_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q7_out_array[i] - q7_out_ref_array[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_mult_q7);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q7_out_ref_array[i], q7_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mult_q7);

    // riscv_mult_q15.c
    BENCH_START(riscv_mult_q15);
    riscv_mult_q15(q15_a_array, q15_b_array, q15_out_array, ARRAY_SIZE);
    BENCH_END(riscv_mult_q15);
    ref_mult_q15(q15_a_array, q15_b_array, q15_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q15_out_array[i] - q15_out_ref_array[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_mult_q15);
            printf("expect: %x, actual: %x\n", q15_out_ref_array[i],
                   q15_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mult_q15);

    // riscv_mult_q31.c
    BENCH_START(riscv_mult_q31);
    riscv_mult_q31(q31_a_array, q31_b_array, q31_out_array, ARRAY_SIZE);
    BENCH_END(riscv_mult_q31);
    ref_mult_q31(q31_a_array, q31_b_array, q31_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (labs(q31_out_array[i] - q31_out_ref_array[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_mult_q31);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q31_out_ref_array[i], q31_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mult_q31);
#endif
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
