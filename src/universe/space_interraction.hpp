/*
 * space_interraction.h
 *
 *  Created on: Oct 3, 2018
 *      Author: David
 */

#ifndef SRC_SPACE_INTERRACTION_HPP_
#define SRC_SPACE_INTERRACTION_HPP_

#include "controller.hpp"
#include "global.hpp"
#include "graphic.hpp"
#include "tools.hpp"
#include "thread.hpp"
#ifdef CUDA_ENABLE
#include "cuda.h"
#include "kernel.cuh"
#endif

#define DEFAULT_TIME_FACTOR		(1.0f)
#define MASS_FACTOR 			(1.0f)
#define DAMPING_FACTOR 			(300.0f)
#define CONST_FACTOR 			(MASS_FACTOR * DAMPING_FACTOR)

#define NB_THREAD 				4

#ifdef SRC_SPACE_INTERRACTION_C_
float si_timeFactor;
#else
extern float si_timeFactor;
#endif

void space_interraction_init(void);
void space_interraction_free(void);
int compute_new_velocity(void * arg);
void start_make_a_move(void);
void make_a_move(struct universe_t *, float deltaInSec);
void update_blaster_pos(struct blaster_t * blaster, float deltaInSec);
void save_entity_trace(struct entityTrace_t * );
void fire_a_blaster(struct blaster_t *, const struct vector3_t camPos, const struct vector3_t camRot);
void space_interraction_main_loop(void);

#endif /* SRC_SPACE_INTERRACTION_HPP_ */
