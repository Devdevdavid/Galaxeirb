#define SRC_MAIN_C_

#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "controller.hpp"
#include "cuda/cuda_lib.hpp"
#include "global.hpp"
#include "graphic.hpp"
#include "shader.hpp"
#include "ui.hpp"

void app_quit(void);

/**
 * Initialize the application
 * @return
 */
int32_t app_init(void)
{
	/** Init time */
	curTime = getTimeOfDayInUs();

	/** Init LOG File */
	log_set_fp(LOG_FILE_PATH);
	log_info("=== Starting %s ===", VERSION_STR);

	/** SDL Initialization */
	if (init_SDL() != 0) {
		log_error("Unable to initialize SDL correctly");
		return -1;
	}

	/** OpenGL Initialization */
	if (init_openGL() != 0) {
		log_error("Unable to initialize openGL correctly");
		return -1;
	}

	/** Universe Initialization */
	if (init_universe(&universe) != 0) {
		return -1;
	}

	/** Init user interface */
	init_ui();

	/** Init camera */
	init_cam_position();

	/** Init the renderer */
	init_render(&universe);

	/** Init interractions */
	space_interraction_init();

#ifdef CUDA_ENABLE
	/** CUDA Initialisation */
	if (init_cuda_lib(&universe) != 0) {
		log_error("Unable to initialize GPU device correctly");
		return -1;
	}
#endif

	return 0;
}

/**
 * Free all memories and buffers
 * allocated at init stage
 */
void app_quit(void)
{
#ifdef CUDA_ENABLE
	/** Free GPU device **/
	cuda_lib_reset();
#endif

	/** Free Renderer */
	free_render();

	/** Free Threads */
	space_interraction_free();

	/** Free universe */
	free_universe(&universe);

	/** Free SDL and OpenGL */
	free_graphic();

	log_info("App exit");
}

/**
 * Main loop of the app
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char ** argv)
{
	uint64_t nextRefresh = 0;

	if (app_init() != 0) {
		app_quit();
		return -1;
	}

	while (!ui.quit) {
		curTime = getTimeOfDayInUs();

		space_interraction_main_loop();
		update_fps();

		if (curTime >= nextRefresh) {
			nextRefresh = curTime + (1000000 / DISPLAY_FRAME_RATE);
			manage_event();
			move_camera();
			manage_action();
			graphic_main_loop();
		}
	}

	app_quit();

	return 0;
}
