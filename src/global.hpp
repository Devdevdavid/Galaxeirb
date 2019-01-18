/*
 * global.h
 *
 *  Created on: Oct 3, 2018
 *      Author: David
 */

#ifndef SRC_GLOBAL_HPP_
#define SRC_GLOBAL_HPP_

#define VERSION 				"v0.2.8"

#ifndef DEBUG
#define VERSION_STR				"GalaXeirb "VERSION
#else
#define VERSION_STR				"GalaXeirb "VERSION" DEBUG"
#endif

#define LOG_FILE_PATH			"galaxeirb.log"
#define ENTITY_TRACE_COUNT		50
#define ENTITY_TRACE_LENGTH		255
#define BLASTER_LENGTH			(32 * 2)
#define BLASTER_MIN_PERIOD_IN_SEC	0.3f
#define NOMINAL_BLASTER_SPEED 	30.0f				// Length units per second
#define DISPLAY_FRAME_RATE		60					// Frame per second

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include "log.hpp"

struct colorRGB_t {
	float r;
	float g;
	float b;
};

struct vector3_t {
	float x;
	float y;
	float z;
};

struct vector2_t {
	float x;
	float y;
};

struct entity_t {
	float * mass;
	struct vector3_t * pos;
	struct vector3_t * currVelocity;
	struct vector3_t * newVelocity;
};

struct entityTrace_t {
	struct entity_t * entity;			// entity followed

	struct vector3_t position[ENTITY_TRACE_LENGTH];		// Float array of entities positions history [x0,y0,z0][x1,y1,z1]...
	struct colorRGB_t color[ENTITY_TRACE_LENGTH];		// Float array of entities colors [r0,g0,b0][r1,g1,b1]...
};

struct blaster_t {
	uint16_t curEntity;
	uint64_t lastFire;									// Instant of the last fire
	struct vector3_t position[BLASTER_LENGTH];			// Float array of entities positions history [x0,y0,z0][x1,y1,z1]...
	struct vector3_t currVelocity[BLASTER_LENGTH];		// Value of the current velocity of the blaster entity
	struct colorRGB_t color[BLASTER_LENGTH];			// Float array of entities colors [r0,g0,b0][r1,g1,b1]...
};

struct universe_t {
	uint32_t entityCount;				// Number of entity in the universe
	struct entity_t * entities;			// List of all entities

	float * mass;						// Float array of entities mass [m0,m1,m2,...]
	struct vector3_t * position;		// Float array of entities positions [x0,y0,z0][x1,y1,z1]...
	struct vector3_t * currVelocity;	// Float array of current entities velocities [vx0,vy0,vz0][vx1,vy1,vz1]...
	struct vector3_t * newVelocity;		// Float array of new entities velocities [vx0,vy0,vz0][vx1,vy1,vz1]...
	struct colorRGB_t * color;			// Float array of entities colors [r0,g0,b0][r1,g1,b1]...
	bool isColorNeedLoad;				// Indicates color changed and need to be reloaded

	struct entityTrace_t * entityTrace; // Array of entityTrace to have a position history of some entities
	struct blaster_t blaster; 			// Blaster for star wars mode
};

enum color_mode_e {
	WHITE,
	MASS,
	RANDOM
};

#ifdef SRC_MAIN_C_
uint64_t curTime;
#else
extern uint64_t curTime;
#endif

#endif /* SRC_GLOBAL_HPP_ */
