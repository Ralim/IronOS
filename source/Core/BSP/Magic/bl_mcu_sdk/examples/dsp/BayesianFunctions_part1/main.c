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

/* 
Those parameters can be generated with the python library scikit-learn.
*/
riscv_gaussian_naive_bayes_instance_f32 S;

#define NB_OF_CLASSES 3
#define VECTOR_DIMENSION 2

const float32_t theta[NB_OF_CLASSES*VECTOR_DIMENSION] = {1.525400412246743, 1.1286029390034396, -1.5547934054920127, 0.9976789084710983, -0.14944114212609047, -3.0461975645866857}; /**< Mean values for the Gaussians */

const float32_t sigma[NB_OF_CLASSES*VECTOR_DIMENSION] = {0.8790413342380022, 1.0687689375338891, 0.9512598687040021, 0.9514543522981638, 1.0755900156857314, 0.9088718773623725}; /**< Variances for the Gaussians */

const float32_t classPriors[NB_OF_CLASSES] = {0.3333333333333333f, 0.3333333333333333f, 0.3333333333333333f}; /**< Class prior probabilities */

int32_t main(void)
{
    BENCH_INIT;
    /* Array of input data */
    float32_t in[2];

    /* Result of the classifier */
    float32_t result[NB_OF_CLASSES];
    float32_t pBufferB[NB_OF_CLASSES];
    float32_t maxProba;
    uint32_t index;
    
    S.vectorDimension = VECTOR_DIMENSION; 
    S.numberOfClasses = NB_OF_CLASSES; 
    S.theta = theta;          
    S.sigma = sigma;         
    S.classPriors = classPriors;    
    S.epsilon=4.731813637210494e-09f; 

    in[0] = 1.5f;
    in[1] = 1.0f;
    BENCH_START(riscv_gaussian_naive_bayes_predict_f32);
    riscv_gaussian_naive_bayes_predict_f32(&S, in, result, pBufferB);
    BENCH_END(riscv_gaussian_naive_bayes_predict_f32);
    riscv_max_f32(result, NB_OF_CLASSES, &maxProba, &index);
    /* Index should be 0 : First class */
     for(int i=0;i<NB_OF_CLASSES;i++)
         printf("%f,",result[i]);
    if (index != 0) {
        BENCH_ERROR(riscv_gaussian_naive_bayes_predict_f32);
        printf("expect: %d, actual: %d\n", 0, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_gaussian_naive_bayes_predict_f32);


    in[0] = -1.5f;
    in[1] = 1.0f;
    BENCH_START(riscv_gaussian_naive_bayes_predict_f32);
    riscv_gaussian_naive_bayes_predict_f32(&S, in, result, pBufferB);
    BENCH_END(riscv_gaussian_naive_bayes_predict_f32);
    riscv_max_f32(result, NB_OF_CLASSES, &maxProba, &index);
    /* Index should be 1 : First class */
    if (index != 1) {
        BENCH_ERROR(riscv_gaussian_naive_bayes_predict_f32);
        printf("expect: %d, actual: %d\n", 1, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_gaussian_naive_bayes_predict_f32);

    in[0] = 0.0f;
    in[1] = -3.0f;
    BENCH_START(riscv_gaussian_naive_bayes_predict_f32);
    riscv_gaussian_naive_bayes_predict_f32(&S, in, result, pBufferB);
    BENCH_END(riscv_gaussian_naive_bayes_predict_f32);
    riscv_max_f32(result, NB_OF_CLASSES, &maxProba, &index);
    /* Index should be 2 : First class */
    if (index != 2) {
        BENCH_ERROR(riscv_gaussian_naive_bayes_predict_f32);
        printf("expect: %d, actual: %d\n", 2, index);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_gaussian_naive_bayes_predict_f32);

    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}