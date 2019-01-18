/*
 * shader.h
 *
 *  Created on: Oct 6, 2018
 *      Author: David
 */

#ifndef SRC_SHADER_HPP_
#define SRC_SHADER_HPP_

#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "global.hpp"

int32_t compile_shader(GLuint *shader, GLenum type, const char shaderFile[]);
int32_t load_shader(const char vertexSource[], const char fragmentSource[]);
void free_shaders(void);
GLuint get_shader_id(void);

#endif /* SRC_SHADER_HPP_ */
