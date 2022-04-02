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

static int DSP_dct4_f32(void)
{
    uint16_t i;
    /* clang-format off */
    fftSize = 128;
    riscv_cfft_radix4_instance_f32 S;
    uint8_t ifftFlag = 0, doBitReverse = 1;
    /* clang-format on */
    riscv_cfft_radix4_init_f32(&S, 64, ifftFlag, doBitReverse);
    riscv_rfft_instance_f32 SS;
    ifftFlag = 0;
    doBitReverse = 1;
    riscv_rfft_init_f32(&SS, &S, fftSize, ifftFlag, doBitReverse);
    riscv_dct4_instance_f32 SSS = {128, 64, 0.125, Weights_128, cos_factors_128,
                                 &SS, &S};
    BENCH_START(riscv_dct4_f32);
    riscv_dct4_f32(&SSS, f32_state, dct4_testinput_f32_50hz_200Hz);
    BENCH_END(riscv_dct4_f32);
    // ref_dct4_f32(&SSS, f32_state, dct4_testinput_f32_50hz_200Hz_ref);
    float32_t resault, resault_ref;
    uint32_t index, index_ref = 5;
    riscv_max_f32(dct4_testinput_f32_50hz_200Hz, 128, &resault, &index);
    // riscv_max_f32(dct4_testinput_f32_50hz_200Hz_ref,128,&resault_ref,&index_ref);
    if (index != index_ref) {
        BENCH_ERROR(riscv_dct4_f32);
        printf("expect: %d, actual: %d\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_dct4_f32);
}
static int DSP_dct4_q31(void)
{
    uint16_t i;
    /* clang-format off */
    /* clang-format on */
    riscv_float_to_q31(dct4_testinput_f32_50hz_200Hz_q31,
                     dct4_testinput_q31_50hz_200Hz, 256);
    riscv_float_to_q31(dct4_testinput_f32_50hz_200Hz_q31,
                     dct4_testinput_q31_50hz_200Hz_ref, 256);
    fftSize = 128;

    riscv_cfft_radix4_instance_q31 S;
    // uint8_t ifftFlag = 0, doBitReverse = 1;
    // riscv_cfft_radix4_init_q31(&S, 64, ifftFlag, doBitReverse);

    riscv_rfft_instance_q31 SS;
    // riscv_rfft_init_q31(&SS, fftSize, ifftFlag, doBitReverse);
    riscv_dct4_instance_q31 SSS;
    // riscv_dct4_instance_q31 SSS = {128, 64, 0x10000000, WeightsQ31_128,
    // cos_factorsQ31_128, &SS, &S};
    riscv_dct4_init_q31(&SSS, &SS, &S, 128, 64, 0x10000000);
    BENCH_START(riscv_dct4_q31);
    riscv_dct4_q31(&SSS, q31_state, dct4_testinput_q31_50hz_200Hz);
    BENCH_END(riscv_dct4_q31);
    // riscv_dct4_init_q31(&SSS,&SS,&S,128,64,0x10000000);
    // ref_dct4_q31(&SSS, q31_state, dct4_testinput_q31_50hz_200Hz_ref);
    q31_t resault, resault_ref;
    uint32_t index, index_ref = 5;
    riscv_shift_q31(dct4_testinput_q31_50hz_200Hz, 7,
                  dct4_testinput_q31_50hz_200Hz, fftSize);
    // riscv_shift_q31(dct4_testinput_q31_50hz_200Hz_ref,9,dct4_testinput_q31_50hz_200Hz_ref,fftSize);
    riscv_max_q31(dct4_testinput_q31_50hz_200Hz, 128, &resault, &index);
    // riscv_max_q31(dct4_testinput_q31_50hz_200Hz_ref,128,&resault_ref,&index_ref);
    if (index != index_ref) {
        BENCH_ERROR(riscv_dct4_q31);
        printf("expect: %d, actual: %d\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_dct4_q31);
}
static int DSP_dct4_q15(void)
{
    uint16_t i;
    /* clang-format off */
    /* clang-format on */
    riscv_float_to_q15(dct4_testinput_f32_50hz_200Hz_q15,
                     dct4_testinput_q15_50hz_200Hz, 256);
    riscv_float_to_q15(dct4_testinput_f32_50hz_200Hz_q15,
                     dct4_testinput_q15_50hz_200Hz_ref, 256);
    fftSize = 128;

    riscv_cfft_radix4_instance_q15 S;
    riscv_rfft_instance_q15 SS;
    riscv_dct4_instance_q15 SSS;
    riscv_dct4_init_q15(&SSS, &SS, &S, fftSize, fftSize / 2, 0x1000);
    BENCH_START(riscv_dct4_q15);
    riscv_dct4_q15(&SSS, q15_state, dct4_testinput_q15_50hz_200Hz);
    BENCH_END(riscv_dct4_q15);
    // riscv_dct4_init_q15(&SSS,&SS,&S,fftSize,fftSize/2,0x1000);
    // ref_dct4_q15(&SSS, q15_state, dct4_testinput_q15_50hz_200Hz_ref);
    q15_t resault, resault_ref;
    uint32_t index, index_ref = 5;
    riscv_shift_q15(dct4_testinput_q15_50hz_200Hz, 6,
                  dct4_testinput_q15_50hz_200Hz, fftSize);
    riscv_max_q15(dct4_testinput_q15_50hz_200Hz, fftSize, &resault, &index);
    //  riscv_shift_q15(dct4_testinput_q15_50hz_200Hz_ref,6,dct4_testinput_q15_50hz_200Hz_ref,fftSize);
    // riscv_max_q15(dct4_testinput_q15_50hz_200Hz_ref,fftSize,&resault,&index_ref);
    if (index != index_ref) {
        BENCH_ERROR(riscv_dct4_q15);
        printf("expect: %d, actual: %d\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_dct4_q15);
}
int main()
{
    BENCH_INIT;
    DSP_dct4_f32();
    DSP_dct4_q31();
    DSP_dct4_q15();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
