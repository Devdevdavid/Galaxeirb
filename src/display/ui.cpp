/*
 * ui.c
 *
 *  Created on: Oct 8, 2018
 *      Author: David
 */

#define SRC_UI_C_
#include "ui.hpp"

/**
 * Init the global structure
 */
void init_ui(void)
{
	ui.isRightKeyPressed = false;
	ui.isLeftKeyPressed = false;
	ui.isUpKeyPressed = false;
	ui.isDownKeyPressed = false;
	ui.isFlyKeyPressed = false;
	ui.isSneakKeyPressed = false;
	ui.autoMove.isEnabled = false;

	ui.mouseMove.x = 0;
	ui.mouseMove.y = 0;

	ui.quit = false;
}

/**
 * Update the global structure according to
 * events reported by SDL
 */
void manage_event(void)
{
	SDL_Event event;
	unsigned int e;

	while (SDL_PollEvent(&event)) {
		e = event.type;

		if (e == SDL_MOUSEMOTION) {
			ui.mouseMove.x += graphic.screenWidth / 2 - event.motion.x;
			ui.mouseMove.y += graphic.screenHeight / 2 - event.motion.y;
			reset_cursor_position();
		}
		else if (e == SDL_MOUSEWHEEL) {
			if (event.wheel.y > 0) {
				si_timeFactor /= 1.01f;
			}
			else if (event.wheel.y < 0) {
				si_timeFactor *= 1.01f;
			}
		}
		else if (e == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_F1) {
				graphic.isAxisVisible = !graphic.isAxisVisible;
			}
			else if (event.key.keysym.sym == SDLK_F2) {
				graphic.isGridVisible = !graphic.isGridVisible;
			}
			else if (event.key.keysym.sym == SDLK_F3) {
				graphic.isUniverseVisible = !graphic.isUniverseVisible;
			}
			else if (event.key.keysym.sym == SDLK_F4) {
				reset_cam_position();
				graphic.isAxisVisible = false;
				graphic.isGridVisible = false;
			}
			else if (event.key.keysym.sym == SDLK_F5) {
				set_next_universe_color();
			}
			else if (event.key.keysym.sym == SDLK_F6) {
				graphic.isUniverseMoving = !graphic.isUniverseMoving;
				start_make_a_move();
			}
			else if (event.key.keysym.sym == SDLK_F7) {
				graphic.isTraceVisible = !graphic.isTraceVisible;
			}
			else if (event.key.keysym.sym == SDLK_F12) {
				graphic.isStarWarsModeEnabled = !graphic.isStarWarsModeEnabled;
			}
			else if (event.key.keysym.sym == SDLK_d) {
				ui.isRightKeyPressed = true;
			}
			else if (event.key.keysym.sym == SDLK_q) {
				ui.isLeftKeyPressed = true;
			}
			else if (event.key.keysym.sym == SDLK_z) {
				ui.isUpKeyPressed = true;
			}
			else if (event.key.keysym.sym == SDLK_s) {
				ui.isDownKeyPressed = true;
			}
			else if (event.key.keysym.sym == SDLK_n) {
				ui.isFlyKeyPressed = true;
			}
			else if (event.key.keysym.sym == SDLK_b) {
				ui.isSneakKeyPressed = true;
			}
			else if (event.key.keysym.sym == SDLK_f) {
				ui.isFireBtnPressed = true;
			}
			else if (event.key.keysym.sym == SDLK_a) {
				ui.autoMove.isEnabled = !ui.autoMove.isEnabled;
				ui.autoMove.centerPosition = graphic.camPosition;
				ui.autoMove.startRotation = graphic.camRotation;
				ui.autoMove.radius = AUTO_MOVE_DEF_RADIUS;
				ui.autoMove.startTimeUs = curTime;
			}
			else if (event.key.keysym.sym == SDLK_ESCAPE) {
				ui.quit = true;
			}
		}
		else if (e == SDL_KEYUP) {
			if (event.key.keysym.sym == SDLK_d) {
				ui.isRightKeyPressed = false;
			}
			else if (event.key.keysym.sym == SDLK_q) {
				ui.isLeftKeyPressed = false;
			}
			else if (event.key.keysym.sym == SDLK_z) {
				ui.isUpKeyPressed = false;
			}
			else if (event.key.keysym.sym == SDLK_s) {
				ui.isDownKeyPressed = false;
			}
			else if (event.key.keysym.sym == SDLK_n) {
				ui.isFlyKeyPressed = false;
			}
			else if (event.key.keysym.sym == SDLK_b) {
				ui.isSneakKeyPressed = false;
			}
			else if (event.key.keysym.sym == SDLK_f) {
				ui.isFireBtnPressed = false;
			}
		}

		// Red cross of the window
		if (e == SDL_QUIT) {
			ui.quit = true;
		}
	}

	// Mouse buttons
	ui.isRightBtnPressed = (SDL_GetMouseState(0, 0) & SDL_BUTTON_RMASK) != 0;
	ui.isLeftBtnPressed = (SDL_GetMouseState(0, 0) & SDL_BUTTON_LMASK) != 0;


}

