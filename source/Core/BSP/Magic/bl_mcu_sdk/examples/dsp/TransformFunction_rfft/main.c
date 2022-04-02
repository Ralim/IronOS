//
// Created by lujun on 19-6-28.
//
// This contains SIN_COS , clarke, inv_clarke, park, inv_park and pid
// each one has it's own function.
// All function can be found in main function.
// If you don't want to use it, then comment it.
#include "riscv_common_tables.h"
#include "riscv_const_structs.h"
#include "riscv_math.h"
#include "array.h"
#include <stdint.h>
#include "../common.h"

#include "../HelperFunctions/math_helper.c"
#include "../HelperFunctions/ref_helper.c"

#include <stdio.h>
#define DELTAF32 (0.05f)
#define DELTAQ31 (63)
#define DELTAQ15 (1)
#define DELTAQ7 (1)

int test_flag_error = 0;

uint32_t fftSize = 1024;
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;

static int DSP_rfft_q31(void)
{
    /* clang-format off */
    uint16_t i, j;
    /* clang-format on */
    riscv_float_to_q31(rfft_testinput_f32_50hz_200Hz,
                     rfft_testinput_q31_50hz_200Hz, 1024);
    riscv_float_to_q31(rfft_testinput_f32_50hz_200Hz,
                     rfft_testinput_q31_50hz_200Hz_ref, 1024);
    riscv_rfft_instance_q31 SS;
    fftSize = 512;
    ifftFlag = 0;
    doBitReverse = 1;
    riscv_rfft_init_q31(&SS, fftSize, ifftFlag, doBitReverse);
    BENCH_START(riscv_rfft_q31);
    riscv_rfft_q31(&SS, rfft_testinput_q31_50hz_200Hz, q31_testOutput);
    BENCH_END(riscv_rfft_q31);
    riscv_rfft_init_q31(&SS, fftSize, ifftFlag, doBitReverse);
    ref_rfft_q31(&SS, rfft_testinput_q31_50hz_200Hz_ref, q31_testOutput_ref);
    q31_t resault, resault_ref;
    uint32_t index, index_ref;
    riscv_shift_q31(q31_testOutput, 8, q31_testOutput, fftSize);
    riscv_shift_q31(q31_testOutput_ref, 8, q31_testOutput_ref, fftSize);
    riscv_max_q31(q31_testOutput, 512, &resault, &index);
    riscv_max_q31(q31_testOutput_ref, 512, &resault_ref, &index_ref);
    if (index != index_ref) {
        BENCH_ERROR(riscv_rfft_q31);
        printf("expect: %d, actual: %d\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_rfft_q31);
}
static int DSP_rfft_q15(void)
{
    /* clang-format off */
    uint16_t i, j;
    /* clang-format on */
    riscv_float_to_q15(rfft_testinput_f32_50hz_200Hz,
                     rfft_testinput_q15_50hz_200Hz, 1024);
    riscv_float_to_q15(rfft_testinput_f32_50hz_200Hz,
                     rfft_testinput_q15_50hz_200Hz_ref, 1024);
    riscv_rfft_instance_q15 SS;
    fftSize = 512;
    ifftFlag = 0;
    doBitReverse = 1;
    riscv_rfft_init_q15(&SS, fftSize, ifftFlag, doBitReverse);
    BENCH_START(riscv_rfft_q15);
    riscv_rfft_q15(&SS, rfft_testinput_q15_50hz_200Hz, q15_testOutput);
    BENCH_END(riscv_rfft_q15);
    riscv_rfft_init_q15(&SS, fftSize, ifftFlag, doBitReverse);
    ref_rfft_q15(&SS, rfft_testinput_q15_50hz_200Hz_ref, q15_testOutput_ref);
    q15_t resault, resault_ref;
    riscv_shift_q15(q15_testOutput, 6, q15_testOutput, fftSize);
    riscv_shift_q15(q15_testOutput_ref, 6, q15_testOutput_ref, fftSize);
    uint32_t index, index_ref;
    riscv_max_q15(q15_testOutput, fftSize, &resault, &index);
    riscv_max_q15(q15_testOutput_ref, fftSize, &resault_ref, &index_ref);
    if (index != index_ref) {
        BENCH_ERROR(riscv_rfft_q15);
        printf("expect: %d, actual: %d\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_rfft_q15);
}

int main()
{
    BENCH_INIT;
    DSP_rfft_q31();
    DSP_rfft_q15();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
