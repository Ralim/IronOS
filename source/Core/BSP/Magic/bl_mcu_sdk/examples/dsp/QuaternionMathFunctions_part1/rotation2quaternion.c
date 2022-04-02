#include "ref.h"

#define RI(x,y) r[(3*(x) + (y))]

void ref_rotation2quaternion_f32(const float32_t *pInputRotations, 
    float32_t *pOutputQuaternions,  
    uint32_t nbQuaternions)
{
   for(uint32_t nb=0; nb < nbQuaternions; nb++)
   {
       const float32_t *r=&pInputRotations[nb*9];
       float32_t *q=&pOutputQuaternions[nb*4];

       float32_t trace = RI(0,0) + RI(1,1) + RI(2,2);

       float32_t doubler;
       float32_t s;

      if (trace > 0)
      {
        doubler = sqrtf(trace + 1.0) * 2; // invs=4*qw
        s = 1.0 / doubler;
        q[0] = 0.25 * doubler;
        q[1] = (RI(2,1) - RI(1,2)) * s;
        q[2] = (RI(0,2) - RI(2,0)) * s;
        q[3] = (RI(1,0) - RI(0,1)) * s;
      }
      else if ((RI(0,0) > RI(1,1)) && (RI(0,0) > RI(2,2)) )
      {
        doubler = sqrtf(1.0 + RI(0,0) - RI(1,1) - RI(2,2)) * 2; // invs=4*qx
        s = 1.0 / doubler;
        q[0] = (RI(2,1) - RI(1,2)) * s;
        q[1] = 0.25 * doubler;
        q[2] = (RI(0,1) + RI(1,0)) * s;
        q[3] = (RI(0,2) + RI(2,0)) * s;
      }
      else if (RI(1,1) > RI(2,2))
      {
        doubler = sqrtf(1.0 + RI(1,1) - RI(0,0) - RI(2,2)) * 2; // invs=4*qy
        s = 1.0 / doubler;
        q[0] = (RI(0,2) - RI(2,0)) * s;
        q[1] = (RI(0,1) + RI(1,0)) * s;
        q[2] = 0.25 * doubler;
        q[3] = (RI(1,2) + RI(2,1)) * s;
      }
      else
      {
        doubler = sqrtf(1.0 + RI(2,2) - RI(0,0) - RI(1,1)) * 2; // invs=4*qz
        s = 1.0 / doubler;
        q[0] = (RI(1,0) - RI(0,1)) * s;
        q[1] = (RI(0,2) + RI(2,0)) * s;
        q[2] = (RI(1,2) + RI(2,1)) * s;
        q[3] = 0.25 * doubler;
      }

    }
}