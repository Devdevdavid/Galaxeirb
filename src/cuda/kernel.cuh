#ifndef __KERNEL_CUH__
#define __KERNEL_CUH__

#include <sys/time.h>
#include "cuda.h"
#include "cuda_runtime.h"
#include "cuda_lib.hpp"
#include "global.hpp"
#include "tools.hpp"

#define NB_PHY_BLOCK	192

uint32_t cuda_allocate(struct universe_t *universe);
void cuda_start_make_a_move(void);
void cuda_make_a_move(struct universe_t *universe, float constFactor, float si_timeFactor);
void cuda_kernel_free(void);
void cuda_free(void);

#endif