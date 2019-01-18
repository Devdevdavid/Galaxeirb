/*
 * graphic.h
 *
 *  Created on: Oct 3, 2018
 *      Author: David
 */

#ifndef SRC_GRAPHIC_HPP_
#define SRC_GRAPHIC_HPP_

#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "controller.hpp"
#include "global.hpp"
#include "render.hpp"
#include "shader.hpp"

#define WINDOW_HEIGHT			480
#define WINDOW_WIDTH			640

#define UPT_TITLE_PERIOD_US		(100 * 1000)

// View Settings
#define SETTINGS_ZNEAR	0.1f
#define SETTINGS_ZFAR	150.0f
#define SETTINGS_FOV	70.0f

struct graphic_t {
	bool isAxisVisible;
	bool isGridVisible;
	bool isUniverseVisible;
	bool isUniverseMoving;
	bool isTraceVisible;
	bool isStarWarsModeEnabled;

	struct vector3_t camPosition;		/**< Position of the camera related to world origin */
	struct vector3_t camRotation;		/**< Rotation of the camera related to world origin */

	int32_t screenWidth;
	int32_t screenHeight;

	float modelview[16];				/**< Matrix model view */
	float projection[16];				/**< Rendering matrix */

	uint32_t frameCounter;

};

#ifdef SRC_GRAPHIC_C_
struct graphic_t graphic;
#else
extern struct graphic_t graphic;
#endif

int32_t init_SDL(void);
int32_t init_openGL(void);
void build_perspective(float *m, float fovy, float aspect, float znear, float zfar);
void init_cam_position(void);
void reset_cam_position(void);
void update_fps(void);
void update_window_title(void);
void reset_cursor_position(void);
void free_graphic(void);
void graphic_main_loop(void);

#endif /* SRC_GRAPHIC_HPP_ */
