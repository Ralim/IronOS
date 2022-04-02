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
    float32_t f32_output, f32_output_ref = 0.211678832116788;
    BENCH_INIT;
    //1-2( sun(min(Sai,Sbi)) / (sum(Sai)+sum(Sbi)) )
    BENCH_START(riscv_braycurtis_distance_f32);
    f32_output = riscv_braycurtis_distance_f32(f32_braycurtis_a_array, f32_braycurtis_b_array, 10);
    BENCH_END(riscv_braycurtis_distance_f32);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_braycurtis_distance_f32);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_braycurtis_distance_f32);

    f32_output_ref = 3.48408251169646;
    //sum(|pi-qi|/(|pi|+|qi|))
    BENCH_START(riscv_canberra_distance_f32);
    f32_output = riscv_canberra_distance_f32(f32_canberra_a_array, f32_canberra_b_array, 10);
    BENCH_END(riscv_canberra_distance_f32);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_canberra_distance_f32);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_canberra_distance_f32);

    f32_output_ref = 0.887821857126397;
    //x12=max(|x1i-x2i|)
    BENCH_START(riscv_chebyshev_distance_f32);
    f32_output = riscv_chebyshev_distance_f32(f32_chebyshev_a_array, f32_chebyshev_b_array, 10);
    BENCH_END(riscv_chebyshev_distance_f32);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_chebyshev_distance_f32);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_chebyshev_distance_f32);

    f32_output_ref = 2.93344596055701;
    //also known as Manhattan Distance
    //x12=sum(|x1i-x2i|)
    BENCH_START(riscv_cityblock_distance_f32);
    f32_output = riscv_cityblock_distance_f32(f32_cityblock_a_array, f32_cityblock_b_array, 10);
    BENCH_END(riscv_cityblock_distance_f32);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_cityblock_distance_f32);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_cityblock_distance_f32);

    f32_output_ref = 0.213739159949741;
    //cos(sita)=a.b/|a||b|=sum(x1i*x2i)/ ( sum(abs(x1i)) * sum(abs(x1i)) )
    //actually it outputs 1-cos(sita)
    BENCH_START(riscv_cosine_distance_f32);
    f32_output = riscv_cosine_distance_f32(f32_cosine_a_array, f32_cosine_b_array, 10);
    BENCH_END(riscv_cosine_distance_f32);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_cosine_distance_f32);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_cosine_distance_f32);

    f32_output_ref = 1.28189071468851;
    //sqrt(sum(square(x1i-x2i)))
    BENCH_START(riscv_euclidean_distance_f32);
    f32_output = riscv_euclidean_distance_f32(f32_euclidean_a_array, f32_euclidean_b_array, 10);
    BENCH_END(riscv_euclidean_distance_f32);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_euclidean_distance_f32);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_euclidean_distance_f32);


    f32_output_ref = 0.733638397638707;
    //sqrt(sum(square(x1i-x2i)))
    BENCH_START(riscv_jensenshannon_distance_f32);
    f32_output = riscv_jensenshannon_distance_f32(f32_jensenshannon_a_array, f32_jensenshannon_b_array, 10);
    BENCH_END(riscv_jensenshannon_distance_f32);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_jensenshannon_distance_f32);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_jensenshannon_distance_f32);

    f32_output_ref = 1.04218658878953;
    //(sum(|xi-yi|p))1/p
    //p and 1/p stands for power p and power 1/p
    BENCH_START(riscv_minkowski_distance_f32);
    f32_output = riscv_minkowski_distance_f32(f32_minkowski_a_array, f32_minkowski_b_array, 3, 10);
    BENCH_END(riscv_minkowski_distance_f32);
    if (fabs(f32_output - f32_output_ref) > DELTAF32) {
        BENCH_ERROR(riscv_minkowski_distance_f32);
        printf("expect: %f, actual: %f\n", f32_output_ref, f32_output);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_minkowski_distance_f32);
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
};
