// ta/vM.c
// author: somnathkarmakar1203@gmail.com

#include <stdint.h>
#include "include/vm.h"
#include <string.h>

TEE_Result vm(const uint32_t *x, uint32_t *outv, uint32_t n, uint32_t q)
{
  memset(outv, 0, n*sizeof(outv[0]));
  /*
    C_1xn = V_1xn * M_nxn
    C[i] = SUM V[j]*M[j,i] for j=0..n-1
    M[r,c]=M[r*n+c]
   */
  /* for (uint32_t i = 0; i < n; i++) */
  /* { */
  /*   for (uint32_t j = 0; j < n; j++) */
  /*   { */
  /*     outv[i] += ((D[j]*x[j*n+i])%q); */
  /*     outv[i] %= q; */
  /*   } */
  /* } */
  /* return TEE_SUCCESS; */
  for (uint32_t i = 0; i < n; i++)
  {
    uint64_t accumulator = 0;
    for (uint32_t j = 0; j < n; j++)
    {
      // Cast components to uint64_t before multiplying to prevent integer overflow
      uint64_t d_val = D[j];
      uint64_t x_val = x[j * n + i];
      
      accumulator += (d_val * x_val);
    }
    // Apply final modulo reduction safely to the output vector index
    outv[i] = (uint32_t)(accumulator % q);
  }
  return TEE_SUCCESS;
}


TEE_Result gen_d(uint32_t *v, uint32_t n, uint32_t q){
  uint32_t i = 0;
  uint32_t max_acceptable = UINT32_MAX - (UINT32_MAX%q);
  while (i<n){
    uint32_t rand_val;
    TEE_GenerateRandom(&rand_val, sizeof(rand_val));
    if (rand_val < max_acceptable){
      v[i] = rand_val%q;
      i++;
    }
  }
  return TEE_SUCCESS;
}
