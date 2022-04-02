//
// Created by lujun on 19-6-28.DELTAF32
//

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

#define ENABLE_ALL
#define F32
#define Q31
#define Q15
#define Q7

int test_flag_error = 0;

int main()
{
    float32_t f32_out[ARRAY_SIZE], f32_out_ref[ARRAY_SIZE];

    BENCH_INIT;
    // conjugate
    BENCH_START(riscv_quaternion_conjugate_f32);
    riscv_quaternion_conjugate_f32(f32_array, f32_array_opt, ARRAY_SIZE/4);
    BENCH_END(riscv_quaternion_conjugate_f32);
    ref_quaternion_conjugate_f32(f32_array, f32_array_ref, ARRAY_SIZE/4);
    for(int i = 0;i<ARRAY_SIZE/4;i++)
        if (abs(f32_array_opt[i] - f32_array_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_quaternion_conjugate_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_array_ref[i], f32_array_opt[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_quaternion_conjugate_f32);

    // inverse
    BENCH_START(riscv_quaternion_inverse_f32);
    riscv_quaternion_inverse_f32(f32_array, f32_array_opt, ARRAY_SIZE/4);
    BENCH_END(riscv_quaternion_inverse_f32);
    ref_quaternion_inverse_f32(f32_array, f32_array_ref, ARRAY_SIZE/4);
    for(int i = 0;i<ARRAY_SIZE/4;i++)
        if (abs(f32_array_opt[i] - f32_array_ref[i])>3) {
            BENCH_ERROR(riscv_quaternion_inverse_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_array_ref[i], f32_array_opt[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_quaternion_inverse_f32);

    // norm
    BENCH_START(riscv_quaternion_norm_f32);
    riscv_quaternion_norm_f32(f32_array, f32_array_opt, ARRAY_SIZE/4);
    BENCH_END(riscv_quaternion_norm_f32);
    ref_quaternion_norm_f32(f32_array, f32_array_ref, ARRAY_SIZE/4);
    for(int i = 0;i<ARRAY_SIZE/4;i++)
        if (abs(f32_array_opt[i] - f32_array_ref[i]) > 2) {
            BENCH_ERROR(riscv_quaternion_norm_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_array_ref[i], f32_array_opt[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_quaternion_norm_f32);

    // normalize
    BENCH_START(riscv_quaternion_normalize_f32);
    riscv_quaternion_normalize_f32(f32_array, f32_array_opt, ARRAY_SIZE/4);
    BENCH_END(riscv_quaternion_normalize_f32);
    ref_quaternion_normalize_f32(f32_array, f32_array_ref, ARRAY_SIZE/4);
    for(int i = 0;i<ARRAY_SIZE/4;i++)
        if (abs(f32_array_opt[i] - f32_array_ref[i]) > 3) {
            BENCH_ERROR(riscv_quaternion_normalize_f32);
            printf("index: %d, expect: %x, actual: %x\n", i, f32_array_ref[i], f32_array_opt[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_quaternion_normalize_f32);

    // product
    BENCH_START(riscv_quaternion_product_f32);
    riscv_quaternion_product_f32(f32_array, f32_b_array, f32_out, ARRAY_SIZE/4);
    BENCH_END(riscv_quaternion_product_f32);
    ref_quaternion_product_f32(f32_array, f32_b_array, f32_out_ref, ARRAY_SIZE/4);
    for(int i = 0;i<ARRAY_SIZE/4;i++)
        if (abs(f32_out[i] - f32_out_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_quaternion_product_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_out_ref[i], f32_out[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_quaternion_product_f32);

    // product_single
    BENCH_START(riscv_quaternion_product_single_f32);
    riscv_quaternion_product_single_f32(f32_array, f32_array_opt, f32_out);
    BENCH_END(riscv_quaternion_product_single_f32);
    ref_quaternion_product_single_f32(f32_array, f32_array_ref, f32_out_ref);
    for(int i = 0;i<4;i++)
        if (abs(f32_out_ref[i] - f32_out[i]) > DELTAF32) {
            BENCH_ERROR(riscv_quaternion_product_single_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_out_ref[i], f32_out[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_quaternion_product_single_f32);

    // quaternion2rotation
    BENCH_START(riscv_quaternion2rotation_f32);
    riscv_quaternion2rotation_f32(f32_array, f32_out, ARRAY_SIZE/9);
    BENCH_END(riscv_quaternion2rotation_f32);
    ref_quaternion2rotation_f32(f32_array, f32_out_ref, ARRAY_SIZE/9);
    for(int i = 0;i<(ARRAY_SIZE/9*9);i++)
        if (abs(f32_out[i] - f32_out_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_quaternion2rotation_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_out_ref[i], f32_out[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_quaternion2rotation_f32);

    // rotation2quaternion
    BENCH_START(riscv_rotation2quaternion_f32);
    riscv_rotation2quaternion_f32(f32_array, f32_out, ARRAY_SIZE/9);
    BENCH_END(riscv_rotation2quaternion_f32);
    ref_rotation2quaternion_f32(f32_array, f32_out_ref, ARRAY_SIZE/9);
    for(int i = 0;i<ARRAY_SIZE/9;i++)
        if (abs(f32_out[i] - f32_out_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_rotation2quaternion_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_out_ref[i], f32_out[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_rotation2quaternion_f32);

    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
