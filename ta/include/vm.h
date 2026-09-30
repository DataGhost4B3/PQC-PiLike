// ta/include/vm.h
// author: somnathkarmakar1203@gmail.com

#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <tee_internal_api.h>
#include <tee_api_types.h>
#include <trace.h>
#include <pilike.h>

extern uint32_t D[];

TEE_Result vm(const uint32_t *x, uint32_t *outv, uint32_t n, uint32_t q);

TEE_Result gen_d(uint32_t *v, uint32_t n, uint32_t q);

#endif /* VM_H */
