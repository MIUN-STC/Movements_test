//#define CAM_ASSERT_F ASSERT_F
//#define CAM_TRACE_F TRACE_F

#include "debug.h"
#include "cam.h"
#include "cam_controller.h"
#include "map.h"
#include "option.h"
#include "control.h"
#include "misc.h"

#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include <inttypes.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define APP_CURVE_COUNT 256
#define APP_SHORT_NAME "Short app name"
#define APP_WINDOW_X SDL_WINDOWPOS_UNDEFINED
#define APP_WINDOW_Y SDL_WINDOWPOS_UNDEFINED
#define APP_WINDOW_HEIGHT 600
#define APP_WINDOW_WIDTH 400

int main (int argc, char * argv [])
{
	struct Gev_Cam cam;
	struct Option opt;
	
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	if (opt.help_flag)
	{
		opt_help (&opt);
		return 0;
	}
	
	{
		int r;
		r = cam_open (&cam);
		if (r) {goto main_error;}
		r = cam_load (&cam, opt.config_file);
		if (r) {goto main_error;}
		r = cam_allocate_frame_buffer (&cam, 4);
		if (r) {goto main_error;}
	}
	
	//SDL Stuff
	SDL_Window * window;
	SDL_Renderer * renderer;
	SDL_Texture * texture;
	SDL_Event event;
	
	{
		int r = SDL_Init (SDL_INIT_VIDEO);
		ASSERT_F (r == 0, "SDL_Init: %s", SDL_GetError());
	}
	
	window = SDL_CreateWindow (APP_SHORT_NAME, APP_WINDOW_X, APP_WINDOW_Y, APP_WINDOW_WIDTH, APP_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
	ASSERT_F (window != NULL, "SDL_CreateWindow: %s", SDL_GetError());
	
	renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	ASSERT_F (renderer != NULL, "SDL_CreateRenderer: %s", SDL_GetError());
	
	texture = create_texture (renderer, &cam);
	ASSERT_F (texture != NULL, "SDL_CreateTexture: %s", SDL_GetError());
	
	SDL_SetRenderDrawColor (renderer, 0, 0, 255, 255);
	SDL_RenderClear (renderer);
	SDL_RenderPresent (renderer);
	
	struct Control ctrl;
	ctrl.exposure = 10000.0f;
	ctrl.exposure_step = 1.0f;
	ctrl.gain = 1.0f;
	ctrl.gain_step = 1.0f;
	ctrl.balance = 70;
	ctrl.mode = MODE_NONE | MODE_SHOW_VIDEO;
	
	cam_set_float (&cam, "ExposureTime", ctrl.exposure);
	cam_set_float (&cam, "Gain", ctrl.gain);

	
			
	while (1) 
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_QUIT: goto main_quit;
				case SDL_KEYDOWN:
				ctrl_input (&ctrl, &cam, &event.key.keysym);
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE: goto main_quit;
				}
				break;
			}
		}
		
		struct Gev_Image * image = NULL;
		uint16_t hmap [APP_CURVE_COUNT];
		SDL_Point curve [APP_CURVE_COUNT];
		
		if ((ctrl.mode & MODE_GET) && (image == NULL))
		{
			cam_read (&cam, &image);
			//TRACE_F ("id %i", (int)image->id);
		}
		
		if ((ctrl.mode & MODE_SHOW_VIDEO) && (image != NULL))
		{
			app_render_image (texture, image);
		}
		
		if ((ctrl.mode & MODE_AUTO_EXPOSURE) && (image != NULL))
		{
			ctrl_auto_exposure2 (&ctrl.exposure, image, ctrl.balance);
			cam_set_float (&cam, "ExposureTime", ctrl.exposure);
			printf ("e %f\n", ctrl.exposure);
		}

		SDL_RenderCopy (renderer, texture, NULL, NULL);
		
		if ((ctrl.mode & MODE_SAVE_IMAGE) && (image != NULL))
		{
			app_save_image (&opt, &ctrl, texture);
		}
		
		if ((ctrl.mode & MODE_SAVE_IMAGE_ONCE) && (image != NULL))
		{
			ctrl.mode ^= MODE_SAVE_IMAGE_ONCE;
			app_save_image (&opt, &ctrl, texture);
		}
		
		if (ctrl.mode & MODE_SHOW_HISTOGRAM)
		{
			if (image) {count_u8_u16 (image->address, image->recv_size, hmap, APP_CURVE_COUNT);}
			app_render_curve (window, hmap, curve, APP_CURVE_COUNT);
		}
		
		if (image)
		{
			cam_release (&cam, image);
			image = NULL;
		}
		
		
		
		if (ctrl.mode & MODE_SHOW_CROSS)
		{
			app_render_cross (window);
		}
		

		SDL_RenderPresent (renderer);
		
	}

main_quit:

	SDL_DestroyTexture (texture);
	SDL_DestroyRenderer (renderer);
	SDL_Quit();
	cam_close (&cam);
	return 0;
	
main_error:
	
	cam_close (&cam);
	return 1;
	
}


