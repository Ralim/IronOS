// This contains SIN_COS , clarke, inv_clarke, park, inv_park and pid
// each one has it's own function.
// All function can be found in main function.
// If you don't want to use it, then comment it.
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

#define ENABLE_ALL
#define CLARKE
#define INV_CLARKE
#define PARK
#define INV_PARK
#define PID

int test_flag_error = 0;

static int DSP_clarke_f32_app()
{
    float32_t Ia = sin(30 * PI / 180);
    float32_t Ib = sin(150 * PI / 180);

    float32_t pIalpha, pIalpha_ref;
    float32_t pIbeta, pIbeta_ref;

    BENCH_START(riscv_clarke_f32);
    for (int i = 0; i < 1000; i++)
        riscv_clarke_f32(Ia, Ib, &pIalpha, &pIbeta);
    BENCH_END(riscv_clarke_f32);
    pIalpha_ref = 0.500000;
    pIbeta_ref = 0.866025;
    if ((fabs(pIalpha - pIalpha_ref) > DELTAF32) ||
        (fabs(pIbeta - pIbeta_ref) > DELTAF32)) {
        BENCH_ERROR(riscv_clarke_f32);
        printf("pIalpha expect: %f, actual: %f\npIbeta expect: %f, actual: %f",
               pIalpha_ref, pIalpha, pIbeta_ref, pIbeta);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_clarke_f32);
}

static int DSP_clarke_q31_app()
{

    const float32_t Ia = sin(30 * PI / 180);
    const float32_t Ib = sin(150 * PI / 180);

    q31_t Ia1;
    q31_t Ib1;

    riscv_float_to_q31(&Ia, &Ia1, 1);
    riscv_float_to_q31(&Ib, &Ib1, 1);

    q31_t pIalpha, pIalpha_ref;
    q31_t pIbeta, pIbeta_ref;

    BENCH_START(riscv_clarke_q31);
    for (int i = 0; i < 1000; i++)
        riscv_clarke_q31(Ia1, Ib1, &pIalpha, &pIbeta);
    BENCH_END(riscv_clarke_q31);
    pIalpha_ref = 1073741824;
    pIbeta_ref = 1859774949;
    if ((labs(pIalpha - pIalpha_ref) > DELTAQ31) ||
        (labs(pIbeta - pIbeta_ref) > DELTAQ31)) {
        BENCH_ERROR(riscv_clarke_q31);
        printf("pIalpha expect: %x, actual: %x\npIbeta expect: %x, actual: %x",
               pIalpha_ref, pIalpha, pIbeta_ref, pIbeta);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_clarke_q31);
}

static int DSP_inv_clarke_f32_app()
{
    float32_t Ia, Ia_ref;
    float32_t Ib, Ib_ref;

    float32_t pIalpha = 0.500000;
    float32_t pIbeta = 0.866025;
    BENCH_START(riscv_inv_clarke_f32);
    for (int i = 0; i < 1000; i++)
        riscv_inv_clarke_f32(pIalpha, pIbeta, &Ia, &Ib);
    BENCH_END(riscv_inv_clarke_f32);
    Ia_ref = 0.500000;
    Ib_ref = 0.500000;
    if ((fabs(Ia - Ia_ref) > DELTAF32) || (fabs(Ib - Ib_ref) > DELTAF32)) {
        BENCH_ERROR(riscv_inv_clarke_f32);
        printf("Ia expect: %f, actual: %f\nIb expect: %f, actual: %f", Ia_ref,
               Ia, Ib_ref, Ib);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_inv_clarke_f32);
}

static int DSP_inv_clarke_q31_app()
{
    q31_t Ia, Ia_ref;
    q31_t Ib, Ib_ref;

    float32_t pIalpha = 0.500000;
    float32_t pIbeta = 0.866025;

    q31_t pIalpha1;
    q31_t pIbeta1;

    riscv_float_to_q31(&pIalpha, &pIalpha1, 1);
    riscv_float_to_q31(&pIalpha, &pIbeta1, 1);

    BENCH_START(riscv_inv_clarke_q31);
    for (int i = 0; i < 1000; i++)
        riscv_inv_clarke_q31(pIalpha1, pIbeta1, &Ia, &Ib);
    BENCH_END(riscv_inv_clarke_q31);
    Ia_ref = 1073741824;
    Ib_ref = 393016784;
    if ((labs(Ia - Ia_ref) > DELTAQ31) || (labs(Ib - Ib_ref) > DELTAQ31)) {
        BENCH_ERROR(riscv_inv_clarke_q31);
        printf("Ia expect: %x, actual: %x\nIb expect: %x, actual: %x", Ia_ref,
               Ia, Ib_ref, Ib);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_inv_clarke_q31);
}

