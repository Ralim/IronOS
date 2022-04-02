#include "ref.h"

void ref_quaternion_conjugate_f32(const float32_t *pInputQuaternions, 
    float32_t *pConjugateQuaternions, 
    uint32_t nbQuaternions)
{
   for(uint32_t i=0; i < nbQuaternions; i++)
   {

      pConjugateQuaternions[4 * i + 0] = pInputQuaternions[4 * i + 0];
      pConjugateQuaternions[4 * i + 1] = -pInputQuaternions[4 * i + 1];
      pConjugateQuaternions[4 * i + 2] = -pInputQuaternions[4 * i + 2];
      pConjugateQuaternions[4 * i + 3] = -pInputQuaternions[4 * i + 3];
   }
}


