#pragma once
#include <SDL2/SDL.h>

struct Camera
{
	float mvp [4*4];
	float mt [4*4];
	float mr [4*4];
	float mp [4*4];
	float mrx [4*4];
	float mry [4*4];
	float ax;
	float ay;
	float az;
};


void sdl_update_projection (SDL_Window * window, float m [4*4])
{
	int w;
	int h;
	SDL_GetWindowSize (window, &w, &h);
	VN_CLR (4*4, m);
	m4f_perspective (m, 45.0f, (float)w/(float)h, 0.1f, 100.0f);
	glViewport (0, 0, w, h);
}


void camera_init (struct Camera * cam, SDL_Window * window)
{
	M4_IDENTITY (cam->mvp);
	M4_IDENTITY (cam->mrx);
	M4_IDENTITY (cam->mry);
	M4_IDENTITY (cam->mt);
	sdl_update_projection (window, cam->mp);
	cam->ax = 0.0f;
	cam->ay = 0.0f;
	cam->az = 0.0f;
}


void camera_update (struct Camera * cam, uint8_t const * keyboard)
{
	float t [4];
	t [0] = (keyboard [SDL_SCANCODE_A] - keyboard [SDL_SCANCODE_D]);
	t [1] = (keyboard [SDL_SCANCODE_LCTRL] - keyboard [SDL_SCANCODE_SPACE]);
	t [2] = (keyboard [SDL_SCANCODE_W] - keyboard [SDL_SCANCODE_S]);
	t [3] = 0;
	v4f_normalize (t, t);
	V4_MUL_SCALAR (t, t, 0.05f);
	cam->ax += (keyboard [SDL_SCANCODE_DOWN] - keyboard [SDL_SCANCODE_UP]) * 0.02f;
	cam->ay += (keyboard [SDL_SCANCODE_RIGHT] - keyboard [SDL_SCANCODE_LEFT]) * 0.02f;
	M4_ROTX (cam->mrx, cam->ax);
	M4_ROTY (cam->mry, cam->ay);
	M4_IDENTITY (cam->mr);
	M4_IDENTITY (cam->mvp);
	m4f_mul (cam->mr, cam->mry, cam->mr);
	m4f_mul (cam->mr, cam->mrx, cam->mr);
	M4_MAC_TRANSPOSE (cam->mt + M4_VT, cam->mr, t);
	m4f_mul (cam->mvp, cam->mt, cam->mvp);
	m4f_mul (cam->mvp, cam->mr, cam->mvp);
	m4f_mul (cam->mvp, cam->mp, cam->mvp);
}