static int DSP_park_f32_app()
{
    float32_t Ia = sin(30 * PI / 180);
    float32_t Ib = sin(120 * PI / 180);

    float32_t pIalpha;
    float32_t Ibeta;

    float32_t pId, pId_ref;
    float32_t pIq, pIq_ref;

    float32_t sinVal = sin(30 * PI / 180);
    float32_t cosVal = cos(30 * PI / 180);

    BENCH_START(riscv_park_f32);
    for (int i = 0; i < 1000; i++)
        riscv_park_f32(Ia, Ib, &pId, &pIq, sinVal, cosVal);
    BENCH_END(riscv_park_f32);
    pId_ref = 0.866025;
    pIq_ref = 0.500000;
    if ((fabs(pId - pId_ref) > DELTAF32) || (fabs(pIq - pIq_ref) > DELTAF32)) {
        BENCH_ERROR(riscv_park_f32);
        printf("pId expect: %f, actual: %f\npIq expect: %f, actual: %f",
               pId_ref, pId, pIq_ref, pIq);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_park_f32);
}

static int DSP_park_q31_app()
{

    const float32_t Ia = sin(30 * PI / 180);
    const float32_t Ib = sin(150 * PI / 180);

    q31_t Ia1;
    q31_t Ib1;

    riscv_float_to_q31(&Ia, &Ia1, 1);
    riscv_float_to_q31(&Ib, &Ib1, 1);

    q31_t pIalpha;
    q31_t pIbeta;

    q31_t pId, pId_ref;
    q31_t pIq, pIq_ref;

    float32_t sinVal = sin(30 * PI / 180);
    float32_t cosVal = cos(30 * PI / 180);

    q31_t sinVal1;
    q31_t cosVal1;

    riscv_float_to_q31(&sinVal, &sinVal1, 1);
    riscv_float_to_q31(&cosVal, &cosVal1, 1);

    BENCH_START(riscv_park_q31);
    for (int i = 0; i < 1000; i++)
        riscv_park_q31(Ia1, Ib1, &pId, &pIq, sinVal1, cosVal1);
    BENCH_END(riscv_park_q31);
    pId_ref = 1466758400;
    pIq_ref = 393016435;
    if ((labs(pId - pId_ref) > DELTAQ31) || (labs(pIq - pIq_ref) > DELTAQ31)) {
        BENCH_ERROR(riscv_park_q31);
        printf("pId expect: %x, actual: %x\npIq expect: %x, actual: %x",
               pId_ref, pId, pIq_ref, pIq);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_park_q31);
}

static int DSP_inv_park_f32_app()
{
    float32_t Ia = 0.866025;
    float32_t Ib = 0.500000;

    float32_t pIalpha, pIalpha_ref;
    float32_t pIbeta, pIbeta_ref;

    float32_t sinVal = sin(30 * PI / 180);
    float32_t cosVal = cos(30 * PI / 180);

    BENCH_START(riscv_inv_park_f32);
    for (int i = 0; i < 1000; i++)
        riscv_inv_park_f32(Ia, Ib, &pIalpha, &pIbeta, sinVal, cosVal);
    BENCH_END(riscv_inv_park_f32);
    pIalpha_ref = 0.500000;
    pIbeta_ref = 0.866025;
    if ((fabs(pIalpha - pIalpha_ref) > DELTAF32) ||
        (fabs(pIbeta - pIbeta_ref) > DELTAF32)) {
        BENCH_ERROR(riscv_inv_park_f32);
        printf("pIalpha expect: %f, actual: %f\npIbeta expect: %f, actual: %f",
               pIalpha_ref, pIalpha, pIbeta_ref, pIbeta);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_inv_park_f32);
}

static int DSP_inv_park_q31_app()
{
    q31_t Ia1 = 1859774464;
    q31_t Ib1 = 1073741824;

    q31_t pIalpha, pIalpha_ref;
    q31_t pIbeta, pIbeta_ref;

    q31_t sinValb = 1073741824;
    q31_t cosValb = 1859775360;

    BENCH_START(riscv_inv_park_q31);
    for (int i = 0; i < 1000; i++)
        riscv_inv_park_q31(Ia1, Ib1, &pIalpha, &pIbeta, sinValb, cosValb);
    BENCH_END(riscv_inv_park_q31);
    pIalpha_ref = 1073740990;
    pIbeta_ref = 1859774912;
    if ((labs(pIalpha - pIalpha_ref) > DELTAQ31) ||
        (labs(pIbeta - pIbeta_ref) > DELTAQ31)) {
        BENCH_ERROR(riscv_inv_park_q31);
        printf(
            "pIalpha expect: %x, actual: %x\npIbeta expect: %x, actual: %x\n",
            pIalpha_ref, pIalpha, pIbeta_ref, pIbeta);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_inv_park_q31);
}

static int DSP_pid_f32_app()
{
    uint32_t i;
    riscv_pid_instance_f32 PIDS;
    PIDS.Kp = 0.4;
    PIDS.Ki = 0.4;
    PIDS.Kd = 0;
    float32_t target, ival, ee = 0, ee_ref = 0;
    output[0] = 0;
    output_ref[0] = 0;
    /* Target value*/
    target = 500;
    /* Inital value */
    ival = 0;
    /* Initial value and target value error */
    ee = target - ival;
    ee_ref = target - ival;
    /* Initial DSP PID controller function*/
    riscv_pid_init_f32(&PIDS, 1);
    BENCH_START(riscv_pid_f32);
    for (i = 1; i < 100; i++) {
        output[i] = riscv_pid_f32(&PIDS, ee);
        ee = target - output[i - 1];
    }
    BENCH_END(riscv_pid_f32);
    for (i = 1; i < 100; i++) {
        output_ref[i] = ref_pid_f32(&PIDS, ee_ref);
        ee_ref = target - output_ref[i - 1];
    }
    if (fabs(output[99] - output_ref[99]) > DELTAF32) {
        BENCH_ERROR(riscv_pid_f32);
        printf("index: %d, expect: %f, actual: %f\n", i, output_ref[99],
               output[99]);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_pid_f32);
}

