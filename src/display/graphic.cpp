/*
 * graphic.c
 *
 *  Created on: Oct 3, 2018
 *      Author: David
 */

#define SRC_GRAPHIC_C_
#include "graphic.hpp"

SDL_Window *sdlMainWindow = NULL;
SDL_GLContext openGLContext = NULL;

/**
 * Init the SDL2 Library
 * @return 0: OK, -1: Error
 */
int32_t init_SDL(void)
{
	SDL_DisplayMode current;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		log_error("Unable to init SDL Video: %s", SDL_GetError());
		return -1;
	}

	if (SDL_GetDesktopDisplayMode(0, &current)) {
		log_error("Unable to get current display mode: %s", SDL_GetError());
		return -1;
	}

	// Only version above 3.2 of OpenGL will be accepted
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Enable swap buffer for the window
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Create the window with SDL and tell it is an openGL content
	sdlMainWindow = SDL_CreateWindow(
			VERSION_STR,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH, WINDOW_HEIGHT,
			SDL_WINDOW_OPENGL |
			SDL_WINDOW_MAXIMIZED |
			SDL_WINDOW_RESIZABLE |
			SDL_WINDOW_ALLOW_HIGHDPI);

	// Check the creation
	if (sdlMainWindow == NULL) {
		log_error("Unable to create the SDL window: %s", SDL_GetError());
		return -1;
	}

	// Get window sizes
	SDL_GetWindowSize(sdlMainWindow, &graphic.screenWidth, &graphic.screenHeight);

	// Hide the cursor
	SDL_ShowCursor(false);

	return 0;
}

/**
 * Init openGL
 * @return 0: OK, -1: Error
 */
int32_t init_openGL(void)
{
	// Create openGL context from the SDL window
	openGLContext = SDL_GL_CreateContext(sdlMainWindow);

	// Check the creation
	if (openGLContext == NULL) {
		log_error("Unable to create the openGl context: %s", SDL_GetError());
		return -1;
	}

	// Show version
	log_info("Open GL Version: %s", glGetString(GL_VERSION));

	// "Should do the magic" fix on embedded board
	glewExperimental = GL_TRUE;

	// Init Glew
	if (glewInit() != GLEW_OK) {
		log_error("Unable to init glew");
		return -1;
	}

	// Load shader files
	if (load_shader("Shaders/basic_opengl_v4.1.vert", "Shaders/basic_opengl_v4.1.frag") != 0) {
		log_error("Unable to load shaders");
		return -1;
	}

	// Enable GL_POINTS resizing in vertex file
	glEnable(GL_PROGRAM_POINT_SIZE);

	// Init matrixes
	set_matrix_identity(graphic.modelview);
	set_matrix_null(graphic.projection);

	// Set initial cursor position
	reset_cursor_position();

	// Set the background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black

	return 0;
}

/**
 * Create the projection matrix with the specified options
 * @param m : Output matrix
 * @param fovy: in degree
 * @param aspect: Ratio between width and height
 * @param znear:
 * @param zfar:
 * @see https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
 */
void build_perspective(float *outProj, float fovY, float aspect, float znear, float zfar)
{
	float f = 1 / tan(to_radians(fovY) / 2);

	outProj[0]  = f / aspect;
	outProj[1]  = 0;
	outProj[2]  = 0;
	outProj[3]  = 0;

	outProj[4]  = 0;
	outProj[5]  = f;
	outProj[6]  = 0;
	outProj[7]  = 0;

	outProj[8]  = 0;
	outProj[9]  = 0;
	outProj[10] = (zfar + znear) / (znear - zfar);
	outProj[11] = -1;

	outProj[12] = 0;
	outProj[13] = 0;
	outProj[14] = (2 * zfar * znear) / (znear - zfar);
	outProj[15] = 0;
}

/**
 * Inititialize the position of the camera
 */
