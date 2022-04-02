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
#define NEGATE
#define OFFSET
#define SCALE
#define SHIFT
#define SUB
#define CLIP

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
#if defined NEGATE || defined ENABLE_ALL
    // ****************************************   negate
    // ******************************* f32_negate
    BENCH_START(riscv_negate_f32);
    riscv_negate_f32(f32_a_array, f32_out_array, ARRAY_SIZE);
    BENCH_END(riscv_negate_f32);
    ref_negate_f32(f32_a_array, f32_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (fabs(f32_out_array[i] - f32_out_ref_array[i]) > DELTAF32) {
            BENCH_ERROR(riscv_negate_f32);
            printf("index: %d, expect: %f, actual: %f\n", i,
                   f32_out_ref_array[i], f32_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_negate_f32);

    // riscv_negate_q7.c
    BENCH_START(riscv_negate_q7);
    riscv_negate_q7(q7_a_array, q7_out_array, ARRAY_SIZE);
    BENCH_END(riscv_negate_q7);
    ref_negate_q7(q7_a_array, q7_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q7_out_array[i] - q7_out_ref_array[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_negate_q7);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q7_out_ref_array[i], q7_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_negate_q7);

    // riscv_negate_q15.c
    BENCH_START(riscv_negate_q15);
    riscv_negate_q15(q15_a_array, q15_out_array, ARRAY_SIZE);
    BENCH_END(riscv_negate_q15);
    ref_negate_q15(q15_a_array, q15_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q15_out_array[i] - q15_out_ref_array[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_negate_q15);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q15_out_ref_array[i], q15_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_negate_q15);

    // riscv_negate_q31.c
    BENCH_START(riscv_negate_q31);
    riscv_negate_q31(q31_a_array, q31_out_array, ARRAY_SIZE);
    BENCH_END(riscv_negate_q31);
    ref_negate_q31(q31_a_array, q31_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (labs(q31_out_array[i] - q31_out_ref_array[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_negate_q31);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q31_out_ref_array[i], q31_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_negate_q31);
#endif
#if defined OFFSET || defined ENABLE_ALL
    // *************************************   offset
    // ********************************** f32_offset
    BENCH_START(riscv_offset_f32);
    riscv_offset_f32(f32_a_array, 100, f32_out_array, ARRAY_SIZE);
    BENCH_END(riscv_offset_f32);
    ref_offset_f32(f32_a_array, 100, f32_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (fabs(f32_out_array[i] - f32_out_ref_array[i]) > DELTAF32) {
            BENCH_ERROR(riscv_offset_f32);
            printf("index: %d, expect: %f, actual: %f\n", i,
                   f32_out_ref_array[i], f32_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_offset_f32);

    // riscv_offset_q7.c
    BENCH_START(riscv_offset_q7);
    riscv_offset_q7(q7_a_array, 100, q7_out_array, ARRAY_SIZE);
    BENCH_END(riscv_offset_q7);
    ref_offset_q7(q7_a_array, 100, q7_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q7_out_array[i] - q7_out_ref_array[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_offset_q7);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q7_out_ref_array[i], q7_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_offset_q7);

    // riscv_offset_q15.c
    BENCH_START(riscv_offset_q15);
    riscv_offset_q15(q15_a_array, 100, q15_out_array, ARRAY_SIZE);
    BENCH_END(riscv_offset_q15);
    ref_offset_q15(q15_a_array, 100, q15_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q15_out_array[i] - q15_out_ref_array[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_offset_q15);
            printf("index:%d, expect: %x, actual: %x\n", i,
                   q15_out_ref_array[i], q15_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_offset_q15);

    // riscv_offset_q31.c
    BENCH_START(riscv_offset_q31);
    riscv_offset_q31(q31_a_array, 100, q31_out_array, ARRAY_SIZE);
    BENCH_END(riscv_offset_q31);
    ref_offset_q31(q31_a_array, 100, q31_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (labs(q31_out_array[i] - q31_out_ref_array[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_offset_q31);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q31_out_ref_array[i], q31_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_offset_q31);
#endif
#if defined SCALE || defined ENABLE_ALL
    // *************************************   scale ***************************
    // f32_scale
    BENCH_START(riscv_scale_f32);
    riscv_scale_f32(f32_a_array, 3.5, f32_out_array, ARRAY_SIZE);
    BENCH_END(riscv_scale_f32);
    ref_scale_f32(f32_a_array, 3.5, f32_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (fabs(f32_out_array[i] - f32_out_ref_array[i]) > DELTAF32) {
            BENCH_ERROR(riscv_scale_f32);
            printf("index: %d, expect: %f, actual: %f\n", i,
                   f32_out_ref_array[i], f32_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_scale_f32);

    // riscv_scale_q7.c
    BENCH_START(riscv_scale_q7);
    riscv_scale_q7(q7_a_array, 6, 7, q7_out_array, ARRAY_SIZE);
    BENCH_END(riscv_scale_q7);
    ref_scale_q7(q7_a_array, 6, 7, q7_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q7_out_array[i] - q7_out_ref_array[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_scale_q7);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q7_out_ref_array[i], q7_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_scale_q7);

    // riscv_scale_q15.c
    BENCH_START(riscv_scale_q15);
    riscv_scale_q15(q15_a_array, 15, 15, q15_out_array, ARRAY_SIZE);
    BENCH_END(riscv_scale_q15);
    ref_scale_q15(q15_a_array, 15, 15, q15_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q15_out_array[i] - q15_out_ref_array[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_scale_q15);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q15_out_ref_array[i], q15_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_scale_q15);

    // riscv_scale_q31.c
    BENCH_START(riscv_scale_q31);
    riscv_scale_q31(q31_a_array, 987746463, -1, q31_out_array, ARRAY_SIZE);
    BENCH_END(riscv_scale_q31);
    ref_scale_q31(q31_a_array, 987746463, -1, q31_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (labs(q31_out_array[i] - q31_out_ref_array[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_scale_q31);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q31_out_ref_array[i], q31_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_scale_q31);
#endif
#if defined SHIFT || defined ENABLE_ALL
    // **************************************   shift
    // ************************************** riscv_shift_q7.c
    BENCH_START(riscv_shift_q7);
    riscv_shift_q7(q7_a_array, -3, q7_out_array, ARRAY_SIZE);
    BENCH_END(riscv_shift_q7);
    ref_shift_q7(q7_a_array, -3, q7_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q7_out_array[i] - q7_out_ref_array[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_shift_q7);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q7_out_ref_array[i], q7_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_shift_q7);

    // riscv_shift_q15.c
    BENCH_START(riscv_shift_q15);
    riscv_shift_q15(q15_a_array, -3, q15_out_array, ARRAY_SIZE);
    BENCH_END(riscv_shift_q15);
    ref_shift_q15(q15_a_array, -3, q15_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q15_out_array[i] - q15_out_ref_array[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_shift_q15);
            printf("index:%d expect: %x, actual: %x\n", i, q15_out_ref_array[i],
                   q15_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_shift_q15);

    // riscv_shift_q31.c
    BENCH_START(riscv_shift_q31);
    riscv_shift_q31(q31_a_array, -3, q31_out_array, ARRAY_SIZE);
    BENCH_END(riscv_shift_q31);
    ref_shift_q31(q31_a_array, -3, q31_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (labs(q31_out_array[i] - q31_out_ref_array[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_shift_q31);
            printf("index:%d, expect: %d, actual: %d\n", i,
                   q31_out_ref_array[i], q31_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_shift_q31);
#endif
#if defined SUB || defined ENABLE_ALL
    // *************************************   sub
    // **************************************** riscv_sub_f32.c
    BENCH_START(riscv_sub_f32);
    riscv_sub_f32(f32_a_array, f32_b_array, f32_out_array, ARRAY_SIZE);
    BENCH_END(riscv_sub_f32);
    ref_sub_f32(f32_a_array, f32_b_array, f32_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (fabs(f32_out_array[i] - f32_out_ref_array[i]) > DELTAF32) {
            BENCH_ERROR(riscv_sub_f32);
            printf("i: %d, expect: %f, actual: %f\n", i, f32_out_ref_array[i],
                   f32_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_sub_f32);

    // riscv_sub_q7.c
    BENCH_START(riscv_sub_q7);
    riscv_sub_q7(q7_a_array, q7_b_array, q7_out_array, ARRAY_SIZE);
    BENCH_END(riscv_sub_q7);
    ref_sub_q7(q7_a_array, q7_b_array, q7_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q7_out_array[i] - q7_out_ref_array[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_sub_q7);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q7_out_ref_array[i], q7_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_sub_q7);

    // riscv_sub_q15.c
    BENCH_START(riscv_sub_q15);
    riscv_sub_q15(q15_a_array, q15_b_array, q15_out_array, ARRAY_SIZE);
    BENCH_END(riscv_sub_q15);
    ref_sub_q15(q15_a_array, q15_b_array, q15_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q15_out_array[i] - q15_out_ref_array[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_sub_q15);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q15_out_ref_array[i], q15_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_sub_q15);

    // riscv_sub_q31.c
    BENCH_START(riscv_sub_q31);
    riscv_sub_q31(q31_a_array, q31_b_array, q31_out_array, ARRAY_SIZE);
    BENCH_END(riscv_sub_q31);
    ref_sub_q31(q31_a_array, q31_b_array, q31_out_ref_array, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (labs(q31_out_array[i] - q31_out_ref_array[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_sub_q31);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q31_out_ref_array[i], q31_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_sub_q31);
#endif
#if defined CLIP || defined ENABLE_ALL
    BENCH_START(riscv_clip_f32);
    riscv_clip_f32(f32_a_array, f32_out_array, -4230.37, 4230.45, ARRAY_SIZE);
    BENCH_END(riscv_clip_f32);
    ref_clip_f32(f32_a_array, f32_out_ref_array, -4230.37, 4230.45, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (fabs(f32_out_array[i] - f32_out_ref_array[i]) > DELTAF32) {
            BENCH_ERROR(riscv_clip_f32);
            printf("i: %d, expect: %f, actual: %f\n", i, f32_out_ref_array[i],
                   f32_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_clip_f32);

    BENCH_START(riscv_clip_q7);
    riscv_clip_q7(q7_a_array, q7_out_array, -100, 100, ARRAY_SIZE);
    BENCH_END(riscv_clip_q7);
    ref_clip_q7(q7_a_array, q7_out_ref_array, -100, 100, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q7_out_array[i] - q7_out_ref_array[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_clip_q7);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q7_out_ref_array[i], q7_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_clip_q7);

    BENCH_START(riscv_clip_q15);
    riscv_clip_q15(q15_a_array, q15_out_array, -2638, 1000, ARRAY_SIZE);
    BENCH_END(riscv_clip_q15);
    ref_clip_q15(q15_a_array, q15_out_ref_array, -2638, 1000, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q15_out_array[i] - q15_out_ref_array[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_clip_q15);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q15_out_ref_array[i], q15_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_clip_q15);

    BENCH_START(riscv_clip_q31);
    riscv_clip_q31(q31_a_array, q31_out_array, -214712319, 214712319, ARRAY_SIZE);
    BENCH_END(riscv_clip_q31);
    ref_clip_q31(q31_a_array, q31_out_ref_array, -214712319, 214712319, ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
        if (abs(q31_out_array[i] - q31_out_ref_array[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_clip_q31);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q31_out_ref_array[i], q31_out_array[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_clip_q31);
#endif /*defined CLIP || defined ENABLE_ALL*/

    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
};
