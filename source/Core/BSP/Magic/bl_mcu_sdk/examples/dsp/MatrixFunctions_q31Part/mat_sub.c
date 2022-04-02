#include "ref.h"
//#include "../HelperFunctions/ref_helper.c"

riscv_status ref_mat_sub_f32(const riscv_matrix_instance_f32 *pSrcA,
                           const riscv_matrix_instance_f32 *pSrcB,
                           riscv_matrix_instance_f32 *pDst)
{
    uint32_t i;
    uint32_t numSamples; /* total number of elements in the matrix  */

    /* Total number of samples in the input matrix */
    numSamples = (uint32_t)pSrcA->numRows * pSrcA->numCols;

    for (i = 0; i < numSamples; i++) {
        pDst->pData[i] = pSrcA->pData[i] - pSrcB->pData[i];
    }

    return RISCV_MATH_SUCCESS;
}

riscv_status ref_mat_sub_q31(const riscv_matrix_instance_q31 *pSrcA,
                           const riscv_matrix_instance_q31 *pSrcB,
                           riscv_matrix_instance_q31 *pDst)
{
    uint32_t i;
    uint32_t numSamples; /* total number of elements in the matrix  */

    /* Total number of samples in the input matrix */
    numSamples = (uint32_t)pSrcA->numRows * pSrcA->numCols;

    for (i = 0; i < numSamples; i++) {
        pDst->pData[i] = ref_sat_q31((q63_t)pSrcA->pData[i] - pSrcB->pData[i]);
    }

    return RISCV_MATH_SUCCESS;
}

riscv_status ref_mat_sub_q15(const riscv_matrix_instance_q15 *pSrcA,
                           const riscv_matrix_instance_q15 *pSrcB,
                           riscv_matrix_instance_q15 *pDst)
{
    uint32_t i;
    uint32_t numSamples; /* total number of elements in the matrix  */

    /* Total number of samples in the input matrix */
    numSamples = (uint32_t)pSrcA->numRows * pSrcA->numCols;

    for (i = 0; i < numSamples; i++) {
        pDst->pData[i] = ref_sat_q15((q31_t)pSrcA->pData[i] - pSrcB->pData[i]);
    }

    return RISCV_MATH_SUCCESS;
}
