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
    int i;
    BENCH_INIT;
    riscv_sort_instance_f32 S_sort;
    riscv_sort_init_f32(&S_sort, RISCV_SORT_BUBBLE, RISCV_SORT_ASCENDING);
    BENCH_START(riscv_sort_f32);
    riscv_sort_f32(&S_sort, f32_a_array, f32_out_array, ARRAY_SIZE);
    BENCH_END(riscv_sort_f32);
    for(i=0;i<ARRAY_SIZE;i++)
    if (fabs(f32_out_array[i] - f32_out_sort_array_ref[i]) > DELTAF32) {
        BENCH_ERROR(riscv_sort_f32);
        printf("value expect: %f, actual: %f\n",
              f32_out_sort_array_ref[i], f32_out_array[i]);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_sort_f32);

    riscv_merge_sort_instance_f32 S_merge_sort;
    riscv_merge_sort_init_f32(&S_merge_sort, RISCV_SORT_ASCENDING, f32_temp_array);
    BENCH_START(riscv_merge_sort_f32);
    riscv_merge_sort_f32(&S_merge_sort, f32_a_array, f32_out_array, ARRAY_SIZE);
    BENCH_END(riscv_merge_sort_f32);
    for(i=0;i<ARRAY_SIZE;i++)
    if (fabs(f32_out_array[i] - f32_out_sort_array_ref[i]) > DELTAF32) {
        BENCH_ERROR(riscv_merge_sort_f32);
        printf("value expect: %f, actual: %f\n",
              f32_out_sort_array_ref[i], f32_out_array[i]);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_merge_sort_f32);

//simulate y = sin(x)
    riscv_spline_instance_f32 S_spline;
    BENCH_START(riscv_spline_init_f32);
    riscv_spline_init_f32(&S_spline, RISCV_SPLINE_NATURAL, f32_x_array, f32_y_array, FUNC_SIZE, f32_spline_coef_array, f32_spline_temp_array);
    BENCH_END(riscv_spline_init_f32);
    BENCH_START(riscv_spline_f32);
    riscv_spline_f32(&S_spline, &f32_test_x_array, f32_out_array, SPLINE_FUNC_SIZE);
    BENCH_END(riscv_spline_f32);
    for(i=0;i<SPLINE_FUNC_SIZE;i++)
    if (fabs(f32_out_array[i] - f32_out_spline_array_ref[i]) > DELTAF32) {
        BENCH_ERROR(riscv_spline_f32);
        printf("index: %d, value expect: %f, actual: %f\n", i,
              f32_out_spline_array_ref[i], f32_out_array[i]);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_spline_f32);

    BENCH_START(riscv_barycenter_f32);
    riscv_barycenter_f32(&f32_barycenter_array, f32_barycenter_weights_array, f32_out_array, VEC_NUM, DIMENSION);
    BENCH_END(riscv_barycenter_f32);
    for(i=0;i<DIMENSION;i++)
    if (fabs(f32_out_array[i] - f32_out_barycenter_array_ref[i]) > DELTAF32) {
        BENCH_ERROR(riscv_barycenter_f32);
        printf("value expect: %f, actual: %f\n",
              f32_out_barycenter_array_ref[i], f32_out_array[i]);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_barycenter_f32);

    BENCH_START(riscv_weighted_sum_f32);
    f32_weighted_output = riscv_weighted_sum_f32(&f32_weighted_sum_array, f32_weighted_array, WEIGHT_NUM);
    BENCH_END(riscv_weighted_sum_f32);
    for(i=0;i<WEIGHT_NUM;i++)
    if (fabs(f32_weighted_output - f32_weighted_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_weighted_sum_f32);
        printf("value expect: %f, actual: %f\n",
              f32_weighted_output_ref, f32_weighted_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_weighted_sum_f32);

    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
