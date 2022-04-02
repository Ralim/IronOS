#include "ref.h"

void ref_and_u16(
    const uint16_t * pSrcA,
    const uint16_t * pSrcB,
          uint16_t * pDst,
          uint32_t blockSize)
{
    uint32_t blkCnt;      /* Loop counter */
    blkCnt = blockSize;
    while (blkCnt > 0U)
    {
        *pDst++ = (*pSrcA++)&(*pSrcB++);

        /* Decrement the loop counter */
        blkCnt--;
    }
}

void ref_and_u32(
    const uint32_t * pSrcA,
    const uint32_t * pSrcB,
          uint32_t * pDst,
          uint32_t blockSize)
{
    /* Initialize blkCnt with number of samples */
    uint32_t blkCnt = blockSize;
    while (blkCnt > 0U)
    {
        *pDst++ = (*pSrcA++)&(*pSrcB++);

        /* Decrement the loop counter */
        blkCnt--;
    }
}

void ref_and_u8(
    const uint8_t * pSrcA,
    const uint8_t * pSrcB,
          uint8_t * pDst,
          uint32_t blockSize)
{
    uint32_t blkCnt = blockSize;
    while (blkCnt > 0U)
    {
        *pDst++ = (*pSrcA++)&(*pSrcB++);

        /* Decrement the loop counter */
        blkCnt--;
    }
}
