/* { dg-require-effective-target vect_int } */

#include <stdarg.h>
#include "../../tree-vect.h"

#define N 8 

int
main1 ()
{
  int i;
  unsigned int out[N*8], a0, a1, a2, a3, a4, a5, a6, a7, b1, b0, b2, b3, b4, b5, b6, b7;
  unsigned int in[N*8] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63};
  float out2[N*8], fa[N*4];
  unsigned int ia[N], ib[N*2];

  for (i = 0; i < N; i++)
    {
      
      a0 = in[i*8] + 5;
      a1 = in[i*8 + 1] + 6;
      a2 = in[i*8 + 2] + 7;
      a3 = in[i*8 + 3] + 8;
      a4 = in[i*8 + 4] + 9;
      a5 = in[i*8 + 5] + 10;
      a6 = in[i*8 + 6] + 11;
      a7 = in[i*8 + 7] + 12;

      b0 = a0 * 3;
      b1 = a1 * 2;
      b2 = a2 * 12;
      b3 = a3 * 5;
      b4 = a4 * 8;
      b5 = a5 * 4;
      b6 = a6 * 3;
      b7 = a7 * 2;

      out[i*8] = b0 - 2;
      out[i*8 + 1] = b1 - 3; 
      out[i*8 + 2] = b2 - 2;
      out[i*8 + 3] = b3 - 1;
      out[i*8 + 4] = b4 - 8;
      out[i*8 + 5] = b5 - 7;
      out[i*8 + 6] = b6 - 3;
      out[i*8 + 7] = b7 - 7;

      ia[i] = b6;
    }

  /* check results:  */
#pragma GCC novector
  for (i = 0; i < N; i++)
    {
      if (out[i*8] !=  (in[i*8] + 5) * 3 - 2
         || out[i*8 + 1] != (in[i*8 + 1] + 6) * 2 - 3
         || out[i*8 + 2] != (in[i*8 + 2] + 7) * 12 - 2
         || out[i*8 + 3] != (in[i*8 + 3] + 8) * 5 - 1
         || out[i*8 + 4] != (in[i*8 + 4] + 9) * 8 - 8
         || out[i*8 + 5] != (in[i*8 + 5] + 10) * 4 - 7
         || out[i*8 + 6] != (in[i*8 + 6] + 11) * 3 - 3
         || out[i*8 + 7] != (in[i*8 + 7] + 12) * 2 - 7
         || ia[i] != (in[i*8 + 6] + 11) * 3)
	abort ();
    }

  for (i = 0; i < N*2; i++)
    {
      out[i*4] = (in[i*4] + 2) * 3;
      out[i*4 + 1] = (in[i*4 + 1] + 2) * 7;
      out[i*4 + 2] = (in[i*4 + 2] + 7) * 3;
      out[i*4 + 3] = (in[i*4 + 3] + 7) * 7;

      ib[i] = 7;
    }

  /* check results:  */
#pragma GCC novector
  for (i = 0; i < N*2; i++)
    {
      if (out[i*4] !=  (in[i*4] + 2) * 3
         || out[i*4 + 1] != (in[i*4 + 1] + 2) * 7
         || out[i*4 + 2] != (in[i*4 + 2] + 7) * 3
         || out[i*4 + 3] != (in[i*4 + 3] + 7) * 7
         || ib[i] != 7)
        abort ();
    }

  for (i = 0; i < N*4; i++)
    {
      out2[i*2] = (float) (in[i*2] * 2 + 11) ;
      out2[i*2 + 1] = (float) (in[i*2 + 1] * 3 + 7);
    
      fa[i] = (float) in[i*2+1];
    }

  /* check results:  */
#pragma GCC novector
  for (i = 0; i < N*4; i++)
    {
      if (out2[i*2] !=  (float) (in[i*2] * 2 + 11)
         || out2[i*2 + 1] != (float) (in[i*2 + 1] * 3 + 7)
         || fa[i] != (float) in[i*2+1])
        abort ();
    }


  return 0;
}

int main (void)
{
  check_vect ();

  main1 ();

  return 0;
}

/* { dg-final { scan-tree-dump-times "vectorized 3 loops" 1 "vect"  {target { vect_strided8 && vect_int_mult } } } } */
/* { dg-final { scan-tree-dump-times "vectorizing stmts using SLP" 6 "vect" {target { vect_strided8 && vect_int_mult } } } } */
  
