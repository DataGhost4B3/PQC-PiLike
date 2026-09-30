
// ta/pilike_ta.c
// author: somnathkarmakar1203@gmail.com

#include <stdint.h>
/* #include <sys/types.h> */
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <pilike.h>
#include <vm.h>

uint32_t D[4096];

TEE_Result TA_CreateEntryPoint(void) { DMSG("has been called");return TEE_SUCCESS; }

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
                                    TEE_Param __unused params[4],
                                    void __unused **sess_ctx) {
  return TEE_SUCCESS;
}

/* TEE_Result TA_InvokeCommandEntryPoint(void __unused *sess_ctx, */
/*                                       uint32_t cmd_id, */
/*                                       uint32_t param_types, */
/*                                       TEE_Param params[4]) { */

/*   if (cmd_id != GET_P) {return TEE_ERROR_BAD_PARAMETERS;} */
  
/*   uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INOUT, */
/* 					     TEE_PARAM_TYPE_VALUE_INPUT, */
/* 					     TEE_PARAM_TYPE_NONE, */
/* 					     TEE_PARAM_TYPE_NONE); */
/*   if (param_types != exp_param_types) { */
/*     return TEE_ERROR_BAD_PARAMETERS; */
/*   } */

/*   uint32_t n = params[1].value.a; */
/*   uint32_t q = params[1].value.b; */
  
/*   uint32_t *X = (uint32_t *)params[0].memref.buffer; */
/*   //  uint32_t n = params[0].memref.size / sizeof(uint32_t); */
/*   uint32_t buffer_size = params[0].memref.size; */

/*   // Sanity check parameters against provided memory boundaries */
/*   if (buffer_size < (n * n * sizeof(uint32_t))) { */
/*     return TEE_ERROR_SHORT_BUFFER; */
/*   } */

/*   TEE_Result res = gen_d(D, n, q); */
/*   if (res != TEE_SUCCESS) { */
/*     return res; */
/*   } */
/*   uint32_t *P_temp = TEE_Malloc(n * sizeof(uint32_t), 0); */
/*   if (!P_temp) { */
/*     return TEE_ERROR_OUT_OF_MEMORY; */
/*   } */
/*   // --- Step C: Compute vector-transpose-matrix multiplication: P = d^T * X mod q --- */
/*   // X is an (n x n) row-major matrix layout -> access element via X[row * n + col] */
/*   for (uint32_t col = 0; col < n; col++) { */
/*     uint64_t accumulator = 0; */
/*     for (uint32_t row = 0; row < n; row++) { */
/*       uint64_t d_val = D[row]; */
/*       uint64_t x_val = X[row * n + col]; */
      
/*       accumulator += d_val * x_val; */
/*     } */
/*     P_temp[col] = (uint32_t)(accumulator % q); */
/*   } */
/*   TEE_MemMove(X, P_temp, n * sizeof(uint32_t)); */
/*   TEE_Free(P_temp); */
/*   return TEE_SUCCESS;   */
/* } */
TEE_Result TA_InvokeCommandEntryPoint(void __unused *sess_ctx,
                                      uint32_t cmd_id,
                                      uint32_t param_types,
                                      TEE_Param params[4]) {

  if (cmd_id != GET_P) { return TEE_ERROR_BAD_PARAMETERS; }
  
  uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INOUT,
                                             TEE_PARAM_TYPE_VALUE_INPUT,
                                             TEE_PARAM_TYPE_NONE,
                                             TEE_PARAM_TYPE_NONE);
  if (param_types != exp_param_types) {
    return TEE_ERROR_BAD_PARAMETERS;
  }

  uint32_t n = params[1].value.a;
  uint32_t q = params[1].value.b;
  
  uint32_t *X = (uint32_t *)params[0].memref.buffer;
  uint32_t buffer_size = params[0].memref.size;

  /* Sanity check parameters against provided memory boundaries */
  if (buffer_size < (n * n * sizeof(uint32_t))) {
    return TEE_ERROR_SHORT_BUFFER;
  }

  /* Generate the secret vector D */
  TEE_Result res = gen_d(D, n, q);
  if (res != TEE_SUCCESS) {
    return res;
  }

  /* Allocate a temporary buffer for the 1xN output vector */
  uint32_t *P_temp = TEE_Malloc(n * sizeof(uint32_t), 0);
  if (!P_temp) {
    return TEE_ERROR_OUT_OF_MEMORY;
  }

  /* Call the external vm() function to perform the multiplication */
  res = vm(X, P_temp, n, q);
  if (res != TEE_SUCCESS) {
    TEE_Free(P_temp);
    return res;
  }

  /* Copy the resulting 1xN vector back to the first row of the shared memory */
  TEE_MemMove(X, P_temp, n * sizeof(uint32_t));
  
  TEE_Free(P_temp);
  return TEE_SUCCESS;  
}

void TA_CloseSessionEntryPoint(void __unused *sess_ctx) {
  //
}

void TA_DestroyEntryPoint(void) {
  //
}
