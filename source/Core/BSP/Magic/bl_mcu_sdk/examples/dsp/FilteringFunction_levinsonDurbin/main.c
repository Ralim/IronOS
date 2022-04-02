//
// Created by lujun on 19-6-28.
//
// This contains IIR, iir and lms filters
// each one has it's own function.
// All function can be found in main function.
// If you don't want to use it, then comment it.
//
// This part only tests result after 50 dots.If you want to test results' all
// dots, please define  WITH_FRONT
//
// If there are too much numbers which makes it hard to read,in VScode you can
// press alt and z can shrimp them to one line
//
// You MUST be careful about overflow.
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
#define SNR_THRESHOLD_F32 (80.0f)

int test_flag_error = 0;

//***************************************************************************************
//				iir		Lattice Filters
//***************************************************************************************
static void DSP_levinson_durbin_f32(void)
{
    int nbCoefs = 180;
    float32_t phi[256];
    float32_t a[256], a_ref[256];
    float32_t err, err_ref;
    for(int i=0;i<256;i++)
        phi[i] = (float32_t) rand();
    BENCH_START(riscv_levinson_durbin_f32);
    riscv_levinson_durbin_f32(&phi, &a, &err, nbCoefs);
    BENCH_END(riscv_levinson_durbin_f32);
    ref_levinson_durbin_f32(&phi, &a_ref, &err_ref, nbCoefs);
    // ScaleValue = 0.052219514664161221f * 0.04279801741658381f;
    for(int i=0;i<nbCoefs;i++)
        if (a_ref[i] != a[i]) {
            BENCH_ERROR(riscv_levinson_durbin_f32);
            printf("evinson_durbin a failed, index: %d, expect: %f, actual: %f\n", i, a_ref[i], a[i]);
            test_flag_error = 1;
        }
    if (err_ref != err) {
        BENCH_ERROR(riscv_levinson_durbin_f32);
        printf("evinson_durbin err failed, expect: %f, actual: %f\n", err_ref, err);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_levinson_durbin_f32);
}

static void DSP_levinson_durbin_q31(void)
{
    int nbCoefs = 180;
    q31_t phi[256];
    q31_t a[256]={0}, a_ref[256]={0};
    q31_t err=0, err_ref=0;
    for(int i=0;i<256;i++)
        phi[i] = (q31_t) rand();
    BENCH_START(riscv_levinson_durbin_q31);
    riscv_levinson_durbin_q31(&phi, &a, &err, nbCoefs);
    BENCH_END(riscv_levinson_durbin_q31);
    ref_levinson_durbin_q31(&phi, &a_ref, &err_ref, nbCoefs);
    // ScaleValue = 0.052219514664161221f * 0.04279801741658381f;
    for(int i=0;i<nbCoefs;i++)
        if (a_ref[i] != a[i]) {
            BENCH_ERROR(riscv_levinson_durbin_q31);
            printf("evinson_durbin a failed, index: %d, expect: %d, actual: %d\n", i, a_ref[i], a[i]);
            test_flag_error = 1;
        }
    if (err_ref != err) {
        BENCH_ERROR(riscv_levinson_durbin_q31);
        printf("evinson_durbin err failed, expect: %x, actual: %x\n", err_ref, err);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_levinson_durbin_q31);
}

int main()
{
    BENCH_INIT;
    DSP_levinson_durbin_f32();
    DSP_levinson_durbin_q31();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
