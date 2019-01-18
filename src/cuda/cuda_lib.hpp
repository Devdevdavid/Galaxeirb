/*
 * cuda.h
 *
 *  Created on: 9 oct. 2018
 *      Author: aurel
 */

#ifndef SRC_CUDA_HPP_
#define SRC_CUDA_HPP_

#ifdef CUDA_ENABLE

#include "global.hpp"
#include "cuda_runtime.h"
#include "cuda.h"
#include "kernel.cuh"


int32_t init_cuda_lib(struct universe_t *);
int32_t cuda_lib_reset();
int32_t cuda_lib_malloc(void ** devPtr, size_t size);
int32_t cuda_lib_free(void * devPtr);
int32_t cuda_lib_memcpy( void * dst, const void * src, size_t count, enum cudaMemcpyKind kind );
int32_t cuda_lib_memset(void * devPtr, int value, size_t count);
#endif /* CUDA_ENABLE */

#endif /* SRC_CUDA_HPP_ */
