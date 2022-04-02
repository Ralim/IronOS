#include "ref.h"

void ref_quaternion_norm_f32(const float32_t *pInputQuaternions, 
  float32_t *pNorms,
  uint32_t nbQuaternions)
{
   float32_t temp;

   for(uint32_t i=0; i < nbQuaternions; i++)
   {
      temp = SQ(pInputQuaternions[4 * i + 0]) +
             SQ(pInputQuaternions[4 * i + 1]) +
             SQ(pInputQuaternions[4 * i + 2]) +
             SQ(pInputQuaternions[4 * i + 3]);
      pNorms[i] = sqrtf(temp);
   }
}


