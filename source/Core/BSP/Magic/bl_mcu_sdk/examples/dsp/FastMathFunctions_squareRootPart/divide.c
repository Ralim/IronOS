#include "ref.h"

riscv_status ref_divide_q15(q15_t numerator,
  q15_t denominator,
  q15_t *quotient,
  int16_t *shift)
{
  int16_t sign=0;
  q31_t temp;
  int16_t shiftForNormalizing;

  *shift = 0;

  sign = (numerator>>15) ^ (denominator>>15);

  if (denominator == 0)
  {
     if (sign)
     {
        *quotient = 0x8000;
     }
     else
     {
        *quotient = 0x7FFF;
     }
     return(RISCV_MATH_NANINF);
  }

  numerator = abs(numerator);
  denominator = abs(denominator);
  
  temp = ((q31_t)numerator << 15) / ((q31_t)denominator);

  shiftForNormalizing= 17 - __CLZ(temp);
  if (shiftForNormalizing > 0)
  {
     *shift = shiftForNormalizing;
     temp = temp >> shiftForNormalizing;
  }

  if (sign)
  {
    temp = -temp;
  }

  *quotient=temp;

  return(RISCV_MATH_SUCCESS);
}