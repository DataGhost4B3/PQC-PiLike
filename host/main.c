/* host/main.c
   author: somnathkarmakar1203@gmail.com */

#include "genx.h"
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <tee_client_api.h>
#include <time.h>

#include <pilike.h>

uint64_t deterministic_floor_32bit(uint32_t m, uint32_t q);
int64_t deterministic_floor_2m_log10(int64_t m, uint64_t q);

int main(void) {
  TEEC_Result res;
  TEEC_Session sess;
  TEEC_Operation op;
  TEEC_Context ctx;
  TEEC_UUID uuid = PILIKE_UUID;
  uint32_t err_origin;
  TEEC_SharedMemory shm;

  res = TEEC_InitializeContext(NULL, &ctx);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
  }

  uint32_t m; /* scanf("Enter m: %" PRIu32 "\n", &m); */
  printf("Enter m: ");
  if (scanf("%" PRIu32, &m) != 1) {
      errx(1, "Invalid input for m");
  }
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  uint32_t q = m*m; printf("q chosen (q=m^2): %" PRIu32 "\n", q);
  uint32_t n = deterministic_floor_32bit(m,q); printf("n: %" PRIu32 "\n", n);
  
  shm.size = n*n*sizeof(uint32_t);
  shm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT ;
  /* shm.flags = TEEC_MEM_SHARED_IN | TEEC_MEM_SHARED_OUT; */
  res = TEEC_AllocateSharedMemory(&ctx, &shm);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_AllocateSharedMemory failed with code 0x%x", res);
  }
  uint32_t *X = (uint32_t *)shm.buffer;
  // initialize X
  int x_init = generate_x(X, n, q);
  if (x_init!=0){errx(1, "failed to generate X");}
  
  res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_OpenSession failed with code 0x%x origin 0x%" PRIX32 "\n", res, err_origin);
  }

  memset(&op, 0, sizeof(op));
  // op.paramTypes
  op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_WHOLE, TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE);
  
  op.params[0].memref.parent = &shm;
  op.params[0].memref.offset = 0;
  op.params[0].memref.size = shm.size;

  op.params[1].value.a = n;
  op.params[1].value.b = q;

  //TEEC_InvokeCommand
  res = TEEC_InvokeCommand(&sess, GET_P, &op, &err_origin);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_InvokeCommand failed with code 0x%x", res);
  }

  uint32_t *P = (uint32_t *)shm.buffer;
  clock_gettime(CLOCK_MONOTONIC, &end);
  for (uint32_t i = 0; i < n; i++){
    printf("%" PRIu32 " ", P[i]);
  }

  double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
  printf("\n[Timing] Core Setup execution (from m to P) took: %.6f seconds\n\n", elapsed);
  
  TEEC_CloseSession(&sess);
  TEEC_ReleaseSharedMemory(&shm);
  TEEC_FinalizeContext(&ctx);
}

/**
 * Computes floor(2 * m * log10(q)) with 100% bit-exact determinism.
 * 
 * @param m Signed 64-bit scaling factor (can be positive, negative, or zero).
 * @param q Unsigned 64-bit integer log argument (must be > 0).
 * @return The deterministic floor result as an int64_t.
 */
int64_t deterministic_floor_2m_log10(int64_t m, uint64_t q) {
    // Handle mathematical edge cases safely
    if (q <= 1 || m == 0) {
        return 0; 
    }

    // 1. Get the integer part of log2(q) via leading zeros count
    int e = 63 - __builtin_clzll(q);
    
    // 2. Compute the fractional bits of log2(q) using fixed-point bit-by-bit squaring.
    // We normalize q to a 1.62 fixed-point format (62 fractional bits).
    unsigned __int128 f = ((unsigned __int128)q) << (62 - e);
    unsigned __int128 log2_frac = 0;
    unsigned __int128 bit = (unsigned __int128)1 << 62;
    
    // Extract 62 bits of highly precise fractional space
    for (int i = 0; i < 62; i++) {
        f = (f * f) >> 62;
        if (f >= ((unsigned __int128)2 << 62)) {
            f >>= 1;
            log2_frac |= bit;
        }
        bit >>= 1;
    }
    
    // Combine integer and fractional parts (Total value is scaled by 2^62)
    unsigned __int128 total_log2 = ((unsigned __int128)e << 62) + log2_frac;
    
    // 3. Convert from log2 to log10 by multiplying by log10(2)
    // Constant definition: log10(2) * 2^62 ≈ 1388255554668266227
    const unsigned __int128 C_LOG10_2 = 1388255554668266227ULL;
    unsigned __int128 total_log10 = (total_log2 * C_LOG10_2) >> 62; // Remains scaled by 2^62
    
    // 4. Multiply by 2 * |m| before evaluating the floor boundary
    unsigned __int128 abs_m = (m < 0) ? -m : m;
    unsigned __int128 intermediate = total_log10 * 2 * abs_m;
    
    // 5. Unscale by shifting right by 62 (This acts as the floor function for positive numbers)
    int64_t final_floor = (int64_t)(intermediate >> 62);
    
    // 6. Handle negative m values to comply with the standard floor function definition:
    // e.g., floor(-2.3) must round down to -3 instead of truncating to -2.
    if (m < 0) {
        unsigned __int128 fractional_mask = ((unsigned __int128)1 << 62) - 1;
        // Check if any fractional remainder bits are set
        if ((intermediate & fractional_mask) != 0) {
            final_floor = -final_floor - 1;
        } else {
            final_floor = -final_floor;
        }
    }
    
    return final_floor;
}

/**
 * Computes floor(2 * m * log10(q)) with 100% bit-exact determinism.
 * Tailored for uint32_t inputs using pure 64-bit integer math.
 * 
 * @param m Unsigned 32-bit scaling factor.
 * @param q Unsigned 32-bit log argument (must be > 0).
 * @return The deterministic floor result as a uint64_t.
 */
uint64_t deterministic_floor_32bit(uint32_t m, uint32_t q) {
    // Handle mathematical edge cases safely
    if (q <= 1 || m == 0) {
        return 0; 
    }

    // 1. Get the integer part of log2(q) via leading zeros count
    // For 32-bit integers, 31 - clz gives the floor of log2(q)
    uint32_t e = 31 - __builtin_clz(q);
    
    // 2. Compute the fractional bits of log2(q) using bit-by-bit squaring.
    // We normalize q to a 1.31 fixed-point format (31 fractional bits).
    uint64_t f = ((uint64_t)q) << (31 - e);
    uint64_t log2_frac = 0;
    uint64_t bit = 1ULL << 31;
    
    // Extract 31 bits of precise fractional space (perfectly fits 64-bit bounds)
    for (int i = 0; i < 31; i++) {
        f = (f * f) >> 31;
        if (f >= (2ULL << 31)) {
            f >>= 1;
            log2_frac |= bit;
        }
        bit >>= 1;
    }
    
    // Combine integer and fractional parts (Total value is scaled by 2^31)
    uint64_t total_log2 = ((uint64_t)e << 31) + log2_frac;
    
    // 3. Convert from log2 to log10 by multiplying by log10(2)
    // Constant definition: log10(2) * 2^31 ≈ 646456993
    const uint64_t C_LOG10_2 = 646456993ULL;
    uint64_t total_log10 = (total_log2 * C_LOG10_2) >> 31; // Remains scaled by 2^31
    
    // 4. Multiply by 2 * m before evaluating the floor boundary
    uint64_t intermediate = total_log10 * 2ULL * m;
    
    // 5. Unscale by shifting right by 31 (This acts as the floor function)
    return intermediate >> 31;
}