static int DSP_pid_q15_app()
{
    uint32_t i;
    riscv_pid_instance_f32 PIDS;
    riscv_pid_instance_q15 PIDS_q15;
    PIDS.Kp = 0.4;
    PIDS.Ki = 0.4;
    PIDS.Kd = 0;
    riscv_float_to_q15(&PIDS.Kp, &PIDS_q15.Kp, 1);
    riscv_float_to_q15(&PIDS.Ki, &PIDS_q15.Ki, 1);
    riscv_float_to_q15(&PIDS.Kd, &PIDS_q15.Kd, 1);
    q15_t ee_q15 = 0, ee_q15_ref = 0,target, ival;
    output_q15[0] = 0;
    output_q15_ref[0] = 0;
    /* Target value*/
    target = 500;
    /* Inital value */
    ival = 0;
    /* Initial value and target value error */
    ee_q15 = target - ival;
    ee_q15_ref = target - ival;
    /* Initial DSP PID controller function*/
    riscv_pid_init_q15(&PIDS_q15, 0);
    BENCH_START(riscv_pid_q15);
    for (i = 1; i < 100; i++) {
        output_q15[i] = riscv_pid_q15(&PIDS_q15, ee_q15);
        ee_q15 = target - output_q15[i - 1];
    }
    BENCH_END(riscv_pid_q15);
    riscv_pid_init_q15(&PIDS_q15, 0);
    for (i = 1; i < 100; i++) {
        output_q15_ref[i] = ref_pid_q15(&PIDS_q15, ee_q15_ref);
        ee_q15_ref = target - output_q15_ref[i - 1];
    }
    if (abs(output_q15[99] - output_q15_ref[99]) > DELTAQ15) {
        BENCH_ERROR(riscv_pid_q15);
        printf("index: %d, expect: %x, actual: %x\n", 99, output_q15_ref[99],
               output_q15[99]);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_pid_q15);
}

static int DSP_pid_q31_app()
{
    uint32_t i;
    riscv_pid_instance_f32 PIDS;
    riscv_pid_instance_q31 PIDS_q31;
    PIDS.Kp = 0.4;
    PIDS.Ki = 0.4;
    PIDS.Kd = 0;
    riscv_float_to_q31(&PIDS.Kp, &PIDS_q31.Kp, 1);
    riscv_float_to_q31(&PIDS.Ki, &PIDS_q31.Ki, 1);
    riscv_float_to_q31(&PIDS.Kd, &PIDS_q31.Kd, 1);
    q31_t ee_q31 = 0, ee_q31_ref = 0,target, ival;
    output_q31[0] = 0;
    output_q31_ref[0] = 0;
    /* Target value*/
    target = 500;
    /* Inital value */
    ival = 0;
    /* Initial value and target value error */
    ee_q31 = target - ival;
    /* Initial DSP PID controller function*/
    riscv_pid_init_q31(&PIDS_q31, 1);
    BENCH_START(riscv_pid_q31);
    for (i = 1; i < 100; i++) {
        output_q31[i] = riscv_pid_q31(&PIDS_q31, ee_q31);
        ee_q31 = target - output_q31[i - 1];
    }
    BENCH_END(riscv_pid_q31);
    for (i = 1; i < 100; i++) {
        output_q31_ref[i] = ref_pid_q31(&PIDS_q31, ee_q31_ref);
        ee_q31_ref = target - output_q31_ref[i - 1];
    }
    if (labs(output_q31[99] - output_q31_ref[99]) > DELTAQ31) {
        BENCH_ERROR(riscv_pid_q31);
        printf("index: %d, expect: %x, actual: %x\n", i, output_q31_ref[i],
               output_q31[i]);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_pid_q31);
}
int main()
{
    BENCH_INIT;
#if defined CLARKE || defined ENABLE_ALL
    DSP_clarke_f32_app();
    DSP_clarke_q31_app();
#endif
#if defined INV_CLARKE || defined ENABLE_ALL
    DSP_inv_clarke_f32_app();
    DSP_inv_clarke_q31_app();
#endif
#if defined PARK || defined ENABLE_ALL
    DSP_park_f32_app();
    DSP_park_q31_app();
#endif
#if defined INV_PARK || defined ENABLE_ALL
    DSP_inv_park_f32_app();
    DSP_inv_park_q31_app();
#endif
#if defined PID || defined ENABLE_ALL
    DSP_pid_f32_app();
    DSP_pid_q15_app();
    DSP_pid_q31_app();
#endif
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}