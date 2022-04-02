#include "ref.h"

/// @private
#define SWAP_ROWS_F32(A,i,j)     \
  for(int w=0;w < n; w++)    \
  {                          \
     float32_t tmp;          \
     tmp = A[i*n + w];       \
     A[i*n + w] = A[j*n + w];\
     A[j*n + w] = tmp;       \
  }

/// @private
#define SWAP_COLS_F32(A,i,j)     \
  for(int w=0;w < n; w++)    \
  {                          \
     float32_t tmp;          \
     tmp = A[w*n + i];       \
     A[w*n + i] = A[w*n + j];\
     A[w*n + j] = tmp;       \
  }

riscv_status ref_mat_ldlt_f32(
  const riscv_matrix_instance_f32 * pSrc,
  riscv_matrix_instance_f32 * pl,
  riscv_matrix_instance_f32 * pd,
  uint16_t * pp)
{

  riscv_status status;                             /* status of matrix inverse */
  {

    const int n=pSrc->numRows;
    int fullRank = 1, diag,k;
    float32_t *pA;

    memcpy(pl->pData,pSrc->pData,n*n*sizeof(float32_t));
    pA = pl->pData;

    for(int k=0;k < n; k++)
    {
      pp[k] = k;
    }


    for(k=0;k < n; k++)
    {
        /* Find pivot */
        float32_t m=F32_MIN,a;
        int j=k; 


        for(int r=k;r<n;r++)
        {
           if (pA[r*n+r] > m)
           {
             m = pA[r*n+r];
             j = r;
           }
        }

        if(j != k)
        {
          SWAP_ROWS_F32(pA,k,j);
          SWAP_COLS_F32(pA,k,j);
        }


        pp[k] = j;

        a = pA[k*n+k];

        if (fabs(a) < 1.0e-8)
        {

            fullRank = 0;
            break;
        }

        for(int w=k+1;w<n;w++)
        {
          for(int x=k+1;x<n;x++)
          {
             pA[w*n+x] = pA[w*n+x] - pA[w*n+k] * pA[x*n+k] / a;
          }
        }

        for(int w=k+1;w<n;w++)
        {
               pA[w*n+k] = pA[w*n+k] / a;
        }

        

    }



    diag=k;
    if (!fullRank)
    {
      diag--;
      for(int row=0; row < n;row++)
      {
        for(int col=k; col < n;col++)
        {
           pl->pData[row*n+col]=0.0;
        }
      }
    }

    for(int row=0; row < n;row++)
    {
       for(int col=row+1; col < n;col++)
       {
         pl->pData[row*n+col] = 0.0;
       }
    }

    for(int d=0; d < diag;d++)
    {
      pd->pData[d*n+d] = pl->pData[d*n+d];
      pl->pData[d*n+d] = 1.0;
    }
  
    status = RISCV_MATH_SUCCESS;

  }

  
  /* Return to application */
  return (status);
}

/// @private
#define SWAP_ROWS_F64(A,i,j)     \
  for(int w=0;w < n; w++)    \
  {                          \
     float64_t tmp;          \
     tmp = A[i*n + w];       \
     A[i*n + w] = A[j*n + w];\
     A[j*n + w] = tmp;       \
  }
/// @private
#define SWAP_COLS_F64(A,i,j)     \
  for(int w=0;w < n; w++)    \
  {                          \
     float64_t tmp;          \
     tmp = A[w*n + i];       \
     A[w*n + i] = A[w*n + j];\
     A[w*n + j] = tmp;       \
  }

riscv_status ref_mat_ldlt_f64(
  const riscv_matrix_instance_f64 * pSrc,
  riscv_matrix_instance_f64 * pl,
  riscv_matrix_instance_f64 * pd,
  uint16_t * pp)
{

  riscv_status status;                             /* status of matrix inverse */
  {

    const int n=pSrc->numRows;
    int fullRank = 1, diag,k;
    float64_t *pA;

    memcpy(pl->pData,pSrc->pData,n*n*sizeof(float64_t));
    pA = pl->pData;

    for(int k=0;k < n; k++)
    {
      pp[k] = k;
    }


    for(k=0;k < n; k++)
    {
        /* Find pivot */
        float64_t m=F64_MIN,a;
        int j=k; 


        for(int r=k;r<n;r++)
        {
           if (pA[r*n+r] > m)
           {
             m = pA[r*n+r];
             j = r;
           }
        }

        if(j != k)
        {
          SWAP_ROWS_F64(pA,k,j);
          SWAP_COLS_F64(pA,k,j);
        }


        pp[k] = j;

        a = pA[k*n+k];

        if (fabs(a) < 1.0e-18)
        {

            fullRank = 0;
            break;
        }

        for(int w=k+1;w<n;w++)
        {
          for(int x=k+1;x<n;x++)
          {
             pA[w*n+x] = pA[w*n+x] - pA[w*n+k] * pA[x*n+k] / a;
          }
        }

        for(int w=k+1;w<n;w++)
        {
               pA[w*n+k] = pA[w*n+k] / a;
        }
    }

    diag=k;
    if (!fullRank)
    {
      diag--;
      for(int row=0; row < n;row++)
      {
        for(int col=k; col < n;col++)
        {
           pl->pData[row*n+col]=0.0;
        }
      }
    }

    for(int row=0; row < n;row++)
    {
       for(int col=row+1; col < n;col++)
       {
         pl->pData[row*n+col] = 0.0;
       }
    }

    for(int d=0; d < diag;d++)
    {
      pd->pData[d*n+d] = pl->pData[d*n+d];
      pl->pData[d*n+d] = 1.0;
    }
  
    status = RISCV_MATH_SUCCESS;

  }

  
  /* Return to application */
  return (status);
}