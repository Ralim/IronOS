//
// Created by lujun on 19-6-28.
//
// This contains f32, q31 and q15 versions of matrix
// each one has it's own function.
// All function can be found in main function.
// If you don't want to use it, then comment it.
#include "riscv_math.h"
#include "array.h"
#include <stdint.h>
#include <stdlib.h>
#include "../common.h"

#include "../HelperFunctions/math_helper.c"
#include "../HelperFunctions/ref_helper.c"

#include <stdio.h>
#define DELTAF32 (0.05f)
#define DELTAQ31 (63)
#define DELTAQ15 (1)
#define DELTAQ7 (1)

int test_flag_error = 0;

int DSP_matrix_q31(void)
{
    riscv_matrix_instance_q31 q31_A;
    riscv_matrix_instance_q31 q31_B;
    riscv_matrix_instance_q31 q31_ref;
    riscv_matrix_instance_q31 q31_des;
    int i;
    riscv_mat_init_q31(&q31_A, ROWS, COLUMNS, (q31_t *)q31_a_array);
    riscv_mat_init_q31(&q31_B, ROWS, COLUMNS, (q31_t *)q31_b_array);
    riscv_mat_init_q31(&q31_des, ROWS, COLUMNS, q31_output);
    riscv_mat_init_q31(&q31_ref, ROWS, COLUMNS, q31_output_ref);

    // ****************   q31   *********************
    // add
    BENCH_START(riscv_mat_add_q31);
    riscv_mat_add_q31(&q31_A, &q31_B, &q31_des);
    BENCH_END(riscv_mat_add_q31);
    ref_mat_add_q31(&q31_A, &q31_B, &q31_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (labs(q31_output[i] - q31_output_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_mat_add_q31);
            printf("index: %d,expect: %x, actual: %x\n", i, q31_output_ref[i],
                   q31_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_add_q31);
    // mult
    BENCH_START(riscv_mat_mult_q31);
    riscv_mat_mult_q31(&q31_A, &q31_B, &q31_des);
    BENCH_END(riscv_mat_mult_q31);
    ref_mat_mult_q31(&q31_A, &q31_B, &q31_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (labs(q31_output[i] - q31_output_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_mat_mult_q31);
            printf("index: %d,expect: %x, actual: %x\n", i, q31_output_ref[i],
                   q31_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_mult_q31);
    // sub
    BENCH_START(riscv_mat_sub_q31);
    riscv_mat_sub_q31(&q31_A, &q31_B, &q31_des);
    BENCH_END(riscv_mat_sub_q31);
    ref_mat_sub_q31(&q31_A, &q31_B, &q31_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (labs(q31_output[i] - q31_output_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_mat_sub_q31);
            printf("index: %d,expect: %x, actual: %x\n", i, q31_output_ref[i],
                   q31_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_sub_q31);
    // trans
    BENCH_START(riscv_mat_trans_q31);
    riscv_mat_trans_q31(&q31_A, &q31_des);
    BENCH_END(riscv_mat_trans_q31);
    ref_mat_trans_q31(&q31_A, &q31_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (labs(q31_output[i] - q31_output_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_mat_trans_q31);
            printf("index: %d,expect: %x, actual: %x\n", i, q31_output_ref[i],
                   q31_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_trans_q31);
    // scale
    BENCH_START(riscv_mat_scale_q31);
    riscv_mat_scale_q31(&q31_A, 8, 2, &q31_des);
    BENCH_END(riscv_mat_scale_q31);
    ref_mat_scale_q31(&q31_A, 8, 2, &q31_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (labs(q31_output[i] - q31_output_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_mat_scale_q31);
            printf("index: %d,expect: %x, actual: %x\n", i, q31_output_ref[i],
                   q31_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_scale_q31);
    // cmplx_mult
    riscv_mat_init_q31(&q31_A, 32, 32, (q31_t *)q31_a_array);
    riscv_mat_init_q31(&q31_B, 32, 32, (q31_t *)q31_b_array);
    riscv_mat_init_q31(&q31_des, 32, 32, q31_output);
    riscv_mat_init_q31(&q31_ref, 32, 32, q31_output_ref);
    BENCH_START(riscv_mat_cmplx_mult_q31);
    riscv_mat_cmplx_mult_q31(&q31_A, &q31_B, &q31_des);
    BENCH_END(riscv_mat_cmplx_mult_q31);
    ref_mat_cmplx_mult_q31(&q31_A, &q31_B, &q31_ref);
    for (int i = 0; i < (32 * 32); i++)
        if (labs(q31_output[i] - q31_output_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_mat_cmplx_mult_q31);
            printf("index: %d,expect: %x, actual: %x\n", i, q31_output_ref[i],
                   q31_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_cmplx_mult_q31);
    // mult_fast
    BENCH_START(riscv_mat_mult_fast_q31);
    riscv_mat_mult_fast_q31(&q31_A, &q31_B, &q31_des);
    BENCH_END(riscv_mat_mult_fast_q31);
    ref_mat_mult_fast_q31(&q31_A, &q31_B, &q31_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (labs(q31_output[i] - q31_output_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_mat_mult_fast_q31);
            printf("index: %d,expect: %x, actual: %x\n", i, q31_output_ref[i],
                   q31_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_mult_fast_q31);
    printf("all tests are passed,well done!\n");
}

int main()
{
    BENCH_INIT;
    DSP_matrix_q31();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
