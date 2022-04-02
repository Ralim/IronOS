#include "ref.h"

void ref_quaternion_product_f32(const float32_t *qa, 
    const float32_t *qb, 
    float32_t *qr,
    uint32_t nbQuaternions)
{
   for(uint32_t i=0; i < nbQuaternions; i++)
   {
     ref_quaternion_product_single_f32(qa, qb, qr);

     qa += 4;
     qb += 4;
     qr += 4;
   }
}


