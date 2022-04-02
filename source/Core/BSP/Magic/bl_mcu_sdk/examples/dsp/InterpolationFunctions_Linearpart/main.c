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
    //simulate y=pi*x + pi/2
    riscv_linear_interp_instance_f32 S_f32 = {
        ARRAY_SIZE,       /*number of values*/
        0,                /*start value of x*/
        0.1,              /*x's spacing*/
        f32_y_array,      /*value of y*/
    };
    float32_t x_f32_input = 5.12, y_f32_output, y_f32_output_ref = 17.6557507131746;
    BENCH_START(riscv_linear_interp_f32);
    y_f32_output = riscv_linear_interp_f32(&S_f32, x_f32_input);
    BENCH_END(riscv_linear_interp_f32);
    if (fabs(y_f32_output - y_f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_linear_interp_f32);
        printf("expect: %f, actual: %f\n", y_f32_output_ref, y_f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_linear_interp_f32);


    //simulate y=0.00000125x+0.00000625
    //in q31 is y=2684x+13421 
    //the x_input is 3972.
    //x_q31_input is in 12.20 format. 12 for start position, 20 for format(calculate before).
    //first calculate the format, then shift right 12 bits. At last move expected index to the high 12 bits
    //this is 39 and 0.72
    q31_t x_q31_input = 41271951, y_q31_output, y_q31_output_ref = 10674269;
    BENCH_START(riscv_linear_interp_q31);
    y_q31_output = riscv_linear_interp_q31(q31_y_array, x_q31_input, ARRAY_SIZE);
    BENCH_END(riscv_linear_interp_q31);
    //change result to float32_t to compare
    riscv_q31_to_float(&y_q31_output, &y_f32_output, 1);
    riscv_q31_to_float(&y_q31_output_ref, &y_f32_output_ref, 1);
    if (fabs(y_f32_output - y_f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_linear_interp_q31);
        printf("expect: %f, actual: %f\n", y_f32_output_ref, y_f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_linear_interp_q31);

    //simulate y=0.000125x+0.000625
    //in q15 is y=4x+20
    //the x_input is 3972.
    //x_q15_input is in 12.20 format. 12 for start position, 20 for format(calculate before).
    //first calculate the format, then shift right 12 bits. At last move expected index to the high 12 bits
    //this is 39 and 0.72
    q31_t x_q15_input = 41271951;
    q15_t y_q15_output, y_q15_output_ref = 15908;
    BENCH_START(riscv_linear_interp_q31);
    y_q15_output = riscv_linear_interp_q15(q15_y_array, x_q15_input, ARRAY_SIZE);
    BENCH_END(riscv_linear_interp_q15);
    //change result to float32_t to compare
    riscv_q15_to_float(&y_q15_output, &y_f32_output, 1);
    riscv_q15_to_float(&y_q15_output_ref, &y_f32_output_ref, 1);
    if (fabs(y_f32_output - y_f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_linear_interp_q15);
        printf("expect: %f, actual: %f\n", y_f32_output_ref, y_f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_linear_interp_q15);

    //simulate y=0.02x+0.03
    //in q7 is y=2x+3
    //the x_input is 39.
    //x_q7_input is in 12.20 format. 12 for start position, 20 for format(calculate before).
    //first calculate the format, then shift right 12 bits. At last move expected index to the high 12 bits
    //this is 35 and 0.8
    q31_t x_q7_input = 37119590;
    q7_t y_q7_output, y_q7_output_ref = 81;
    BENCH_START(riscv_linear_interp_q31);
    y_q7_output = riscv_linear_interp_q7(q7_y_array, x_q7_input, 9);
    BENCH_END(riscv_linear_interp_q7);
    //change result to float32_t to compare
    riscv_q7_to_float(&y_q7_output, &y_f32_output, 1);
    riscv_q7_to_float(&y_q7_output_ref, &y_f32_output_ref, 1);
    if (fabs(y_f32_output - y_f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_linear_interp_q7);
        printf("expect: %f, actual: %f\n", y_f32_output_ref, y_f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_linear_interp_q7);

    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
};
