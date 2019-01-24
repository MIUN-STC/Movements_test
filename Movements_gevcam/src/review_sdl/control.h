#pragma once

#include "debug.h"
#include "cam.h"
//#include "filters.h"
#include "map.h"
#include "common.h"

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

#define MODE_NONE 0x1
#define MODE_CTRL_EXPOSURE 0x2
#define MODE_CTRL_GAIN 0x4


#define MODE_GET 0x20
#define MODE_SHOW_HISTOGRAM 0x40
#define MODE_SHOW_CROSS 0x80
#define MODE_SHOW_VIDEO 0x100

#define MODE_AUTO_EXPOSURE 0x200
#define MODE_AUTO_GAIN 0x400

#define MODE_SAVE_IMAGE 0x1000
#define MODE_SAVE_IMAGE_ONCE 0x2000


struct Control
{
	//Used for auto or manual exposure control.
	float exposure;
	//Step size for manual exposure control in pow10 scale.
	int exposure_step;
	//Used for auto or manual gain control.
	float gain;
	//Step size for manual gain control in pow10 scale.
	int gain_step;
	//The balance for exposure.
	float balance;
	//A set of modes where each mode can be enabled or disable.
	int mode;
	//Used to check if the mode has changed.
	int mode_old;
};


void ctrl_input (struct Control * ctrl, struct Gev_Cam * cam, SDL_Keysym * key)
{
	int arrow_x = 0;
	int arrow_y = 0;
	switch (key->sym)
	{
		case SDLK_SPACE:
		ctrl->mode ^= MODE_GET;
		if (ctrl->mode & MODE_GET) 
		{
			cam_start (cam);
		}
		else
		{
			cam_stop (cam);
		}
		break;
		
		case SDLK_5:
		ctrl->mode ^= MODE_AUTO_EXPOSURE;
		break;
		
		case SDLK_6:
		ctrl->mode ^= MODE_AUTO_GAIN;
		break;
		
		case SDLK_e:
		ctrl->mode ^= MODE_CTRL_EXPOSURE;
		break;
		
		case SDLK_g:
		ctrl->mode ^= MODE_CTRL_GAIN;
		break;
		
		case SDLK_s:
		ctrl->mode ^= MODE_SAVE_IMAGE;
		break;
		
		case SDLK_p:
		ctrl->mode ^= MODE_SAVE_IMAGE_ONCE;
		break;
		
		case SDLK_c:
		ctrl->mode ^= MODE_SHOW_CROSS;
		break;
		
		case SDLK_h:
		ctrl->mode ^= MODE_SHOW_HISTOGRAM;
		break;
		
		case SDLK_LEFT:
		arrow_x = 1;
		break;
		
		case SDLK_RIGHT:
		arrow_x = -1;
		break;
		
		case SDLK_UP:
		arrow_y = 1;
		break;
		
		case SDLK_DOWN:
		arrow_y = -1;
		break;
	}
	
	if (ctrl->mode != ctrl->mode_old)
	{
		if (ctrl->mode & MODE_CTRL_EXPOSURE) {printf ("MODE_CTRL_EXPOSURE | ");}
		if (ctrl->mode & MODE_CTRL_GAIN) {printf ("MODE_CTRL_GAIN | ");}
		if (ctrl->mode & MODE_GET) {printf ("MODE_GET | ");}
		if (ctrl->mode & MODE_AUTO_EXPOSURE) {printf ("MODE_AUTO_EXPOSURE | ");}
		if (ctrl->mode & MODE_AUTO_GAIN) {printf ("MODE_AUTO_GAIN | ");}
		if (ctrl->mode & MODE_SHOW_CROSS) {printf ("MODE_SHOW_CROSS | ");}
		if (ctrl->mode & MODE_SHOW_HISTOGRAM) {printf ("MODE_SHOW_HISTOGRAM | ");}
		if (ctrl->mode & MODE_SHOW_VIDEO) {printf ("MODE_SHOW_VIDEO | ");}
		if (ctrl->mode & MODE_SAVE_IMAGE) {printf ("MODE_SAVE_IMAGE | ");}
		if (ctrl->mode & MODE_SAVE_IMAGE_ONCE) {printf ("MODE_SAVE_IMAGE_ONCE");}
		printf ("\n");
		ctrl->mode_old = ctrl->mode;
	}
	
	
	if (ctrl->mode & MODE_CTRL_EXPOSURE)
	{
		ctrl->exposure_step += arrow_x;
		if (arrow_y)
		{
			float exposure = ctrl->exposure;
			if (ctrl->exposure_step < 0)
			{
				exposure += arrow_y * pow (10, ctrl->exposure_step);
			}
			else if (ctrl->exposure_step > 0)
			{
				exposure += arrow_y * pow (10, ctrl->exposure_step - 1);
			}
			int r = cam_set_float (cam, "ExposureTime", exposure);
			if (r) 
			{
				printf ("ExposureTime au of range!\n");
			}
			else
			{
				ctrl->exposure = exposure;
			}
		}
		if (arrow_y || arrow_x)
		{
			printf ("%021.010f\n", ctrl->exposure);
			printf("%*s|%i\n", 10-ctrl->exposure_step, "", ctrl->exposure_step);
		}
	}
	
	
	if (ctrl->mode & MODE_CTRL_GAIN)
	{
		ctrl->gain_step += arrow_x;
		if (arrow_y)
		{
			float gain = ctrl->gain;
			if (ctrl->gain_step < 0)
			{
				gain += arrow_y * pow (10, ctrl->gain_step);
			}
			else if (ctrl->gain_step > 0)
			{
				gain += arrow_y * pow (10, ctrl->gain_step - 1);
			}
			int r = cam_set_float (cam, "Gain", gain);
			if (r)
			{
				printf ("Gain out of range!\n");
			}
			else
			{
				ctrl->gain = gain;
			}
		}
		if (arrow_y || arrow_x)
		{
			printf ("%021.010f\n", ctrl->gain);
			printf ("%*s|%i\n", 10-ctrl->gain_step, "", ctrl->gain_step);
		}
	}
}

