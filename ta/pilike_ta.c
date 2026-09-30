// ta/pilike_ta.c
// author: somnathkarmakar1203@gmail.com

#include <stdint.h>
#include <stdbool.h>
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <pilike.h>
#include <vm.h>

uint32_t D[4096];
static bool d_initialized = false;

TEE_Result TA_CreateEntryPoint(void) { 
  DMSG("has been called");
  return TEE_SUCCESS; 
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
                                    TEE_Param __unused params[4],
                                    void __unused **sess_ctx) {
  return TEE_SUCCESS;
}

TEE_Result TA_InvokeCommandEntryPoint(void __unused *sess_ctx,
                                      uint32_t cmd_id,
                                      uint32_t param_types,
                                      TEE_Param params[4]) {

  if (cmd_id != GET_P) { return TEE_ERROR_BAD_PARAMETERS; }
  
  uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                                             TEE_PARAM_TYPE_VALUE_INPUT,
                                             TEE_PARAM_TYPE_MEMREF_OUTPUT,
                                             TEE_PARAM_TYPE_NONE);
  if (param_types != exp_param_types) {
    return TEE_ERROR_BAD_PARAMETERS;
  }

  uint32_t n = params[1].value.a;
  uint32_t q = params[1].value.b;
  
  uint32_t *X = (uint32_t *)params[0].memref.buffer;
  uint32_t *P_out = (uint32_t *)params[2].memref.buffer;

  /* Sanity check both parameters against provided memory boundaries */
  if (params[0].memref.size < (n * n * sizeof(uint32_t))) {
    return TEE_ERROR_SHORT_BUFFER;
  }
  if (params[2].memref.size < (n * sizeof(uint32_t))) {
    return TEE_ERROR_SHORT_BUFFER;
  }

  /* Generate the secret vector D only if it hasn't been created yet */
  if (!d_initialized) {
    TEE_Result res = gen_d(D, n, q);
    if (res != TEE_SUCCESS) {
      return res;
    }
    d_initialized = true;
    DMSG("Secret vector d generated and stored securely.");
    DMSG("Elements of secure vector D:");
    for (uint32_t i = 0; i < n; i++) {
        DMSG("D[%u] = %u", (unsigned int)i, (unsigned int)D[i]);
    }
  } else {
    DMSG("Secret vector d already exists in secure memory.");
  }

  /* Call the external vm() function to perform the multiplication directly into the host's output buffer */
  TEE_Result res = vm(X, P_out, n, q);
  
  return res;  
}

void TA_CloseSessionEntryPoint(void __unused *sess_ctx) {
  //
}

void TA_DestroyEntryPoint(void) {
  //
}
