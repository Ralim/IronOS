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

    BENCH_INIT;
    //simulate z=3.1*x + 7.2*y + 1.25
    riscv_bilinear_interp_instance_f32 S_f32 = {
        20,                 /*number of Rows*/
        20,                 /*number of Columns*/
        f32_z_array,        /*value of y*/
    };
    float32_t x_f32_input = 5.12, y_f32_input = 6.24, z_f32_output, z_f32_output_ref = 62.0500000000000;
    BENCH_START(riscv_bilinear_interp_f32);
    z_f32_output = riscv_bilinear_interp_f32(&S_f32, x_f32_input, y_f32_input);
    BENCH_END(riscv_bilinear_interp_f32);
    if (fabs(z_f32_output - z_f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_bilinear_interp_f32);
        printf("expect: %f, actual: %f\n", z_f32_output_ref, z_f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_bilinear_interp_f32);

    //simulate z=47120*x + 39675*y + 96200
    //stands for z=0.000022*x + 0.000018*y + 0.000045
    //test:x:5.12 y:6.24
    riscv_bilinear_interp_instance_q31 S_q31 = {
        20,                  /*number of Rows*/
        20,                  /*number of Columns*/
        q31_z_array,         /*value of y*/
    };
    q31_t x_q31_input = 5305794, y_q31_input = 6417285, z_q31_output, z_q31_output_ref = 579735;
    BENCH_START(riscv_bilinear_interp_q31);
    z_q31_output = riscv_bilinear_interp_q31(&S_q31, x_q31_input, y_q31_input);
    BENCH_END(riscv_bilinear_interp_q31);
    riscv_q31_to_float(&z_q31_output, &z_f32_output, 1);
    riscv_q31_to_float(&z_q31_output_ref, &z_f32_output_ref, 1);
    if (fabs(z_f32_output - z_f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_bilinear_interp_q31);
        printf("expect: %f, actual: %f\n", z_f32_output_ref, z_f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_bilinear_interp_q31);

    //simulate z=41*x + 205*y + 328
    //stands for z=0.00125*x + 0.00625*y + 0.01
    //test:x:5.12 y:6.24
    riscv_bilinear_interp_instance_q15 S_q15 = {
        20,                   /*number of Rows*/
        20,                   /*number of Columns*/
        q15_z_array,          /*value of y*/
    };
    q31_t x_q15_input = 5305794, y_q15_input = 6417285; 
    q15_t z_q15_output, z_q15_output_ref = 1815;
    BENCH_START(riscv_bilinear_interp_q15);
    z_q15_output = riscv_bilinear_interp_q15(&S_q15, x_q15_input, y_q15_input);
    BENCH_END(riscv_bilinear_interp_q15);
    riscv_q15_to_float(&z_q15_output, &z_f32_output, 1);
    riscv_q15_to_float(&z_q15_output_ref, &z_f32_output_ref, 1);
    if (fabs(z_f32_output - z_f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_bilinear_interp_q15);
        printf("expect: %f, actual: %f\n", z_f32_output_ref, z_f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_bilinear_interp_q15);

    //simulate z=16*x + 3*y + 1
    //stands for z=0.125*x + 0.025*y + 0.01
    //test:x:5.12 y:6.24
    riscv_bilinear_interp_instance_q7 S_q7 = {
        20,                   /*number of Rows*/
        20,                   /*number of Columns*/
        q7_z_array,           /*value of y*/
    };
    q31_t x_q7_input = 5305794, y_q7_input = 6417285; 
    q7_t z_q7_output, z_q7_output_ref = 103;
    BENCH_START(riscv_bilinear_interp_q7);
    z_q7_output = riscv_bilinear_interp_q7(&S_q7, x_q7_input, y_q7_input);
    BENCH_END(riscv_bilinear_interp_q7);
    riscv_q7_to_float(&z_q7_output, &z_f32_output, 1);
    riscv_q7_to_float(&z_q7_output_ref, &z_f32_output_ref, 1);
    if (fabs(z_f32_output - z_f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_bilinear_interp_q7);
        printf("expect: %f, actual: %f\n", z_f32_output_ref, z_f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_bilinear_interp_q7);

    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
};
