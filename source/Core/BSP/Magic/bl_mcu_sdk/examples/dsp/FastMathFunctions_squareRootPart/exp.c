#include "ref.h"

void ref_vexp_f32(
  const float32_t * pSrc,
        float32_t * pDst,
        uint32_t blockSize)
{
   uint32_t blkCnt; 

   blkCnt = blockSize;

   while (blkCnt > 0U)
   {
      /* C = log(A) */
  
      /* Calculate log and store result in destination buffer. */
      *pDst++ = expf(*pSrc++);
  
      /* Decrement loop counter */
      blkCnt--;
   }
}