void init_cam_position(void)
{
	graphic.isAxisVisible = true;
	graphic.isGridVisible = true;
	graphic.isUniverseVisible = true;
	graphic.isUniverseMoving = false;
	graphic.isTraceVisible = true;

	// Enable the Star Wars mode by default if option is here
#ifdef STAR_WARS_ENABLE
	graphic.isStarWarsModeEnabled = true;
#else
	graphic.isStarWarsModeEnabled = false;
#endif

	// Start position of the camera
	graphic.camPosition.x = 5;
	graphic.camPosition.y = 5;
	graphic.camPosition.z = 5;

	graphic.camRotation.x = - M_PI / 4;
	graphic.camRotation.y = M_PI / 4;
	graphic.camRotation.z = 0; 				/**< Never used */

	build_perspective(graphic.projection, SETTINGS_FOV, (double) graphic.screenWidth / graphic.screenHeight, SETTINGS_ZNEAR, SETTINGS_ZFAR);
}

/**
 * Set the camera at the world origin
 */
void reset_cam_position(void)
{
	graphic.camRotation.x = 0;
	graphic.camRotation.y = 0;
	graphic.camRotation.z = 0;

	graphic.camPosition.x = 0;
	graphic.camPosition.y = 0;
	graphic.camPosition.z = 0;
}

/**
 * Update the FPS counter
 * /!\ 	Should be called after each frame
 * 		generated to get an accurate FPS counter
 */
void update_fps(void)
{
	// Count the number of pass here (should be the number of frames)
	++graphic.frameCounter;
}

/**
 * Update the label of the window with FPS, rotation and position
 */
void update_window_title(void)
{
	static uint64_t lastFPSUpdate = 0;
	char title[100];
	float fps;

	if (curTime - lastFPSUpdate > UPT_TITLE_PERIOD_US) {
		// Compute FPS
		fps = graphic.frameCounter * 1000000.0f / (curTime - lastFPSUpdate);

		// Save current sate
		lastFPSUpdate = curTime;

		// Previous frames have been managed, clear the counter
		graphic.frameCounter = 0;

		// Update the label
		sprintf(title, VERSION_STR" | FPS = %5.0f | Pos.XYZ= %.1f %.1f %.1f | Rot.XYZ= %.1f° %.1f° %.1f°", fps,
				graphic.camPosition.x, graphic.camPosition.y, graphic.camPosition.z,
				to_degrees(graphic.camRotation.x), to_degrees(graphic.camRotation.y), to_degrees(graphic.camRotation.z));
		SDL_SetWindowTitle(sdlMainWindow, title);
	}

}

/**
 * Reset the position of the cursor in the middle of the screen
 */
void reset_cursor_position(void)
{
	SDL_WarpMouseInWindow(sdlMainWindow, graphic.screenWidth / 2, graphic.screenHeight / 2);
}

/**
 * Free OpenGL (Shaders) and SDL2
 */
void free_graphic(void)
{
	/** Free Shaders */
	free_shaders();

	/** Free OpenGL context */
	SDL_GL_DeleteContext(openGLContext);

	/** Free SDL Window */
	SDL_DestroyWindow(sdlMainWindow);
	SDL_Quit();
}

/**
 * Clear the screen and generate the next frame
 * according to the current parameters
 */
void graphic_main_loop(void)
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	// Display only what user want to see
	if (graphic.isGridVisible) {
		// Grid is not dynamic, no load needed
		show_grid();
	}
	if (graphic.isUniverseVisible) {
		load_universe(&universe);
		show_universe(&universe);
	}
	if (graphic.isStarWarsModeEnabled) {
		load_blasters(&universe);
		show_blasters(&universe);
	}
	if (graphic.isAxisVisible) {
		// Axes are not dynamic, no load needed
		show_axes();
	}
	if (graphic.isTraceVisible) {
		load_entityTrace(&universe);
		show_entityTrace();
	}

	// Update the view throught the shader
	glUniformMatrix4fv(glGetUniformLocation(get_shader_id(), "u_proj_matrix"), 1, GL_FALSE, graphic.projection);
	glUniformMatrix4fv(glGetUniformLocation(get_shader_id(), "u_model_matrix"), 1, GL_FALSE, graphic.modelview);

	// Update the FPS counter
	update_window_title();

	// Update the SDL Window
	SDL_GL_SwapWindow(sdlMainWindow);
}
