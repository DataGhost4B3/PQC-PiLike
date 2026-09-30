/* host/main.c
   author: somnathkarmakar1203@gmail.com */

#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <pilike.h>
#include <generatex.h>

int main(void) {
  TEEC_Result res;
  TEEC_Session sess;
  TEEC_Operation op;
  TEEC_UUID uuid = PILIKE_UUID;
  uint32_t err_origin;
  TEEC_SharedMemory shm;

  res = TEEC_InitializeContext(NULL, &ctx);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
  }

  uint32_t m; scanf("Enter m: %" PRIu32 "\n", &m);
  uint32_t q = m*m; printf("q chosen (q=m^2): %" PRIu32 "\n", q);
  uint32_t n;
  
  shm.size = n*n*sizeof(uint32_t);
  shm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT ;
  res = TEEC_AllocateSharedMemory(&ctx, &shm);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_AllocateSharedMemory failed with code 0x%x", res);
  }
  uint32_t *X = (uint32_t *)shm.buffer;
  // initialize X
  
  res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
  if (res != TEEC_SUCCCESS) {
    errx(1, "TEEC_OpenSession failed with code 0x%x origin 0x%" PRIX32 "\n", res, err_origin);
  }

  memset(&op, 0, sizeof(op));
  // op.paramTypes
  op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_WHOLE, TEEC_NONE, TEEC_NONE, TEEC_NONE);
  op.params[0].memref.parent = &shm;
  op.params[0].memref.offset = 0;
  op.params[0].memref.size = shm.size;

  //TEEC_InvokeCommand
  res = TEEC_InvokeCommand(&sess, GET_P, &op, &err_origin);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_InvokeCommand failed with code 0x%x", res);
  }
  
  TEEC_CloseSession(&sess);
  TEEC_ReleaseSharedMemory(&shm);
  TEEC_FinalizeContext(&ctx);
}
