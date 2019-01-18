/*
 * render.h
 *
 *  Created on: Oct 8, 2018
 *      Author: David
 */

#ifndef SRC_RENDER_HPP_
#define SRC_RENDER_HPP_

#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "global.hpp"
#include "shader.hpp"

#define VAO_NUMBER						(4 + ENTITY_TRACE_COUNT)

#define VAO_INDEX_AXES					0
#define VAO_INDEX_GRID					1
#define VAO_INDEX_UNIVERSE				2
#define VAO_INDEX_ENTITIES_TRACE		3
#define VAO_INDEX_BLASTERS				4

#define VBO_INDEX_POSITION				0
#define VBO_INDEX_COLOR					1

struct vao_t {
	GLuint vaoIdTable[VAO_NUMBER];
	GLuint vboIdTable[VAO_NUMBER * 2]; // 2: Positions and colors
};

void init_render(struct universe_t * universe);
void free_render(void);
void load_axes(void);
void show_axes(void);
void load_grid(uint8_t sqareCount, float width);
void show_grid(void);
void load_universe(struct universe_t *);
void show_universe(struct universe_t *);
void load_entityTrace(struct universe_t *);
void show_entityTrace(void);
void load_blasters(struct universe_t *);
void show_blasters(struct universe_t *);

#endif /* SRC_RENDER_HPP_ */
