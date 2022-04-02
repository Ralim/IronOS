#include "ref.h"

riscv_status ref_mat_solve_upper_triangular_f32(
  const riscv_matrix_instance_f32 * ut,
  const riscv_matrix_instance_f32 * a,
  riscv_matrix_instance_f32 * dst)
{
riscv_status status;                             /* status of matrix inverse */
  {
    int i,j,k,n;

    n = dst->numRows;

    float32_t *pX = dst->pData;
    float32_t *pUT = ut->pData;
    float32_t *pA = a->pData;

    float32_t *ut_row;
    float32_t *a_col;

    for(j=0; j < n; j ++)
    {
       a_col = &pA[j];

       for(i=n-1; i >= 0 ; i--)
       {
            ut_row = &pUT[n*i];

            float32_t tmp=a_col[i * n];
            
            for(k=n-1; k > i; k--)
            {
                tmp -= ut_row[k] * pX[n*k+j];
            }

            if (ut_row[i]==0.0f)
            {
              return(RISCV_MATH_SINGULAR);
            }
            tmp = tmp / ut_row[i];
            pX[i*n+j] = tmp;
       }

    }
    status = RISCV_MATH_SUCCESS;

  }

  
  /* Return to application */
  return (status);
}

riscv_status ref_mat_solve_upper_triangular_f64(
  const riscv_matrix_instance_f64 * ut,
  const riscv_matrix_instance_f64 * a,
  riscv_matrix_instance_f64 * dst)
{
riscv_status status;                             /* status of matrix inverse */

  {

    int i,j,k,n;

    n = dst->numRows;

    float64_t *pX = dst->pData;
    float64_t *pUT = ut->pData;
    float64_t *pA = a->pData;

    float64_t *ut_row;
    float64_t *a_col;

    for(j=0; j < n; j ++)
    {
       a_col = &pA[j];

       for(i=n-1; i >= 0 ; i--)
       {
            ut_row = &pUT[n*i];

            float64_t tmp=a_col[i * n];
            
            for(k=n-1; k > i; k--)
            {
                tmp -= ut_row[k] * pX[n*k+j];
            }

            if (ut_row[i]==0.0f)
            {
              return(RISCV_MATH_SINGULAR);
            }
            tmp = tmp / ut_row[i];
            pX[i*n+j] = tmp;
       }

    }
    status = RISCV_MATH_SUCCESS;

  }

  
  /* Return to application */
  return (status);
}