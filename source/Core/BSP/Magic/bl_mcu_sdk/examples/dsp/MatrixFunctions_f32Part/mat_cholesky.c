#include "ref.h"

#include "dsp/matrix_functions.h"

riscv_status ref_mat_cholesky_f32(
  const riscv_matrix_instance_f32 * pSrc,
        riscv_matrix_instance_f32 * pDst)
{

  riscv_status status;                             /* status of matrix inverse */

  {
    int i,j,k;
    int n = pSrc->numRows;
    float32_t invSqrtVj;
    float32_t *pA,*pG;

    pA = pSrc->pData;
    pG = pDst->pData;
    

    for(i=0 ; i < n ; i++)
    {
       for(j=i ; j < n ; j++)
       {
          pG[j * n + i] = pA[j * n + i];

          for(k=0; k < i ; k++)
          {
             pG[j * n + i] = pG[j * n + i] - pG[i * n + k] * pG[j * n + k];
          }
       }

       if (pG[i * n + i] <= 0.0f)
       {
         return(RISCV_MATH_DECOMPOSITION_FAILURE);
       }

       invSqrtVj = 1.0f/sqrtf(pG[i * n + i]);
       for(j=i ; j < n ; j++)
       {
         pG[j * n + i] = pG[j * n + i] * invSqrtVj ;
       }
    }

    status = RISCV_MATH_SUCCESS;

  }

  
  /* Return to application */
  return (status);
}

riscv_status ref_mat_cholesky_f64(
  const riscv_matrix_instance_f64 * pSrc,
        riscv_matrix_instance_f64 * pDst)
{
  riscv_status status;                             /* status of matrix inverse */

  {
    int i,j,k;
    int n = pSrc->numRows;
    float64_t invSqrtVj;
    float64_t *pA,*pG;

    pA = pSrc->pData;
    pG = pDst->pData;
    

    for(i=0 ; i < n ; i++)
    {
       for(j=i ; j < n ; j++)
       {
          pG[j * n + i] = pA[j * n + i];

          for(k=0; k < i ; k++)
          {
             pG[j * n + i] = pG[j * n + i] - pG[i * n + k] * pG[j * n + k];
          }
       }

       if (pG[i * n + i] <= 0.0f)
       {
         return(RISCV_MATH_DECOMPOSITION_FAILURE);
       }

       invSqrtVj = 1.0/sqrt(pG[i * n + i]);
       for(j=i ; j < n ; j++)
       {
         pG[j * n + i] = pG[j * n + i] * invSqrtVj ;
       }
    }

    status = RISCV_MATH_SUCCESS;

  }

  
  /* Return to application */
  return (status);
}