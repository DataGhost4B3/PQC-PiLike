
// ta/pilike_ta.c
// author: somnathkarmakar1203@gmail.com

#include <stdint.h>
#include <sys/types.h>
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <pilike.h>
#include <vm.h>

TEEC_Result TA_CreateEntryPoint(void) { DMSG("has been called");return TEE_SUCCESS; }

TEEC_Result TA_OpenSessionEntryPoint(uint32_t param_types,
                                    TEE_Param __unused params[4],
                                    void __unused **sess_ctx) {
  return TEE_SUCCESS;
}

TEEC_Result TA_InvokeCommandEntryPoint(void __unused *sess_ctx,
                                      uint32_t cmd_id,
                                      uint32_t param_types,
                                      TEE_Param params[4]) {

  if (cmd_id != GET_P) {return TEE_ERROR_BAD_PARAMETERS;}
  
  uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INOUT,
					     TEE_PARAM_TYPE_NONE,
					     TEE_PARAM_TYPE_NONE,
					     TEE_PARAM_TYPE_NONE,);
  if (param_types != exp_param_types) {
    return TEE_ERROR_BAD_PARAMETERS;
  }

  float *X = (uint32_t *)params[0].memref.buffer;
  uint32_t n = params[0].memref.size / sizeof(uint32_t);

  for (uint32_t i = 0; i < n; i++){
    //gen d then calc P
  }
    
}


void TA_CloseSessionEntryPoint(void __unused *sess_ctx) {
  //
}

void TA_DestroyEntryPoint(void) {
  //
}
