// This contains SIN_COS , clarke, inv_clarke, park, inv_park and pid
// each one has it's own function.
// All function can be found in main function.
// If you don't want to use it, then comment it.
#include "riscv_math.h"
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

int test_flag_error = 0;

static int DSP_SIN_COS_F32(void)
{
    int16_t i;
    float32_t pSinVal, pSinVal_ref;
    float32_t pCosVal, pCosVal_ref;

    BENCH_START(riscv_sin_cos_f32);
    for (int i = 0; i < 1000; i++)
        riscv_sin_cos_f32(0, &pSinVal, &pCosVal);
    BENCH_END(riscv_sin_cos_f32);
    ref_sin_cos_f32(0, &pSinVal_ref, &pCosVal_ref);
    if ((fabs(pSinVal - pSinVal_ref) > DELTAF32) ||
        (fabs(pCosVal - pCosVal_ref) > DELTAF32)) {
        BENCH_ERROR(riscv_sin_cos_f32);
        printf("sin expect: %f, actual: %f\ncos expect: %f, actual: %f",
               pSinVal_ref, pSinVal, pCosVal_ref, pCosVal);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_sin_cos_f32);
}

static int DSP_SIN_COS_Q31(void)
{
    int16_t i;
    q31_t pSinVal, pSinVal_ref;
    q31_t pCosVal, pCosVal_ref;

    BENCH_START(riscv_sin_cos_q31);
    for (int i = 0; i < 1000; i++)
        riscv_sin_cos_q31(0, &pSinVal, &pCosVal);
    BENCH_END(riscv_sin_cos_q31);
    ref_sin_cos_q31(0, &pSinVal_ref, &pCosVal_ref);
    if ((labs(pSinVal - pSinVal_ref) > DELTAQ31) ||
        (labs(pCosVal - pCosVal_ref) > DELTAQ31)) {
        BENCH_ERROR(riscv_sin_cos_q31);
        printf("sin expect: %x, actual: %x\ncos expect: %x, actual: %x",
               pSinVal_ref, pSinVal, pCosVal_ref, pCosVal);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_sin_cos_q31);
}

int main()
{
    BENCH_INIT;
    DSP_SIN_COS_F32();
    DSP_SIN_COS_Q31();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}