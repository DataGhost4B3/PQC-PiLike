// ta/vM.c
// author: somnathkarmakar1203@gmail.com

#include <vm.h>
#include <string.h>

TEE_Result vm(const uint32_t *x, uint32_t *outv, uint32_t n, uint32_t q)
{
  memset(outv, 0, n*sizeof(outv[0]));
  /*
    C_1xn = V_1xn * M_nxn
    C[i] = SUM V[j]*M[j,i] for j=0..n-1
    M[r,c]=M[r*n+c]
   */
  for (uint32_t i = 0; i < n; i++)
  {
    for (uint32_t j = 0; j < n; j++)
    {
      outv[i] += ((D[j]*x[j*n+i])%q);
      outv[i] %= q;
    }
  }
  return TEE_SUCCESS;
}