/**
 * Update the camera according to the events captured
 */
void move_camera(void)
{
	static struct vector2_t oldMouseMove;
	static uint64_t lastCamMove = 0;
	float autoMoveTeta;
	float distanceToMove;
	struct vector3_t direction = {0.0f, 0.0f, 0.0f};

	/** Rotation */
	// X is common
	graphic.camRotation.x -= (oldMouseMove.y - ui.mouseMove.y) / 1000;

	// Y is managed differently according to the auto move
	if (ui.autoMove.isEnabled == false) {
		graphic.camRotation.y -= (oldMouseMove.x - ui.mouseMove.x) / 1000;
	} else {
		graphic.camRotation.y = - ((float) (curTime - ui.autoMove.startTimeUs)) / 1000.0f / AUTO_MOVE_PERIOD_MS * 2 * M_PI ;
	}

	/** Limit rotation */
	if (graphic.camRotation.x > (M_PI / 2)) {
		graphic.camRotation.x = (M_PI / 2);
	}
	else if (graphic.camRotation.x < -(M_PI / 2)) {
		graphic.camRotation.x = -(M_PI / 2);
	}

	/** Keep the value in [0.0, 2 * M_PI] */
	if (graphic.camRotation.y > (2 * M_PI)) {
		graphic.camRotation.y -= 2 * M_PI;
	} else if (graphic.camRotation.y < 0.0f) {
		graphic.camRotation.y += 2 * M_PI;
	}

	/** Position */
	if (ui.isRightKeyPressed) 	{ direction.x += 1; }
	if (ui.isLeftKeyPressed) 	{ direction.x -= 1; }
	if (ui.isUpKeyPressed) 		{ direction.z -= 1; }
	if (ui.isDownKeyPressed) 	{ direction.z += 1; }
	if (ui.isFlyKeyPressed) 	{ direction.y += 1; }
	if (ui.isSneakKeyPressed) 	{ direction.y -= 1; }

	if (direction.x != 0 || direction.y != 0 || direction.z != 0) {
		/** Compute the distance according to the speed */
		distanceToMove = ((float) (curTime - lastCamMove)) / 1000000.0f * NOMINAL_CAM_SPEED;

		/** Normalize the direction to conserve same speed througt all axes */
		normalize_vector3(&direction);

		if (graphic.isStarWarsModeEnabled) {
			rotate_landmark(&direction, direction, graphic.camRotation);

			graphic.camPosition.x += distanceToMove * direction.x;
			graphic.camPosition.y += distanceToMove * direction.y;
			graphic.camPosition.z += distanceToMove * direction.z;

		} else {
			/** Apply the move to position */
			graphic.camPosition.y += direction.y * distanceToMove;

			if (ui.autoMove.isEnabled == false) {
				/** Change point of origin (From camera to world origin)
				 * and save result in the same variable (That's bad but efficient) */
				rotate_landmark_y(&direction, direction, graphic.camRotation.y);

				graphic.camPosition.x += direction.x * distanceToMove;
				graphic.camPosition.z += direction.z * distanceToMove;
			} else {
				ui.autoMove.radius += direction.z * distanceToMove;
			}
		}
	}

	if (ui.autoMove.isEnabled == true) {
		autoMoveTeta = ((float) (curTime - ui.autoMove.startTimeUs)) / 1000.0f / AUTO_MOVE_PERIOD_MS * 2 * M_PI ;
		graphic.camPosition.x = ui.autoMove.centerPosition.x - ui.autoMove.radius * sin(0 + autoMoveTeta);
		graphic.camPosition.z = ui.autoMove.centerPosition.z + ui.autoMove.radius * cos(0 + autoMoveTeta);
	}

	/** Compute the new modelview */
	get_fps_modelview(graphic.modelview, graphic.camPosition, graphic.camRotation.x, graphic.camRotation.y);

	/** Save state for next loop */
	oldMouseMove = ui.mouseMove;
	lastCamMove = curTime;
}

/**
 * Get change the color mode of the universe
 */
void set_next_universe_color(void)
{
	static enum color_mode_e universeColorMode = WHITE;

	if (universeColorMode == WHITE) {
		universeColorMode = MASS;
	}
	else if (universeColorMode == MASS) {
		universeColorMode = RANDOM;
	} else {
		universeColorMode = WHITE;
	}
	color_entities(&universe, universeColorMode);
}

void manage_action(void)
{
	if (graphic.isStarWarsModeEnabled) {
		if (ui.isFireBtnPressed) {
			fire_a_blaster(&universe.blaster, graphic.camPosition, graphic.camRotation);
		}
	}
}
