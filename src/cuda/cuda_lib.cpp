/*
 * cuda.c
 *
 *  Created on: 8 oct. 2018
 *      Author: aurel
 */

#ifdef CUDA_ENABLE
#include "cuda_lib.hpp"

/**
 * init cuda device
 * @return 0: OK, -1: unable to setup cuda device
 */
int32_t init_cuda_lib(struct universe_t * universe) {
	cudaError_t cuda_status;

	cuda_status = cudaSetDevice(0);
	if (cuda_status != cudaSuccess) {
		log_error("init_failed");
		return -1;
	}

	if (cuda_allocate(universe) != 0) {
		return -1;
	}

	return 0;
}

/**
 * init cuda device
 * @return 0: OK, -1: Unable to reset device
 */
int32_t cuda_lib_reset(void) {
	cudaError_t cuda_status;

	cuda_free();

	cuda_status = cudaDeviceReset();
	if (cuda_status != cudaSuccess) {
		log_error("reset_failed");
		return -1;
	}

	return 0;
}

/**
 * malloc for cuda ram
 * @param devPtr
 * @param size
 * @return 0: OK, -1: unable to allocate buffer
 */
int32_t cuda_lib_malloc(void ** devPtr, size_t size) {
	cudaError_t cuda_status;

	cuda_status = cudaMalloc(devPtr, size);
	if (cuda_status != cudaSuccess) {
		log_error("malloc_failed");
		return -1;
	}

	return 0;
}

/**
 * malloc for cuda ram
 * @param devPtr
 * @param size
 * @return 0: OK, -1: unable to free buffer
 */
int32_t cuda_lib_free(void * devPtr) {
	cudaError_t cuda_status;

	cuda_status = cudaFree(devPtr);
	if (cuda_status != cudaSuccess) {
		log_error("free_failed");
		return -1;
	}

	return 0;
}

/**
 * memcpy between GPU ram and CPU ram
 * @dst
 * @src
 * @count
 * @cudaMencpyKind : cudaMemcpyHostToDevice or cudaMemcpyDeviceToHost (Device = GPU ram, host = CPU ram)
 * @return 0: OK, -1: unable to copy buffer
 */
int32_t cuda_lib_memcpy(void * dst, const void * src, size_t count, enum cudaMemcpyKind kind) {
	cudaError_t cudaStatus;

	cudaStatus = cudaMemcpy(dst, src, count, kind);
	if (cudaStatus != cudaSuccess) {
		log_error("memcpy_failed");
		return -1;
	}

	return 0;
}

int32_t cuda_lib_memset(void * devPtr, int value, size_t count) {
	cudaError_t cudaStatus;

	cudaStatus = cudaMemset(devPtr, value, count);
	if (cudaStatus != cudaSuccess) {
		log_error("memset_failed");
		return -1;
	}

	return 0;
}

#endif
