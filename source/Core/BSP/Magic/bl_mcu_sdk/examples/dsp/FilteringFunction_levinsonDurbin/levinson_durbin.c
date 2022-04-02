#include "ref.h"

void ref_levinson_durbin_f32(const float32_t *phi,
  float32_t *a, 
  float32_t *err,
  int nbCoefs)
{
   float32_t e;

   a[0] = phi[1] / phi[0];

   e = phi[0] - phi[1] * a[0];
   for(int p=1; p < nbCoefs; p++)
   {
      float32_t suma=0.0f;
      float32_t sumb=0.0f;
      float32_t k;
      int nb,j;

      for(int i=0; i < p; i++)
      {
         suma += a[i] * phi[p - i];
         sumb += a[i] * phi[i + 1];
      }

      k = (phi[p+1]-suma)/(phi[0] - sumb);


      nb = p >> 1;
      j=0;
      for(int i =0; i < nb ; i++)
      {
          float32_t x,y;

          x=a[j] - k * a[p-1-j];
          y=a[p-1-j] - k * a[j];

          a[j] = x;
          a[p-1-j] = y;

          j++;
      }

      nb = p & 1;
      if (nb)
      {
            a[j]=a[j]- k * a[p-1-j];
      }

      a[p] = k;
      e = e * (1.0f - k*k);


   }
   *err = e;
}

#include "dsp/filtering_functions.h"

#define ONE_Q31 0x7FFFFFFFL
#define TWO_Q30 0x7FFFFFFFL

#define HALF_Q31 0x00008000L
#define ONE_Q15 0x7FFF
#define HALF_Q15 0x3FFF
#define LOWPART_MASK 0x07FFF

__STATIC_FORCEINLINE q31_t mul32x16(q31_t a, q15_t b)
{
  q31_t r = ((q63_t)a * (q63_t)b) >> 15;

  return(r);
  
}

__STATIC_FORCEINLINE q31_t mul32x32(q31_t a, q31_t b)
{
  //q31_t r = __SSAT(((q63_t)a * b) >> 31,31);
  q31_t r = ((q63_t)a * b) >> 31;

  return(r);
  
}

__STATIC_FORCEINLINE q31_t divide(q31_t n, q31_t d)
{
  riscv_status status;
  int16_t shift;
  q15_t inverse;
  q31_t r;
  // We are computing:
  // n / d = n / (h + l) where h and l are the high end and low end part.
  // 1 / (h + l) = 1 / h (1 - l / h)
  // Our division algorithm has a shift. So it is returning a scaled value sh.
  // So we need a << shift to convert 1/ sh to 1/h.
  // In below code, we are organizing the computation differently. Instead of computing:
  // 1 / h (1 - l / h) 
  // we are computing
  // 1 / h (2 - (l + h) / h) 
  // 1 / h (2 - d / h)
  // Also, we are not computing 1/h in Q15 but in Q14.
  // 2 is expressed in Q30.
  // So at the end of all computation we need a << 2

  // Result is in Q14 because of use of HALF_Q15 instead of ONE_Q15.
  status=riscv_divide_q15(HALF_Q15,d>>16,&inverse,&shift);
  (void)status;
  
  // d is used instead of l
  // So we will need to substract to 2 instead of 1.
  r = mul32x16(d,inverse);
  r = TWO_Q30 - (r << shift);
  r = mul32x16(r, inverse);
  r = mul32x32(r,n) ;
  r = r << (shift + 2);
  
  return(r);
  
}

void ref_levinson_durbin_q31(const q31_t *phi,
  q31_t *a, 
  q31_t *err,
  int nbCoefs)
{
   q31_t e;

   //a[0] = phi[1] / phi[0];
   a[0] = divide(phi[1], phi[0]);
   

   //e = phi[0] - phi[1] * a[0];
   e = phi[0] - mul32x32(phi[1],a[0]);

   for(int p=1; p < nbCoefs; p++)
   {
      q63_t suma=0;
      q63_t sumb=0;
      q31_t k;
      int nb,j;

      for(int i=0; i < p; i++)
      {
         suma += ((q63_t)a[i] * phi[p - i]);
         sumb += ((q63_t)a[i] * phi[i + 1]);
      }

      suma = suma >> 31;
      sumb = sumb >> 31;



      //k = (phi[p+1]-suma)/(phi[0] - sumb);
      k = divide(phi[p+1]-(q31_t)suma,phi[0] - (q31_t)sumb);


      nb = p >> 1;
      j=0;
      for(int i =0;i < nb ; i++)
      {
          q31_t x,y;

          //x = a[j] - k * a[p-1-j];
          x = a[j] - mul32x32(k,a[p-1-j]);

          //y = a[p-1-j] - k * a[j];
          y = a[p-1-j] - mul32x32(k , a[j]);

          a[j] = x;
          a[p-1-j] = y;

          j++;
      }

      nb = p & 1;
      if (nb)
      {
            //a[j] = a[j]- k * a[p-1-j];
            a[j] = a[j] - mul32x32(k,a[p-1-j]);
      }

      a[p] = k;

      // e = e * (1 - k*k);
      e = mul32x32(e,ONE_Q31 - mul32x32(k,k));


   }
   *err = e;
}
