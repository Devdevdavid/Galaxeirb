/*
 * controler.h
 *
 *  Created on: Oct 3, 2018
 *      Author: David
 */

#ifndef SRC_CONTROLLER_HPP_
#define SRC_CONTROLLER_HPP_

#include "global.hpp"
#include "space_interraction.hpp"
#include "tools.hpp"
#ifdef CUDA_ENALBLE
#include "cuda.h"
#endif

#ifdef SRC_CONTROLLER_C_
struct universe_t universe;
#else
extern struct universe_t universe;
#endif

int32_t init_universe(struct universe_t *);
void init_entityTrace(struct entityTrace_t *, struct entity_t *);
void init_entityBlaster(struct blaster_t * entityBlaster);
void free_universe(struct universe_t *);
int32_t tie_fighter_entity_data(struct universe_t * universe);
int32_t parse_file_entity_data(struct universe_t *, const char pathToFile[]);
int32_t color_entities(struct universe_t *universe, enum color_mode_e color_mode);

#endif /* SRC_CONTROLLER_HPP_ */
