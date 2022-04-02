#include "ref.h"

void ref_quaternion2rotation_f32(const float32_t *pInputQuaternions, 
    float32_t *pOutputRotations, 
    uint32_t nbQuaternions)
{
   for(uint32_t nb=0; nb < nbQuaternions; nb++)
   {
        float32_t q00 = SQ(pInputQuaternions[0 + nb * 4]);
        float32_t q11 = SQ(pInputQuaternions[1 + nb * 4]);
        float32_t q22 = SQ(pInputQuaternions[2 + nb * 4]);
        float32_t q33 = SQ(pInputQuaternions[3 + nb * 4]);
        float32_t q01 =  pInputQuaternions[0 + nb * 4]*pInputQuaternions[1 + nb * 4];
        float32_t q02 =  pInputQuaternions[0 + nb * 4]*pInputQuaternions[2 + nb * 4];
        float32_t q03 =  pInputQuaternions[0 + nb * 4]*pInputQuaternions[3 + nb * 4];
        float32_t q12 =  pInputQuaternions[1 + nb * 4]*pInputQuaternions[2 + nb * 4];
        float32_t q13 =  pInputQuaternions[1 + nb * 4]*pInputQuaternions[3 + nb * 4];
        float32_t q23 =  pInputQuaternions[2 + nb * 4]*pInputQuaternions[3 + nb * 4];

        float32_t xx = q00 + q11 - q22 - q33;
        float32_t yy = q00 - q11 + q22 - q33;
        float32_t zz = q00 - q11 - q22 + q33;
        float32_t xy = 2*(q12 - q03);
        float32_t xz = 2*(q13 + q02);
        float32_t yx = 2*(q12 + q03);
        float32_t yz = 2*(q23 - q01);
        float32_t zx = 2*(q13 - q02);
        float32_t zy = 2*(q23 + q01);

        pOutputRotations[0 + nb * 9] = xx; pOutputRotations[1 + nb * 9] = xy; pOutputRotations[2 + nb * 9] = xz;
        pOutputRotations[3 + nb * 9] = yx; pOutputRotations[4 + nb * 9] = yy; pOutputRotations[5 + nb * 9] = yz;
        pOutputRotations[6 + nb * 9] = zx; pOutputRotations[7 + nb * 9] = zy; pOutputRotations[8 + nb * 9] = zz;
   }
}

