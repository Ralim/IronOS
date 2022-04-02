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
#include "../common.h"

#include "../HelperFunctions/math_helper.c"
#include "../HelperFunctions/ref_helper.c"

#include <stdio.h>
#define DELTAF32 (0.05f)
#define DELTAF64 (0.05f)
#define DELTAQ31 (63)
#define DELTAQ15 (1)
#define DELTAQ7 (1)

int test_flag_error = 0;







int DSP_matrix_f32(void)
{
    riscv_matrix_instance_f32 f32_A;
    riscv_matrix_instance_f32 f32_B;
    riscv_matrix_instance_f32 f32_ref;
    riscv_matrix_instance_f32 f32_des;
    riscv_matrix_instance_f32 f32_back;
    int i;
    riscv_mat_init_f32(&f32_A, ROWS, COLUMNS, (float32_t *)f32_a_array);
    riscv_mat_init_f32(&f32_B, ROWS, COLUMNS, (float32_t *)f32_b_array);
    riscv_mat_init_f32(&f32_des, ROWS, COLUMNS, f32_output);
    riscv_mat_init_f32(&f32_back, ROWS, COLUMNS, f32_output_back);
    riscv_mat_init_f32(&f32_ref, ROWS, COLUMNS, f32_output_ref);

    BENCH_START(riscv_mat_add_f32);
    riscv_mat_add_f32(&f32_A, &f32_B, &f32_des);
    BENCH_END(riscv_mat_add_f32);
    ref_mat_add_f32(&f32_A, &f32_B, &f32_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (fabs(f32_output[i] - f32_output_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_mat_add_f32);
            printf("index: %d,expect: %f, actual: %f\n", i, f32_output_ref[i],
                   f32_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_add_f32);
    // inverse
    BENCH_START(riscv_mat_inverse_f32);
    riscv_mat_inverse_f32(&f32_A, &f32_des);
    BENCH_END(riscv_mat_inverse_f32);
    // ref_mat_inverse_f32(&f32_A, &f32_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (fabs(f32_output[i] - f32_inv_output_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_mat_inverse_f32);
            printf("index: %d,expect: %f, actual: %f\n", i,
                   f32_inv_output_ref[i], f32_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_inverse_f32);
    // mult
    BENCH_START(riscv_mat_mult_f32);
    riscv_mat_mult_f32(&f32_A, &f32_B, &f32_des);
    BENCH_END(riscv_mat_mult_f32);
    ref_mat_mult_f32(&f32_A, &f32_B, &f32_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (fabs(f32_output[i] - f32_output_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_mat_mult_f32);
            printf("index: %d,expect: %f, actual: %f\n", i, f32_output_ref[i],
                   f32_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_mult_f32);
    // scale
    BENCH_START(riscv_mat_scale_f32);
    riscv_mat_scale_f32(&f32_A, 8.73f, &f32_des);
    BENCH_END(riscv_mat_scale_f32);
    ref_mat_scale_f32(&f32_A, 8.73f, &f32_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (fabs(f32_output[i] - f32_output_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_mat_scale_f32);
            printf("index: %d,expect: %f, actual: %f\n", i, f32_output_ref[i],
                   f32_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_scale_f32);
    // sub
    BENCH_START(riscv_mat_sub_f32);
    riscv_mat_sub_f32(&f32_A, &f32_B, &f32_des);
    BENCH_END(riscv_mat_sub_f32);
    ref_mat_sub_f32(&f32_A, &f32_B, &f32_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (fabs(f32_output[i] - f32_output_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_mat_sub_f32);
            printf("index: %d,expect: %f, actual: %f\n", i, f32_output_ref[i],
                   f32_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_sub_f32);
    // trans
    BENCH_START(riscv_mat_trans_f32);
    riscv_mat_trans_f32(&f32_A, &f32_des);
    BENCH_END(riscv_mat_trans_f32);
    ref_mat_trans_f32(&f32_A, &f32_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (fabs(f32_output[i] - f32_output_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_mat_trans_f32);
            printf("index: %d,expect: %f, actual: %f\n", i, f32_output_ref[i],
                   f32_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_trans_f32);
    // cmplx_mult
    riscv_mat_init_f32(&f32_A, ROWS/2, COLUMNS/2, (float32_t *)f32_a_array);
    riscv_mat_init_f32(&f32_B, ROWS/2, COLUMNS/2, (float32_t *)f32_b_array);
    riscv_mat_init_f32(&f32_des, ROWS/2, COLUMNS/2, f32_output);
    riscv_mat_init_f32(&f32_ref, ROWS/2, COLUMNS/2, f32_output_ref);
    BENCH_START(riscv_mat_cmplx_mult_f32);
    riscv_mat_cmplx_mult_f32(&f32_A, &f32_B, &f32_des);
    BENCH_END(riscv_mat_cmplx_mult_f32);
    ref_mat_cmplx_mult_f32(&f32_A, &f32_B, &f32_ref);
    for (i = 0; i < (ROWS/2 * COLUMNS/2); i++)
        if (fabs(f32_output[i] - f32_output_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_mat_cmplx_mult_f32);
            printf("index: %d,expect: %f, actual: %f\n", i, f32_output_ref[i],
                   f32_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_cmplx_mult_f32);
    // cholesky
    riscv_mat_init_f32(&f32_A, ROWS, COLUMNS, (float32_t *)f32_a_array);
    riscv_mat_init_f32(&f32_des, ROWS, COLUMNS, f32_output);
    riscv_mat_init_f32(&f32_ref, ROWS, COLUMNS, f32_output_ref);

    BENCH_START(riscv_mat_cholesky_f32);
    riscv_mat_cholesky_f32(&f32_A, &f32_des);
    BENCH_END(riscv_mat_cholesky_f32);
    ref_mat_cholesky_f32(&f32_A, &f32_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (fabs(f32_output[i] - f32_output_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_mat_cholesky_f32);
            printf("index: %d,expect: %f, actual: %f\n", i, f32_output_ref[i],
                   f32_output[i]);
            test_flag_error = 1;
        }
    // if(a == RISCV_MATH_DECOMPOSITION_FAILURE) printf("!!!!!!!!!!!!!!!!!!!!!");
    BENCH_STATUS(riscv_mat_cholesky_f32);
    // solve upper triangular matrix
    // initialize upper triangular matrix by setting 0 to lower elements in matrix
    for(int i=0;i<ROWS;i++){
        for(int j=0;j<i;j++){
            f32_a_array[i*COLUMNS+j] = 0;
        }
    }
    riscv_mat_init_f32(&f32_B, ROWS, COLUMNS, (float32_t *)f32_b_array);

    BENCH_START(riscv_mat_solve_upper_triangular_f32);
    riscv_mat_solve_upper_triangular_f32(&f32_A, &f32_B, &f32_des);
    BENCH_END(riscv_mat_solve_upper_triangular_f32);
    ref_mat_solve_upper_triangular_f32(&f32_A, &f32_B, &f32_ref);
    for (int i = 0; i < (ROWS * 1); i++)
        if (fabs(f32_output[i] - f32_output_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_mat_solve_upper_triangular_f32);
            printf("index: %d,expect: %f, actual: %f\n", i, f32_output_ref[i],
                   f32_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_solve_upper_triangular_f32);

    // solve lower triangular matrix
    // initialize lower triangular matrix by setting 0 to upper elements in matrix
    for(int i=0;i<ROWS;i++){
        for(int j=COLUMNS-1;j>i;j--){
            f32_a_array[i*COLUMNS+j] = 0;
        }
    }
    BENCH_START(riscv_mat_solve_lower_triangular_f32);
    riscv_mat_solve_lower_triangular_f32(&f32_A, &f32_B, &f32_des);
    BENCH_END(riscv_mat_solve_lower_triangular_f32);
    ref_mat_solve_lower_triangular_f32(&f32_A, &f32_B, &f32_ref);
    for (int i = 0; i < (ROWS * 1); i++)
        if (fabs(f32_output[i] - f32_output_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_mat_solve_lower_triangular_f32);
            printf("index: %d,expect: %f, actual: %f\n", i, f32_output_ref[i],
                   f32_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_solve_lower_triangular_f32);

    // // Initialize Symmetric Matrices
    uint16_t pp[ROWS], pp_ref[ROWS];
    for(int i=0;i<ROWS;i++){
        for(int j=COLUMNS;j>i;j--){
            if(i==j) break;
            f32_a_array[i*COLUMNS+j] = f32_a_array[j*COLUMNS+i];
        }
    }
    riscv_mat_init_f32(&f32_B, ROWS, COLUMNS, (float32_t *)f32_b_array);
    BENCH_START(riscv_mat_ldlt_f32);
    riscv_mat_ldlt_f32(&f32_A, &f32_B, &f32_des, pp);
    BENCH_END(riscv_mat_ldlt_f32);
    ref_mat_ldlt_f32(&f32_A, &f32_back, &f32_ref, pp_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (fabs(f32_output[i] - f32_output_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_mat_ldlt_f32);
            printf("index: %d,expect: %f, actual: %f\n", i, f32_output_ref[i],
                   f32_output[i]);
            test_flag_error = 1;
        }
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (fabs(f32_b_array[i] - f32_output_back[i]) > DELTAF32) {
            BENCH_ERROR(riscv_mat_ldlt_f32);
            printf("index: %d,expect: %f, actual: %f\n", i, f32_output_back[i],
                   f32_b_array[i]);
            test_flag_error = 1;
        }
    for (int i = 0; i < ROWS; i++)
        if ((pp - pp_ref) > DELTAF32) {
                BENCH_ERROR(riscv_mat_ldlt_f32);
                printf("expect: %f, actual: %f\n", pp_ref, pp);
                test_flag_error = 1;
            }
    BENCH_STATUS(riscv_mat_ldlt_f32);

    printf("all tests are passed,well done!\n");
}

int DSP_matrix_f64(void)
{
    riscv_matrix_instance_f64 f64_A;
    riscv_matrix_instance_f64 f64_B;
    riscv_matrix_instance_f64 f64_back;
    riscv_matrix_instance_f64 f64_ref;
    riscv_matrix_instance_f64 f64_des;
    int i;
    riscv_mat_init_f64(&f64_A, ROWS, COLUMNS, (float64_t *)f64_a_array);
    riscv_mat_init_f64(&f64_B, ROWS, COLUMNS, (float64_t *)f64_b_array);
    riscv_mat_init_f64(&f64_des, ROWS, COLUMNS, f64_output);
    riscv_mat_init_f64(&f64_back, ROWS, COLUMNS, f64_output_back);
    riscv_mat_init_f64(&f64_ref, ROWS, COLUMNS, f64_output_ref);

    BENCH_START(riscv_mat_cholesky_f64);
    riscv_mat_cholesky_f64(&f64_A, &f64_des);
    BENCH_END(riscv_mat_cholesky_f64);
    ref_mat_cholesky_f64(&f64_A, &f64_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (fabs(f64_output[i] - f64_output_ref[i]) > DELTAF64) {
            BENCH_ERROR(riscv_mat_cholesky_f64);
            printf("index: %d,expect: %f, actual: %f\n", i, f64_output_ref[i],
                   f64_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_cholesky_f64);

    // solve upper triangular matrix
    // initialize upper triangular matrix by setting 0 to lower elements in matrix
    for(int i=0;i<ROWS;i++){
        for(int j=0;j<i;j++){
            f64_a_array[i*COLUMNS+j] = 0;
        }
    }
    riscv_mat_init_f64(&f64_B, ROWS, 1, (float64_t *)f64_b_array);

    BENCH_START(riscv_mat_solve_upper_triangular_f64);
    riscv_mat_solve_upper_triangular_f64(&f64_A, &f64_B, &f64_des);
    BENCH_END(riscv_mat_solve_upper_triangular_f64);
    ref_mat_solve_upper_triangular_f64(&f64_A, &f64_B, &f64_ref);
    for (int i = 0; i < (ROWS * 1); i++)
        if (fabs(f64_output[i] - f64_output_ref[i]) > DELTAF64) {
            BENCH_ERROR(riscv_mat_solve_upper_triangular_f64);
            printf("index: %d,expect: %f, actual: %f\n", i, f64_output_ref[i],
                   f64_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_solve_upper_triangular_f64);

    // solve lower triangular matrix
    // initialize lower triangular matrix by setting 0 to upper elements in matrix
    for(int i=0;i<ROWS;i++){
        for(int j=COLUMNS-1;j>i;j--){
            f64_a_array[i*COLUMNS+j] = 0;
        }
    }
    BENCH_START(riscv_mat_solve_lower_triangular_f64);
    riscv_mat_solve_lower_triangular_f64(&f64_A, &f64_B, &f64_des);
    BENCH_END(riscv_mat_solve_lower_triangular_f64);
    ref_mat_solve_lower_triangular_f64(&f64_A, &f64_B, &f64_ref);
    for (int i = 0; i < (ROWS * 1); i++)
        if (fabs(f64_output[i] - f64_output_ref[i]) > DELTAF64) {
            BENCH_ERROR(riscv_mat_solve_lower_triangular_f64);
            printf("index: %d,expect: %f, actual: %f\n", i, f64_output_ref[i],
                   f64_output[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_mat_solve_lower_triangular_f64);

    // Initialize Symmetric Matrices
    uint16_t pp[ROWS], pp_ref[ROWS];
    for(int i=0;i<ROWS;i++){
        for(int j=COLUMNS;j>i;j--){
            if(i==j) break;
            f64_a_array[i*COLUMNS+j] = f64_a_array[j*COLUMNS+i];
        }
    }
    riscv_mat_init_f64(&f64_B, ROWS, COLUMNS, (float64_t *)f64_b_array);
    BENCH_START(riscv_mat_ldlt_f64);
    riscv_mat_ldlt_f64(&f64_A, &f64_B, &f64_des, pp);
    BENCH_END(riscv_mat_ldlt_f64);
    ref_mat_ldlt_f64(&f64_A, &f64_back, &f64_ref, pp_ref);
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (fabs(f64_output[i] - f64_output_ref[i]) > DELTAF64) {
            BENCH_ERROR(riscv_mat_ldlt_f64);
            printf("index: %d,expect: %f, actual: %f\n", i, f64_output_ref[i],
                   f64_output[i]);
            test_flag_error = 1;
        }
    for (int i = 0; i < (ROWS * COLUMNS); i++)
        if (fabs(f64_b_array[i] - f64_output_back[i]) > DELTAF64) {
            BENCH_ERROR(riscv_mat_ldlt_f64);
            printf("index: %d,expect: %f, actual: %f\n", i, f64_output_back[i],
                   f64_b_array[i]);
            test_flag_error = 1;
        }
    for (int i = 0; i < ROWS; i++)
        if ((pp - pp_ref) > DELTAF64) {
                BENCH_ERROR(riscv_mat_ldlt_f64);
                printf("expect: %f, actual: %f\n", pp_ref, pp);
                test_flag_error = 1;
            }
    BENCH_STATUS(riscv_mat_ldlt_f64);

    printf("all tests are passed,well done!\n");
}


int main()
{
    BENCH_INIT;
    // DSP_matrix_f32();
    DSP_matrix_f64();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
