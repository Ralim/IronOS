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

int DSP_matrix_q15(void)
{
    riscv_matrix_instance_q15 q15_A;
    riscv_matrix_instance_q15 q15_B;
    riscv_matrix_instance_q15 q15_ref;
    riscv_matrix_instance_q15 q15_des;
    int i;

    // ****************   f32   *********************

    riscv_mat_init_q15(&q15_A, ROWS, COLUMNS, (q15_t *)q15_a_array);
    riscv_mat_init_q15(&q15_B, ROWS, COLUMNS, (q15_t *)q15_b_array);
    riscv_mat_init_q15(&q15_des, ROWS, COLUMNS, q15_output);
    riscv_mat_init_q15(&q15_ref, ROWS, COLUMNS, q15_output_ref);

    // ****************   q15   *********************
    // add
    BENCH_START(riscv_mat_add_q15);
    riscv_mat_add_q15(&q15_A, &q15_B, &q15_des);
    BENCH_END(riscv_mat_add_q15);
    ref_mat_add_q15(&q15_A, &q15_B, &q15_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (abs(q15_output[i] - q15_output_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_mat_add_q15);
            printf("index: %d,expect: %x, actual: %x\n", i, q15_output_ref[i],
                   q15_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_add_q15);
    // mult
    BENCH_START(riscv_mat_mult_q15);
    riscv_mat_mult_q15(&q15_A, &q15_B, &q15_des, q15_output_back);
    BENCH_END(riscv_mat_mult_q15);
    ref_mat_mult_q15(&q15_A, &q15_B, &q15_ref);
    for (int i = 0; i < 100; i++)
        if (abs(q15_output[i] - q15_output_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_mat_mult_q15);
            printf("index: %d,expect: %x, actual: %x\t", i, q15_output_ref[i],
                   q15_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_mult_q15);
    // sub
    BENCH_START(riscv_mat_sub_q15);
    riscv_mat_sub_q15(&q15_A, &q15_B, &q15_des);
    BENCH_END(riscv_mat_sub_q15);
    ref_mat_sub_q15(&q15_A, &q15_B, &q15_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (abs(q15_output[i] - q15_output_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_mat_sub_q15);
            printf("index: %d,expect: %x, actual: %x\n", i, q15_output_ref[i],
                   q15_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_sub_q15);
    // trans
    BENCH_START(riscv_mat_trans_q15);
    riscv_mat_trans_q15(&q15_A, &q15_des);
    BENCH_END(riscv_mat_trans_q15);
    ref_mat_trans_q15(&q15_A, &q15_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (abs(q15_output[i] - q15_output_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_mat_trans_q15);
            printf("index: %d,expect: %x, actual: %x\n", i, q15_output_ref[i],
                   q15_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_trans_q15);
    // scale
    BENCH_START(riscv_mat_scale_q15);
    riscv_mat_scale_q15(&q15_A, 8, 2, &q15_des);
    BENCH_END(riscv_mat_scale_q15);
    ref_mat_scale_q15(&q15_A, 8, 2, &q15_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (abs(q15_output[i] - q15_output_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_mat_scale_q15);
            printf("index: %d,expect: %x, actual: %x\n", i, q15_output_ref[i],
                   q15_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_scale_q15);
    // cmplx_mult
    riscv_mat_init_q15(&q15_A, 32, 32, (q15_t *)q15_a_array);
    riscv_mat_init_q15(&q15_B, 32, 32, (q15_t *)q15_b_array);
    riscv_mat_init_q15(&q15_des, 32, 32, q15_output);
    riscv_mat_init_q15(&q15_ref, 32, 32, q15_output_ref);
    BENCH_START(riscv_mat_cmplx_mult_q15);
    riscv_mat_cmplx_mult_q15(&q15_A, &q15_B, &q15_des, q15_output_back);
    BENCH_END(riscv_mat_cmplx_mult_q15);
    ref_mat_cmplx_mult_q15(&q15_A, &q15_B, &q15_ref);
    for (int i = 0; i < (32 * 32); i++)
        if (abs(q15_output[i] - q15_output_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_mat_cmplx_mult_q15);
            printf("index: %d,expect: %x, actual: %x\n", i, q15_output_ref[i],
                   q15_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_cmplx_mult_q15);
    // mult_fast
    BENCH_START(riscv_mat_mult_fast_q15);
    riscv_mat_mult_fast_q15(&q15_A, &q15_B, &q15_des, q15_output_back);
    BENCH_END(riscv_mat_mult_fast_q15);
    ref_mat_mult_fast_q15(&q15_A, &q15_B, &q15_ref);
    for (int i = 0; i < (10 * 10); i++)
        if (abs(q15_output[i] - q15_output_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_mat_mult_fast_q15);
            printf("index: %d,expect: %x, actual: %x\n", i, q15_output_ref[i],
                   q15_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_mult_fast_q15);
    printf("all tests are passed,well done!\n");
}

int main()
{
    BENCH_INIT;
    DSP_matrix_q15();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
