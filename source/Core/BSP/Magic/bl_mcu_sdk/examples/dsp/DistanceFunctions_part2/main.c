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

int test_flag_error = 0;

int main()
{
    int i;

// Noticing that all the following functions are in Big-end!

    BENCH_INIT;
    float32_t f32_output, f32_output_ref =  0.282051282051282;
    //1.0*(ctf + cft) / (2.0*ctt + cft + ctf)
    //t stands for true,f stands for false
    BENCH_START(riscv_dice_distance);
    f32_output = riscv_dice_distance(u32_dice_a_array, u32_dice_b_array, 36);
    BENCH_END(riscv_dice_distance);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_dice_distance);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_dice_distance);

    f32_output_ref =  0.583333333333333;
    //1.0*(ctf + cft) / numberOfBools
    BENCH_START(riscv_hamming_distance);
    f32_output = riscv_hamming_distance(u32_hamming_a_array, u32_hamming_b_array, 36);
    BENCH_END(riscv_hamming_distance);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_hamming_distance);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_hamming_distance);

    f32_output_ref =  0.769230769230769;
    //1.0*(ctf + cft) / (ctt + cft + ctf)
    BENCH_START(riscv_jaccard_distance);
    f32_output = riscv_jaccard_distance(u32_jaccard_a_array, u32_jaccard_b_array, 36);
    BENCH_END(riscv_jaccard_distance);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_jaccard_distance);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_jaccard_distance);

    f32_output_ref =  0.894736842105263;
    //1.0*(ctf + cft - ctt + numberOfBools) / (cft + ctf + numberOfBools)
    BENCH_START(riscv_kulsinski_distance);
    f32_output = riscv_kulsinski_distance(u32_kulsinski_a_array, u32_kulsinski_b_array, 36);
    BENCH_END(riscv_kulsinski_distance);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_kulsinski_distance);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_kulsinski_distance);

    f32_output_ref =  0.615384615384615;
    //r = 2*(ctf + cft)
    //return(1.0*r / (r + ctt + cff))
    BENCH_START(riscv_rogerstanimoto_distance);
    f32_output = riscv_rogerstanimoto_distance(u32_rogerstanimoto_a_array, u32_rogerstanimoto_b_array, 36);
    BENCH_END(riscv_rogerstanimoto_distance);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_rogerstanimoto_distance);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_rogerstanimoto_distance);

    f32_output_ref =  0.833333333333333;
    //1.0*(numberOfBools - ctt) / ((float32_t)numberOfBools)
    BENCH_START(riscv_russellrao_distance);
    f32_output = riscv_russellrao_distance(u32_russellrao_a_array, u32_russellrao_b_array, 36);
    BENCH_END(riscv_russellrao_distance);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_russellrao_distance);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_russellrao_distance);

    f32_output_ref =  0.714285714285714;
    //r = 2.0*(ctf + cft);
    //s = 1.0*(cff + ctt);
    //r / (s+r)
    BENCH_START(riscv_sokalmichener_distance);
    f32_output = riscv_sokalmichener_distance(u32_sokalmichener_a_array, u32_sokalmichener_b_array, 36);
    BENCH_END(riscv_sokalmichener_distance);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_sokalmichener_distance);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_sokalmichener_distance);

    f32_output_ref =  0.790697674418605;
    //r = 2.0*(ctf + cft);
    //r / (r + ctt)
    BENCH_START(riscv_sokalsneath_distance);
    f32_output = riscv_sokalsneath_distance(u32_sokalsneath_a_array, u32_sokalsneath_b_array, 36);
    BENCH_END(riscv_sokalsneath_distance);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_sokalsneath_distance);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_sokalsneath_distance);

    f32_output_ref =  1.26760563380282;
    //r = 2*(ctf * cft);
    //1.0*r / (r/2.0 + ctt * cff)
    BENCH_START(riscv_yule_distance);
    f32_output = riscv_yule_distance(u32_yule_a_array, u32_yule_b_array, 36);
    BENCH_END(riscv_yule_distance);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_yule_distance);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_yule_distance);

    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
};
