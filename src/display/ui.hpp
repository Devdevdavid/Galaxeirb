/*
 * ui.h
 *
 *  Created on: Oct 8, 2018
 *      Author: David
 */

#ifndef SRC_UI_HPP_
#define SRC_UI_HPP_

#include <time.h>

#include "global.hpp"
#include "graphic.hpp"
#include "space_interraction.hpp"

#define NOMINAL_CAM_SPEED 		10.0f					// Length units per second
#define AUTO_MOVE_PERIOD_MS		30000.0f				// Period in seconds
#define AUTO_MOVE_DEF_RADIUS	30.0f					// Length units

struct autoMove_t {
	bool isEnabled;
	struct vector3_t centerPosition;
	struct vector3_t startRotation;
	uint64_t startTimeUs;
	float radius;
};

struct ui_t {
	struct autoMove_t autoMove;

	bool isUpKeyPressed;
	bool isDownKeyPressed;
	bool isLeftKeyPressed;
	bool isRightKeyPressed;

	bool isFlyKeyPressed;
	bool isSneakKeyPressed;

	bool isLeftBtnPressed;
	bool isRightBtnPressed;

	bool isFireBtnPressed;

	struct vector2_t mouseMove;

	bool quit;
};

#ifdef SRC_UI_C_
struct ui_t ui;
#else
extern struct ui_t ui;
#endif

void init_ui(void);
void manage_event(void);
void move_camera(void);
void set_next_universe_color(void);
void manage_action(void);


#endif /* SRC_UI_HPP_ */
