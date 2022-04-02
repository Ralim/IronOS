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
    q63_t q63_out, q63_out_ref;
    float32_t f32_out, f32_out_ref;
    q31_t q31_out, q31_out_ref;
    q15_t q15_out, q15_out_ref;
    q7_t q7_out, q7_out_ref;
    uint32_t index, index_ref;

    BENCH_INIT;
#if defined F32 || defined ENABLE_ALL
    //*****************************   f32   *************************
    // max
    BENCH_START(riscv_max_f32);
    riscv_max_f32(f32_array, ARRAY_SIZE, &f32_out, &index);
    BENCH_END(riscv_max_f32);
    ref_max_f32(f32_array, ARRAY_SIZE, &f32_out_ref, &index_ref);
    if ((fabs(f32_out - f32_out_ref) > DELTAF32) || (index != index_ref)) {
        BENCH_ERROR(riscv_max_f32);
        printf("value expect: %f, actual: %f\nindex expect: %f, actual: %f",
               f32_out_ref, f32_out, index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_max_f32);
    // max_no_idx
    BENCH_START(riscv_max_no_idx_f32);
    riscv_max_no_idx_f32(f32_array, ARRAY_SIZE, &f32_out);
    BENCH_END(riscv_max_no_idx_f32);
    ref_max_no_idx_f32(f32_array, ARRAY_SIZE, &f32_out_ref);
    if ((fabs(f32_out - f32_out_ref) > DELTAF32) ) {
        BENCH_ERROR(riscv_max_no_idx_f32);
        printf("value expect: %f, actual: %f\nindex expect: %f, actual: %f",
               f32_out_ref, f32_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_max_no_idx_f32);
    // mean
    BENCH_START(riscv_mean_f32);
    riscv_mean_f32(f32_array, ARRAY_SIZE, &f32_out);
    BENCH_END(riscv_mean_f32);
    ref_mean_f32(f32_array, ARRAY_SIZE, &f32_out_ref);
    if (fabs(f32_out - f32_out_ref) > DELTAF32) {
        BENCH_ERROR(riscv_mean_f32);
        printf("expect: %f, actual: %f\n", f32_out_ref, f32_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_mean_f32);
    // min
    BENCH_START(riscv_min_f32);
    riscv_min_f32(f32_array, ARRAY_SIZE, &f32_out, &index);
    BENCH_END(riscv_min_f32);
    ref_min_f32(f32_array, ARRAY_SIZE, &f32_out_ref, &index_ref);
    if ((fabs(f32_out - f32_out_ref) > DELTAF32) || (index != index_ref)) {
        BENCH_ERROR(riscv_min_f32);
        printf("value expect: %f, actual: %f\nindex expect: %f, actual: %f",
               f32_out_ref, f32_out, index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_min_f32);
    // power
    BENCH_START(riscv_power_f32);
    riscv_power_f32(f32_array, ARRAY_SIZE, &f32_out);
    BENCH_END(riscv_power_f32);
    ref_power_f32(f32_array, ARRAY_SIZE, &f32_out_ref);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (fabs(f32_out - f32_out_ref) > DELTAF32) {
            BENCH_ERROR(riscv_power_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_out_ref,
                   f32_out);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_power_f32);
    // rms
    BENCH_START(riscv_rms_f32);
    riscv_rms_f32(f32_array, ARRAY_SIZE, &f32_out);
    BENCH_END(riscv_rms_f32);
    ref_rms_f32(f32_array, ARRAY_SIZE, &f32_out_ref);
    if (fabs(f32_out - f32_out_ref) > DELTAF32) {
        BENCH_ERROR(riscv_rms_f32);
        printf("expect: %f, actual: %f\n", f32_out_ref, f32_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_rms_f32);
    // std
    BENCH_START(riscv_std_f32);
    riscv_std_f32(f32_array, ARRAY_SIZE, &f32_out);
    BENCH_END(riscv_std_f32);
    ref_std_f32(f32_array, ARRAY_SIZE, &f32_out_ref);
    if (fabs(f32_out - f32_out_ref) > DELTAF32) {
        BENCH_ERROR(riscv_std_f32);
        printf("expect: %f, actual: %f\n", f32_out_ref, f32_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_std_f32);
    // var
    BENCH_START(riscv_var_f32);
    riscv_var_f32(f32_array, ARRAY_SIZE, &f32_out);
    BENCH_END(riscv_var_f32);
    ref_var_f32(f32_array, ARRAY_SIZE, &f32_out_ref);
    if (fabs(f32_out - f32_out_ref) > DELTAF32) {
        BENCH_ERROR(riscv_var_f32);
        printf("expect: %f, actual: %f\n", f32_out_ref, f32_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_var_f32);
    //absmax
    BENCH_START(riscv_absmax_f32);
    riscv_absmax_f32(f32_array, ARRAY_SIZE, &f32_out, &index);
    BENCH_END(riscv_absmax_f32);
    ref_absmax_f32(f32_array, ARRAY_SIZE, &f32_out_ref, &index_ref);
    if (fabs(f32_out - f32_out_ref) > DELTAF32) {
        BENCH_ERROR(riscv_absmax_f32);
        printf("expect: %f, actual: %f\n", f32_out_ref, f32_out);
        test_flag_error = 1;
    }
    if (fabs(index - index_ref) > DELTAF32) {
        BENCH_ERROR(riscv_absmax_f32);
        printf("expect: %f, actual: %f\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_absmax_f32);
    //absmin
    BENCH_START(riscv_absmin_f32);
    riscv_absmin_f32(f32_array, ARRAY_SIZE, &f32_out, &index);
    BENCH_END(riscv_absmin_f32);
    ref_absmin_f32(f32_array, ARRAY_SIZE, &f32_out_ref, &index_ref);
    if (fabs(f32_out - f32_out_ref) > DELTAF32) {
        BENCH_ERROR(riscv_absmin_f32);
        printf("expect: %f, actual: %f\n", f32_out_ref, f32_out);
        test_flag_error = 1;
    }
    if (fabs(index - index_ref) > DELTAF32) {
        BENCH_ERROR(riscv_absmin_f32);
        printf("expect: %d, actual: %d\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_absmin_f32);
#endif
#if defined Q31 || defined ENABLE_ALL
    // ********************************* q31 *****************************
    // max
    BENCH_START(riscv_max_q31);
    riscv_max_q31(q31_array, ARRAY_SIZE, &q31_out, &index);
    BENCH_END(riscv_max_q31);
    ref_max_q31(q31_array, ARRAY_SIZE, &q31_out_ref, &index_ref);
    if ((labs(q31_out - q31_out_ref) > DELTAQ31) || (index != index_ref)) {
        BENCH_ERROR(riscv_max_q31);
        printf("value expect: %x, actual: %x\nindex expect: %x, actual: %x\n",
               q31_out_ref, q31_out, index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_max_q31);
    // mean
    BENCH_START(riscv_mean_q31);
    riscv_mean_q31(q31_array, ARRAY_SIZE, &q31_out);
    BENCH_END(riscv_mean_q31);
    ref_mean_q31(q31_array, ARRAY_SIZE, &q31_out_ref);
    if (labs(q31_out - q31_out_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_mean_q31);
        printf("expect: %x, actual: %x\n", q31_out_ref, q31_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_mean_q31);
    // min
    BENCH_START(riscv_min_q31);
    riscv_min_q31(q31_array, ARRAY_SIZE, &q31_out, &index);
    BENCH_END(riscv_min_q31);
    ref_min_q31(q31_array, ARRAY_SIZE, &q31_out_ref, &index_ref);
    if ((labs(q31_out - q31_out_ref) > DELTAQ31) || (index != index_ref)) {
        BENCH_ERROR(riscv_min_q31);
        printf("value expect: %x, actual: %x\nindex expect: %d, actual: %d",
               q31_out_ref, q31_out, index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_min_q31);
    // power
    BENCH_START(riscv_power_q31);
    riscv_power_q31(q31_array, ARRAY_SIZE, &q63_out);
    BENCH_END(riscv_power_q31);
    ref_power_q31(q31_array, ARRAY_SIZE, &q63_out_ref);
    if (labs(q31_out - q31_out_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_power_q31);
        printf("expect: %x, actual: %x\n", q63_out_ref, q63_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_power_q31);
    // rms
    BENCH_START(riscv_rms_q31);
    riscv_rms_q31(q31_array, ARRAY_SIZE, &q31_out);
    BENCH_END(riscv_rms_q31);
    ref_rms_q31(q31_array, ARRAY_SIZE, &q31_out_ref);
    if (labs(q31_out - q31_out_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_rms_q31);
        printf("expect: %x, actual: %x\n", q31_out_ref, q31_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_rms_q31);
    // std
    BENCH_START(riscv_std_q31);
    riscv_std_q31(q31_array, ARRAY_SIZE, &q31_out);
    BENCH_END(riscv_std_q31);
    ref_std_q31(q31_array, ARRAY_SIZE, &q31_out_ref);
    if (labs(q31_out - q31_out_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_std_q31);
        printf("expect: %x, actual: %x\n", q31_out_ref, q31_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_std_q31);
    // var
    BENCH_START(riscv_var_q31);
    riscv_var_q31(q31_array, ARRAY_SIZE, &q31_out);
    BENCH_END(riscv_var_q31);
    ref_var_q31(q31_array, ARRAY_SIZE, &q31_out_ref);
    if (labs(q31_out - q31_out_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_var_q31);
        printf("expect: %x, actual: %x\n", q31_out_ref, q31_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_var_q31);
    //absmax
    BENCH_START(riscv_absmax_q31);
    riscv_absmax_q31(q31_array, ARRAY_SIZE, &q31_out, &index);
    BENCH_END(riscv_absmax_q31);
    ref_absmax_q31(q31_array, ARRAY_SIZE, &q31_out_ref, &index_ref);
    if (fabs(q31_out - q31_out_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_absmax_q31);
        printf("expect: %f, actual: %f\n", q31_out_ref, q31_out);
        test_flag_error = 1;
    }
    if (fabs(index - index_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_absmax_q31);
        printf("expect: %f, actual: %f\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_absmax_q31);
    //absmin
    BENCH_START(riscv_absmin_q31);
    riscv_absmin_q31(q31_array, ARRAY_SIZE, &q31_out, &index);
    BENCH_END(riscv_absmin_q31);
    ref_absmin_q31(q31_array, ARRAY_SIZE, &q31_out_ref, &index_ref);
    if (fabs(q31_out - q31_out_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_absmin_q31);
        printf("expect: %f, actual: %f\n", q31_out_ref, q31_out);
        test_flag_error = 1;
    }
    if (fabs(index - index_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_absmin_q31);
        printf("expect: %f, actual: %f\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_absmin_q31);
#endif
#if defined Q15 || defined ENABLE_ALL
    // ********************************* q15 *****************************
    // max
    BENCH_START(riscv_max_q15);
    riscv_max_q15(q15_array, ARRAY_SIZE, &q15_out, &index);
    BENCH_END(riscv_max_q15);
    ref_max_q15(q15_array, ARRAY_SIZE, &q15_out_ref, &index_ref);
    if ((abs(q15_out - q15_out_ref) > DELTAQ15) || (index != index_ref)) {
        BENCH_ERROR(riscv_max_q15);
        printf("value expect: %x, actual: %x\nindex expect: %d, actual: %d",
               q15_out_ref, q15_out, index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_max_q15);
    // mean
    BENCH_START(riscv_mean_q15);
    riscv_mean_q15(q15_array, ARRAY_SIZE, &q15_out);
    BENCH_END(riscv_mean_q15);
    ref_mean_q15(q15_array, ARRAY_SIZE, &q15_out_ref);
    if (abs(q15_out - q15_out_ref) > DELTAQ15) {
        BENCH_ERROR(riscv_mean_q15);
        printf("expect: %x, actual: %x\n", q15_out_ref, q15_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_mean_q15);
    // min
    BENCH_START(riscv_min_q15);
    riscv_min_q15(q15_array, ARRAY_SIZE, &q15_out, &index);
    BENCH_END(riscv_min_q15);
    ref_min_q15(q15_array, ARRAY_SIZE, &q15_out_ref, &index_ref);
    if ((abs(q15_out - q15_out_ref) > DELTAQ15) || (index != index_ref)) {
        BENCH_ERROR(riscv_min_q15);
        printf("value expect: %x, actual: %x\nindex expect: %d, actual: %d",
               q15_out_ref, q15_out, index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_min_q15);
    // power
    BENCH_START(riscv_power_q15);
    riscv_power_q15(q15_array, ARRAY_SIZE, &q63_out);
    BENCH_END(riscv_power_q15);
    ref_power_q15(q15_array, ARRAY_SIZE, &q63_out_ref);
    if (abs(q15_out - q15_out_ref) > DELTAQ15) {
        BENCH_ERROR(riscv_power_q15);
        printf("expect: %x, actual: %x\n", q63_out_ref, q63_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_power_q15);
    // rms
    BENCH_START(riscv_rms_q15);
    riscv_rms_q15(q15_array, ARRAY_SIZE, &q15_out);
    BENCH_END(riscv_rms_q15);
    ref_rms_q15(q15_array, ARRAY_SIZE, &q15_out_ref);
    if (abs(q15_out - q15_out_ref) > DELTAQ15) {
        BENCH_ERROR(riscv_rms_q15);
        printf("expect: %x, actual: %x\n", q15_out_ref, q15_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_rms_q15);
    // std
    BENCH_START(riscv_std_q15);
    riscv_std_q15(q15_array, ARRAY_SIZE, &q15_out);
    BENCH_END(riscv_std_q15);
    ref_std_q15(q15_array, ARRAY_SIZE, &q15_out_ref);
    if (abs(q15_out - q15_out_ref) > DELTAQ15) {
        BENCH_ERROR(riscv_std_q15);
        printf("expect: %x, actual: %x\n", q15_out_ref, q15_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_std_q15);
    // var
    BENCH_START(riscv_var_q15);
    riscv_var_q15(q15_array, ARRAY_SIZE, &q15_out);
    BENCH_END(riscv_var_q15);
    ref_var_q15(q15_array, ARRAY_SIZE, &q15_out_ref);
    if (abs(q15_out - q15_out_ref) > DELTAQ15) {
        BENCH_ERROR(riscv_var_q15);
        printf("expect: %x, actual: %x\n", q15_out_ref, q15_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_var_q15);
    //absmax
    BENCH_START(riscv_absmax_q15);
    riscv_absmax_q15(q15_array, ARRAY_SIZE, &q15_out, &index);
    BENCH_END(riscv_absmax_q15);
    ref_absmax_q15(q15_array, ARRAY_SIZE, &q15_out_ref, &index_ref);
    if (fabs(q15_out - q15_out_ref) > DELTAQ15) {
        BENCH_ERROR(riscv_absmax_q15);
        printf("expect: %f, actual: %f\n", q15_out_ref, q15_out);
        test_flag_error = 1;
    }
    if (fabs(index - index_ref) > DELTAQ15) {
        BENCH_ERROR(riscv_absmax_q15);
        printf("expect: %f, actual: %f\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_absmax_q15);
    //absmin
    BENCH_START(riscv_absmin_q15);
    riscv_absmin_q15(q15_array, ARRAY_SIZE, &q15_out, &index);
    BENCH_END(riscv_absmin_q15);
    ref_absmin_q15(q15_array, ARRAY_SIZE, &q15_out_ref, &index_ref);
    if (fabs(q15_out - q15_out_ref) > DELTAQ15) {
        BENCH_ERROR(riscv_absmin_q15);
        printf("expect: %f, actual: %f\n", q15_out_ref, q15_out);
        test_flag_error = 1;
    }
    if (fabs(index - index_ref) > DELTAQ15) {
        BENCH_ERROR(riscv_absmin_q15);
        printf("expect: %f, actual: %f\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_absmin_q15);
#endif
#if defined Q7 || defined ENABLE_ALL
    // ********************************* q7 *****************************
    // max
    BENCH_START(riscv_max_q7);
    riscv_max_q7(q7_array, ARRAY_SIZE, &q7_out, &index);
    BENCH_END(riscv_max_q7);
    ref_max_q7(q7_array, ARRAY_SIZE, &q7_out_ref, &index_ref);
    if ((abs(q7_out - q7_out_ref) > DELTAQ7) || (index != index_ref)) {
        BENCH_ERROR(riscv_max_q7);
        printf("value expect: %x, actual: %x\nindex expect: %d, actual: %d",
               q7_out_ref, q7_out, index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_max_q7);
    // mean
    BENCH_START(riscv_mean_q7);
    riscv_mean_q7(q7_array, ARRAY_SIZE, &q7_out);
    BENCH_END(riscv_mean_q7);
    ref_mean_q7(q7_array, ARRAY_SIZE, &q7_out_ref);
    if (abs(q7_out - q7_out_ref) > DELTAQ7) {
        BENCH_ERROR(riscv_mean_q7);
        printf("expect: %x, actual: %x\n", q7_out_ref, q7_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_mean_q7);
    // min
    BENCH_START(riscv_min_q7);
    riscv_min_q7(q7_array, ARRAY_SIZE, &q7_out, &index);
    BENCH_END(riscv_min_q7);
    ref_min_q7(q7_array, ARRAY_SIZE, &q7_out_ref, &index_ref);
    if ((abs(q7_out - q7_out_ref) > DELTAQ7) || (index != index_ref)) {
        BENCH_ERROR(riscv_min_q7);
        printf("value expect: %x, actual: %x\nindex expect: %d, actual: %d",
               q7_out_ref, q7_out, index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_min_q7);
    // power
    BENCH_START(riscv_power_q7);
    riscv_power_q7(q7_array, ARRAY_SIZE, &q31_out);
    BENCH_END(riscv_power_q7);
    ref_power_q7(q7_array, ARRAY_SIZE, &q31_out_ref);
    if (abs(q31_out - q31_out_ref) > DELTAQ7) {
        BENCH_ERROR(riscv_power_q7);
        printf("expect: %x, actual: %x\n", q63_out_ref, q63_out);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_power_q7);
    //absmax
    BENCH_START(riscv_absmax_q7);
    riscv_absmax_q7(q7_array, ARRAY_SIZE, &q7_out, &index);
    BENCH_END(riscv_absmax_q7);
    ref_absmax_q7(q7_array, ARRAY_SIZE, &q7_out_ref, &index_ref);
    if (fabs(q7_out - q7_out_ref) > DELTAQ7) {
        BENCH_ERROR(riscv_absmax_q7);
        printf("expect: %f, actual: %f\n", q7_out_ref, q7_out);
        test_flag_error = 1;
    }
    if (fabs(index - index_ref) > DELTAQ7) {
        BENCH_ERROR(riscv_absmax_q7);
        printf("expect: %f, actual: %f\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_absmax_q7);
    //absmin
    BENCH_START(riscv_absmin_q7);
    riscv_absmin_q7(q7_array, ARRAY_SIZE, &q7_out, &index);
    BENCH_END(riscv_absmin_q7);
    ref_absmin_q7(q7_array, ARRAY_SIZE, &q7_out_ref, &index_ref);
    if (fabs(q7_out - q7_out_ref) > DELTAQ7) {
        BENCH_ERROR(riscv_absmin_q7);
        printf("expect: %f, actual: %f\n", q7_out_ref, q7_out);
        test_flag_error = 1;
    }
    if (fabs(index - index_ref) > DELTAQ7) {
        BENCH_ERROR(riscv_absmin_q7);
        printf("expect: %f, actual: %f\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_absmin_q7);
    printf("all tests are passed,well done!\n");
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
