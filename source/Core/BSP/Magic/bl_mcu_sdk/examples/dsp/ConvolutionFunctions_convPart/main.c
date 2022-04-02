//
// Created by lujun on 19-6-28.
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
#define DELTAQ15 (2)
#define DELTAQ7 (2)
#define RISCV_MATH_LOOPUNROLL

int test_flag_error = 0;
int main()
{
    uint16_t i;
    BENCH_INIT;

    // *****************************************************
    // conv part
    // *****************************************************
    BENCH_START(riscv_conv_f32);
    riscv_conv_f32(test_conv_input_f32_A, ARRAYA_SIZE_F32, test_conv_input_f32_B, ARRAYB_SIZE_F32,
                 output_f32);
    BENCH_END(riscv_conv_f32);
    ref_conv_f32(test_conv_input_f32_A, ARRAYA_SIZE_F32, test_conv_input_f32_B, ARRAYB_SIZE_F32,
                 output_f32_ref);
    for (int i = 0; i < (ARRAYA_SIZE_F32 + ARRAYB_SIZE_F32 - 1); i++)
        if (fabs(output_f32_ref[i] - output_f32[i]) > DELTAF32) {
            BENCH_ERROR(riscv_conv_f32);
            printf("index:%d, expect: %f, actual: %f\n", i, output_f32_ref[i],
                   output_f32[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_conv_f32);

    BENCH_START(riscv_conv_q31);
    riscv_conv_q31(test_conv_input_q31_A, ARRAYA_SIZE_Q31, test_conv_input_q31_B, ARRAYB_SIZE_Q31,
                 output_q31);
    BENCH_END(riscv_conv_q31);
    ref_conv_q31(test_conv_input_q31_A, ARRAYA_SIZE_Q31, test_conv_input_q31_B, ARRAYB_SIZE_Q31,
                 output_q31_ref);
    for (int i = 0; i < (ARRAYA_SIZE_Q31 + ARRAYB_SIZE_Q31 - 1); i++)
        if (labs(output_q31_ref[i] - output_q31[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_conv_q31);
            printf("index:%d, expect: %d, actual: %d\n", i, output_q31_ref[i],
                   output_q31[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_conv_q31);

    BENCH_START(riscv_conv_q15);
    riscv_conv_q15(test_conv_input_q15_A, ARRAYA_SIZE_Q15, test_conv_input_q15_B, ARRAYB_SIZE_Q15,
                 output_q15);
    BENCH_END(riscv_conv_q15);
    ref_conv_q15(test_conv_input_q15_A, ARRAYA_SIZE_Q15, test_conv_input_q15_B, ARRAYB_SIZE_Q15,
                 output_q15_ref);
    for (int i = 0; i < (ARRAYA_SIZE_Q15 + ARRAYB_SIZE_Q15 - 1); i++)
        if (abs(output_q15_ref[i] - output_q15[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_conv_q15);
            printf("index:%d, expect: %d, actual: %d\n", i, output_q15_ref[i],
                   output_q15[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_conv_q15);

    BENCH_START(riscv_conv_q7);
    riscv_conv_q7(test_conv_input_q7_A, ARRAYA_SIZE_Q7, test_conv_input_q7_B, ARRAYB_SIZE_Q7, output_q7);
    BENCH_END(riscv_conv_q7);
    ref_conv_q7(test_conv_input_q7_A, ARRAYA_SIZE_Q7, test_conv_input_q7_B, ARRAYB_SIZE_Q7,
                output_q7_ref);
    for (int i = 0; i < (ARRAYA_SIZE_Q7 + ARRAYB_SIZE_Q7 - 1); i++)
        if (abs(output_q7_ref[i] - output_q7[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_conv_q7);
            printf("index:%d, expect: %d, actual: %d\n", i, output_q7_ref[i],
                   output_q7[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_conv_q7);

    BENCH_START(riscv_conv_fast_q15);
    riscv_conv_fast_q15(test_conv_input_q15_A, ARRAYA_SIZE_Q15, test_conv_input_q15_B, ARRAYB_SIZE_Q15,
                      output_q15);
    BENCH_END(riscv_conv_fast_q15);
    ref_conv_fast_q15(test_conv_input_q15_A, ARRAYA_SIZE_Q15, test_conv_input_q15_B, ARRAYB_SIZE_Q15,
                      output_q15_ref);
    for (int i = 0; i < (ARRAYA_SIZE_Q15 + ARRAYB_SIZE_Q15 - 1); i++)
        if (abs(output_q15_ref[i] - output_q15[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_conv_fast_q15);
            printf("index:%d, expect: %d, actual: %d\n", i, output_q15_ref[i],
                   output_q15[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_conv_fast_q15);

    BENCH_START(riscv_conv_fast_q31);
    riscv_conv_fast_q31(test_conv_input_q31_A, ARRAYA_SIZE_Q31, test_conv_input_q31_B, ARRAYB_SIZE_Q31,
                      output_q31);
    BENCH_END(riscv_conv_fast_q31);
    ref_conv_fast_q31(test_conv_input_q31_A, ARRAYA_SIZE_Q31, test_conv_input_q31_B, ARRAYB_SIZE_Q31,
                      output_q31_ref);
    for (int i = 0; i < (ARRAYA_SIZE_Q31 + ARRAYB_SIZE_Q31 - 1); i++)
        if (labs(output_q31_ref[i] - output_q31[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_conv_fast_q31);
            printf("index:%d, expect: %d, actual: %d\n", i, output_q31_ref[i],
                   output_q31[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_conv_fast_q31);

    BENCH_START(riscv_conv_opt_q15);
    riscv_conv_opt_q15(test_conv_input_q15_A, ARRAYA_SIZE_Q15, test_conv_input_q15_B, ARRAYB_SIZE_Q15,
                     output_q15, pScratch1, pScratch2);
    BENCH_END(riscv_conv_opt_q15);
    ref_conv_opt_q15(test_conv_input_q15_A, ARRAYA_SIZE_Q15, test_conv_input_q15_B, ARRAYB_SIZE_Q15,
                     output_q15_ref, pScratch1, pScratch2);
    for (int i = 0; i < (ARRAYA_SIZE_Q15 + ARRAYB_SIZE_Q15 - 1); i++)
        if (abs(output_q15_ref[i] - output_q15[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_conv_opt_q15);
            printf("index:%d, expect: %d, actual: %d\n", i, output_q15_ref[i],
                   output_q15[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_conv_opt_q15);

    BENCH_START(riscv_conv_opt_q7);
    riscv_conv_opt_q7(test_conv_input_q7_A, ARRAYA_SIZE_Q7, test_conv_input_q7_B, ARRAYB_SIZE_Q7,
                    output_q7, pScratch1, pScratch2);
    BENCH_END(riscv_conv_opt_q7);
    ref_conv_opt_q7(test_conv_input_q7_A, ARRAYA_SIZE_Q7, test_conv_input_q7_B, ARRAYB_SIZE_Q7,
                    output_q7_ref, pScratch1, pScratch2);
    for (int i = 0; i < (ARRAYA_SIZE_Q7 + ARRAYB_SIZE_Q7 - 1); i++)
        if (abs(output_q7_ref[i] - output_q7[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_conv_opt_q7);
            printf("index:%d, expect: %d, actual: %d\n", i, output_q7_ref[i],
                   output_q7[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_conv_opt_q7);

    BENCH_START(riscv_conv_fast_opt_q15);
    riscv_conv_fast_opt_q15(test_conv_input_q15_A, ARRAYA_SIZE_Q15, test_conv_input_q15_B, ARRAYB_SIZE_Q15,
                          output_q15, pScratch1, pScratch2);
    BENCH_END(riscv_conv_fast_opt_q15);
    ref_conv_fast_opt_q15(test_conv_input_q15_A, ARRAYA_SIZE_Q15, test_conv_input_q15_B, ARRAYB_SIZE_Q15,
                          output_q15_ref, pScratch1, pScratch2);
    for (int i = 0; i < (ARRAYA_SIZE_Q7 + ARRAYB_SIZE_Q7 - 1); i++)
        if (abs(output_q15_ref[i] - output_q15[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_conv_fast_opt_q15);
            printf("index:%d, expect: %d, actual: %d\n", i, output_q15_ref[i],
                   output_q15[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_conv_fast_opt_q15);
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
