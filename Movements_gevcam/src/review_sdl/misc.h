#pragma once

#include "debug.h"
#include "cam.h"
#include "filters.h"
#include "map.h"

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



void app_render_cross (SDL_Window * window)
{
	SDL_Renderer * renderer = SDL_GetRenderer (window);
	int W;
	int H;
	SDL_GetWindowSize (window, &W, &H);
	SDL_SetRenderDrawColor (renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine (renderer, W/2, 0, W/2, H);
	SDL_RenderDrawLine (renderer, 0, H/2, W, H/2);
}


void app_render_curve (SDL_Window * window, uint16_t * hmap, SDL_Point * curve, size_t count)
{
	SDL_Renderer * renderer = SDL_GetRenderer (window);
	int W;
	int H;
	SDL_GetWindowSize (window, &W, &H);
	SDL_SetRenderDrawColor (renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	for (size_t I = 0; I < count; I = I + 1)
	{
		curve [I].x = map_lin_int (I, 0, count - 1, 0, W - 1);
		curve [I].y = H - 1 - map_lin_int (hmap [I], 0, UINT16_MAX, 0, H - 1);
	}
	SDL_RenderDrawLines (renderer, curve, count);
}


void app_render_image (SDL_Texture * texture, struct Gev_Image * image)
{
	if (image == NULL) {return;}
	int pitch;
	int width;
	int height;
	struct RGBA8888 * rgba = NULL;
	int R;
	R = SDL_LockTexture (texture, NULL, (void *) &rgba, &pitch);
	ASSERT_F (R == 0, "SDL_LockTexture: %s", SDL_GetError());
	R = SDL_QueryTexture (texture, NULL, NULL, &width, &height);
	rgba8888_unfiltered (image->w, image->h, image->address, rgba);	
	SDL_UnlockTexture (texture);
}



void app_set_file_name (struct Control * ctrl, char * buffer, size_t length, char const * name)
{
	time_t raw;
	struct tm * T;
	time (&raw);
	T = localtime (&raw);
	snprintf 
	(
		buffer, 
		length, 
		"%04i%02i%02i_%02i%02i%02i_g%02i_e%06i_%s.png", 
		T->tm_year + 1900,
		T->tm_mon + 1,
		T->tm_mday,
		T->tm_hour,
		T->tm_min,
		T->tm_sec,
		(int) ctrl->gain,
		(int) ctrl->exposure,
		name
	);
}


void app_save_image (struct Option * opt, struct Control * ctrl, SDL_Texture * texture)
{
	char buffer [100];
	int pitch;
	int width;
	int height;
	struct RGBA8888 * rgba = NULL;
	int R;
	R = SDL_LockTexture (texture, NULL, (void *) &rgba, &pitch);
	ASSERT_F (R == 0, "SDL_LockTexture: %s", SDL_GetError());
	SDL_UnlockTexture (texture);
	R = SDL_QueryTexture (texture, NULL, NULL, &width, &height);
	ASSERT_F (R == 0, "SDL_LockTexture: %s", SDL_GetError());
	SDL_Surface * surface = SDL_CreateRGBSurfaceFrom (rgba, width, height, 32, pitch, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	ASSERT_F (surface != NULL, "%s", "");
	app_set_file_name (ctrl, buffer, sizeof (buffer), opt->image_filename);
	R = IMG_SavePNG (surface, buffer);
	SDL_FreeSurface (surface);
	ASSERT_F (R == 0, "%s", "");
}


SDL_Texture * create_texture (SDL_Renderer * renderer, struct Gev_Cam * cam)
{
	SDL_Texture * texture;
	uint32_t width;
	uint32_t height;
	cam_get_u32 (cam, "Width", &width);
	cam_get_u32 (cam, "Height", &height);
	texture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	return texture;
}
