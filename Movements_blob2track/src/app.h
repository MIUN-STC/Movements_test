#pragma once

#include <stdio.h>
#include <unistd.h>

#include "SDLGL.h"
#include "global.h"
#include "debug.h"
#include "debug_gl.h"
#include "camera.h"
#include "ui.h"
#include "bufv.h"


#define APP_QUIT 1U << 1
#define APP_ERROR 1U << 2
#define APP_WINDOW_WIDTH 1024
#define APP_WINDOW_HEIGHT 768
#define APP_TITLE "3D plot of (x y t)"



void app_init (uint32_t *flags)
{
	{
		int r = SDL_Init (SDL_INIT_VIDEO);
		ASSERT_F (r == 0, "SDL_Init %i\n", SDL_GetError ());
		(*flags) |= (r & APP_ERROR);
	}
}



void app_update_camera (struct Cam * cam, uint8_t const * keyboard)
{
	float p1 [4];
	float q1 [4];
	sdl_get_translation_vector (p1, keyboard);
	sdl_get_rotation_vector (q1, keyboard);
	cam_update (cam, p1, q1);
	float mm [16];
	m4f32_identity (mm);
	cam_mvp_update (cam, mm);
}


