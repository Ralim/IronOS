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
#define DELTAF32 (1.0f)
#define DELTAF64 (1.0f)
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
    BENCH_INIT;
//entropy -Sum(p ln p)
    BENCH_START(riscv_entropy_f32);
    f32_out_array = riscv_entropy_f32(f32_a_array, ARRAY_SIZE);
    BENCH_END(riscv_entropy_f32);
    if (fabs(f32_out_array - f32_out_entropy_array_ref) > DELTAF32) {
        BENCH_ERROR(riscv_entropy_f32);
        printf("value expect: %f, actual: %f\n",
              f32_out_entropy_array_ref, f32_out_array);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_entropy_f32);

//entropy -Sum(p ln p)
    BENCH_START(riscv_entropy_f64);
    f64_out_array = riscv_entropy_f64(f64_a_array, ARRAY_SIZE);
    BENCH_END(riscv_entropy_f64);
    if (fabs(f64_out_array - f64_out_entropy_array_ref) > DELTAF64) {
        BENCH_ERROR(riscv_entropy_f64);
        printf("value expect: %f, actual: %f\n",
               f64_out_entropy_array_ref, f64_out_array);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_entropy_f64);

    BENCH_START(riscv_kullback_leibler_f32);
    f32_out_array = riscv_kullback_leibler_f32(f32_a_array, f32_b_array, ARRAY_SIZE);
    BENCH_END(riscv_kullback_leibler_f32);
    if (fabs(f32_out_array - f32_out_kl_array_ref) > DELTAF32) {
        BENCH_ERROR(riscv_kullback_leibler_f32);
        printf("value expect: %f, actual: %f\n",
               f32_out_kl_array_ref, f32_out_array);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_kullback_leibler_f32);

    BENCH_START(riscv_kullback_leibler_f64);
    f64_out_array = riscv_kullback_leibler_f64(f64_a_array, f64_b_array, ARRAY_SIZE);
    BENCH_END(riscv_kullback_leibler_f64);
    if (fabs(f64_out_array - f64_out_kl_array_ref) > DELTAF64) {
        BENCH_ERROR(riscv_kullback_leibler_f64);
        printf("value expect: %f, actual: %f\n",
               f64_out_kl_array_ref, f64_out_array);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_kullback_leibler_f64);

    BENCH_START(riscv_logsumexp_dot_prod_f32);
    f32_out_array = riscv_logsumexp_dot_prod_f32(f32_a_array, f32_b_array, ARRAY_SIZE, f32_temp_array);
    BENCH_END(riscv_logsumexp_dot_prod_f32);
    if (fabs(f32_out_array - f32_out_lsedp_array_ref) > DELTAF32) {
        BENCH_ERROR(riscv_logsumexp_dot_prod_f32);
        printf("value expect: %f, actual: %f\n",
               f32_out_lsedp_array_ref, f32_out_array);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_logsumexp_dot_prod_f32);

    BENCH_START(riscv_logsumexp_f32);
    f32_out_array = riscv_logsumexp_f32(f32_a_array, ARRAY_SIZE);
    BENCH_END(riscv_logsumexp_f32);
    if (fabs(f32_out_array - f32_out_lse_array_ref) > DELTAF32) {
        BENCH_ERROR(riscv_logsumexp_f32);
        printf("value expect: %f, actual: %f\n",
               f32_out_lse_array_ref, f32_out_array);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_logsumexp_f32);

    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
