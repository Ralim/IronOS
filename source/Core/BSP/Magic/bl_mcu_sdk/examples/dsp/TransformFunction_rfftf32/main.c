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

static int DSP_rfft_fast_f32(void)
{
    uint16_t i;
    /* clang-format off */
    riscv_rfft_fast_instance_f32 SS;
    /* clang-format on */
    riscv_rfft_fast_init_f32(&SS, 512);
    BENCH_START(riscv_rfft_fast_f32);
    riscv_rfft_fast_f32(&SS, rfft_testinput_f32_50hz_200Hz_fast, f32_testOutput, 0);
    BENCH_END(riscv_rfft_fast_f32);
    ref_rfft_fast_f32(&SS, rfft_testinput_f32_50hz_200Hz_fast_ref,
                      f32_testOutput_ref, 0);
    float32_t resault, resault_ref;
    uint32_t index, index_ref;
    riscv_max_f32(f32_testOutput, 512, &resault, &index);
    riscv_max_f32(f32_testOutput_ref, 512, &resault_ref, &index_ref);

    if (index != index_ref) {
        BENCH_ERROR(riscv_rfft_fast_f32);
        printf("expect: %d, actual: %d\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_rfft_fast_f32);
}
static int DSP_rfft_f32(void)
{
    /* clang-format off */
    uint16_t i, j;
    riscv_rfft_instance_f32 SS;
    riscv_cfft_radix4_instance_f32 S_CFFT;
    /* clang-format on */
    fftSize = 512;
    ifftFlag = 0;
    doBitReverse = 1;
    riscv_cfft_radix4_init_f32(&S_CFFT, 256, ifftFlag, doBitReverse);
    riscv_rfft_init_f32(&SS, &S_CFFT, fftSize, ifftFlag, doBitReverse);
    BENCH_START(riscv_rfft_f32);
    riscv_rfft_f32(&SS, rfft_testinput_f32_50hz_200Hz, f32_testOutput);
    BENCH_END(riscv_rfft_f32);
    riscv_cfft_radix4_init_f32(&S_CFFT, 256, ifftFlag, doBitReverse);
    riscv_rfft_init_f32(&SS, &S_CFFT, fftSize, ifftFlag, doBitReverse);
    ref_rfft_f32(&SS, rfft_testinput_f32_50hz_200Hz_ref, f32_testOutput_ref);
    float32_t resault, resault_ref;
    uint32_t index, index_ref;
    riscv_max_f32(f32_testOutput, fftSize, &resault, &index);
    riscv_max_f32(f32_testOutput_ref, fftSize, &resault_ref, &index_ref);
    if (index != index_ref) {
        BENCH_ERROR(riscv_rfft_f32);
        printf("expect: %d, actual: %d\n", index_ref, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_rfft_f32);
}

int main()
{
    BENCH_INIT;
    DSP_rfft_fast_f32();
    DSP_rfft_f32();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
