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

static int DSP_cfft_radix2_q15(void)
{
    uint16_t i;
    riscv_float_to_q15(cfft_testinput_f32_50hz_200Hz,
                     cfft_testinput_q15_50hz_200Hz, 1024);
    fftSize = 512;
    riscv_cfft_radix2_instance_q15 S;
    uint8_t ifftFlag = 0, doBitReverse = 1;
    riscv_cfft_radix2_init_q15(&S, 512, ifftFlag, doBitReverse);
    BENCH_START(riscv_cfft_radix2_q15);
    riscv_cfft_radix2_q15(&S, cfft_testinput_q15_50hz_200Hz);
    BENCH_END(riscv_cfft_radix2_q15);
    q15_t resault, resault_ref;
    uint32_t index, index_ref = 205;
    riscv_max_q15(cfft_testinput_q15_50hz_200Hz, 1024, &resault, &index);
    if (index != index_ref) {
        BENCH_ERROR(riscv_cfft_radix2_q15);
        printf("expect: %d, actual: %d\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_cfft_radix2_q15);
}
static int DSP_cfft_radix2_q31(void)
{
    uint16_t i;
    riscv_float_to_q31(cfft_testinput_f32_50hz_200Hz,
                     cfft_testinput_q31_50hz_200Hz, 1024);
    riscv_float_to_q31(cfft_testinput_f32_50hz_200Hz_ref,
                     cfft_testinput_q31_50hz_200Hz_ref, 1024);
    fftSize = 512;
    riscv_cfft_radix2_instance_q31 S;
    uint8_t ifftFlag = 0, doBitReverse = 1;
    riscv_cfft_radix2_init_q31(&S, 512, ifftFlag, doBitReverse);
    BENCH_START(riscv_cfft_radix2_q31);
    riscv_cfft_radix2_q31(&S, cfft_testinput_q31_50hz_200Hz);
    BENCH_END(riscv_cfft_radix2_q31);
    ref_cfft_radix2_q31(&S, cfft_testinput_q31_50hz_200Hz_ref);
    q31_t resault, resault_ref;
    uint32_t index, index_ref;
    riscv_max_q31(cfft_testinput_q31_50hz_200Hz, 1024, &resault, &index);
    riscv_max_q31(cfft_testinput_q31_50hz_200Hz_ref, 1024, &resault_ref,
                &index_ref);
    if (index != index_ref) {
        BENCH_ERROR(riscv_cfft_radix2_q31);
        printf("expect: %d, actual: %d\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_cfft_radix2_q31);
}
static int DSP_cfft_radix2_f32(void)
{
    uint16_t i;
    fftSize = 512;
    riscv_cfft_radix2_instance_f32 S;
    uint8_t ifftFlag = 0, doBitReverse = 1;
    riscv_cfft_radix2_init_f32(&S, 512, ifftFlag, doBitReverse);
    BENCH_START(riscv_cfft_radix2_f32);
    riscv_cfft_radix2_f32(&S, cfft_testinput_f32_50hz_200Hz);
    BENCH_END(riscv_cfft_radix2_f32);
    ref_cfft_radix2_f32(&S, cfft_testinput_f32_50hz_200Hz_ref);
    float32_t resault, resault_ref;
    uint32_t index, index_ref;
    riscv_max_f32(cfft_testinput_f32_50hz_200Hz, 1024, &resault, &index);
    riscv_max_f32(cfft_testinput_f32_50hz_200Hz_ref, 1024, &resault_ref,
                &index_ref);
    if (index != index_ref) {
        BENCH_ERROR(riscv_cfft_radix2_f32);
        printf("expect: %d, actual: %d\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_cfft_radix2_f32);
}

int main()
{
    BENCH_INIT;
    DSP_cfft_radix2_q15();
    DSP_cfft_radix2_q31();
    DSP_cfft_radix2_f32();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
