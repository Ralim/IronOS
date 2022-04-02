#include "ref.h"

riscv_status ref_mat_solve_lower_triangular_f32(
  const riscv_matrix_instance_f32 * lt,
  const riscv_matrix_instance_f32 * a,
  riscv_matrix_instance_f32 * dst)
  {
  riscv_status status;                             /* status of matrix inverse */
  {
    /* a1 b1 c1   x1 = a1
          b2 c2   x2   a2
             c3   x3   a3

    x3 = a3 / c3 
    x2 = (a2 - c2 x3) / b2

    */
    int i,j,k,n;

    n = dst->numRows;

    float32_t *pX = dst->pData;
    float32_t *pLT = lt->pData;
    float32_t *pA = a->pData;

    float32_t *lt_row;
    float32_t *a_col;

    for(j=0; j < n; j ++)
    {
       a_col = &pA[j];

       for(i=0; i < n ; i++)
       {
            lt_row = &pLT[n*i];

            float32_t tmp=a_col[i * n];
            
            for(k=0; k < i; k++)
            {
                tmp -= lt_row[k] * pX[n*k+j];
            }

            if (lt_row[i]==0.0f)
            {
              return(RISCV_MATH_SINGULAR);
            }
            tmp = tmp / lt_row[i];
            pX[i*n+j] = tmp;
       }

    }
    status = RISCV_MATH_SUCCESS;

  }

  /* Return to application */
  return (status);
}

  riscv_status ref_mat_solve_lower_triangular_f64(
  const riscv_matrix_instance_f64 * lt,
  const riscv_matrix_instance_f64 * a,
  riscv_matrix_instance_f64 * dst)
  {
  riscv_status status;                             /* status of matrix inverse */

  {
    /* a1 b1 c1   x1 = a1
          b2 c2   x2   a2
             c3   x3   a3

    x3 = a3 / c3 
    x2 = (a2 - c2 x3) / b2

    */
    int i,j,k,n;

    n = dst->numRows;

    float64_t *pX = dst->pData;
    float64_t *pLT = lt->pData;
    float64_t *pA = a->pData;

    float64_t *lt_row;
    float64_t *a_col;

    for(j=0; j < n; j ++)
    {
       a_col = &pA[j];

       for(i=0; i < n ; i++)
       {
            lt_row = &pLT[n*i];

            float64_t tmp=a_col[i * n];
            
            for(k=0; k < i; k++)
            {
                tmp -= lt_row[k] * pX[n*k+j];
            }

            if (lt_row[i]==0.0f)
            {
              return(RISCV_MATH_SINGULAR);
            }
            tmp = tmp / lt_row[i];
            pX[i*n+j] = tmp;
       }

    }
    status = RISCV_MATH_SUCCESS;

  }

  /* Return to application */
  return (status);
}