#include "ref.h"

void ref_quaternion_inverse_f32(const float32_t *pInputQuaternions, 
  float32_t *pInverseQuaternions,
  uint32_t nbQuaternions)
{
   float32_t temp;

   for(uint32_t i=0; i < nbQuaternions; i++)
   {

      temp = SQ(pInputQuaternions[4 * i + 0]) +
             SQ(pInputQuaternions[4 * i + 1]) +
             SQ(pInputQuaternions[4 * i + 2]) +
             SQ(pInputQuaternions[4 * i + 3]);

      pInverseQuaternions[4 * i + 0] = pInputQuaternions[4 * i + 0] / temp;
      pInverseQuaternions[4 * i + 1] = -pInputQuaternions[4 * i + 1] / temp;
      pInverseQuaternions[4 * i + 2] = -pInputQuaternions[4 * i + 2] / temp;
      pInverseQuaternions[4 * i + 3] = -pInputQuaternions[4 * i + 3] / temp;
   }
}


