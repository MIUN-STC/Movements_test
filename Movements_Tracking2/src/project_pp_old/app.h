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


enum app_glprogram_name
{
	APP_GLPROGRAM1,
	APP_GLPROGRAM2,
	APP_GLPROGRAM3,
	APP_GLPROGRAM_COUNT
};

static const char * app_glprogram_src [] =
{
	"src/project_pp/shader.glvs;src/project_pp/shader.glfs",
	NULL,
	NULL
};

enum app_glbuf_name
{
	APP_GLBUF_V1,
	APP_GLBUF_C1,
	APP_GLBUF_E1,
	APP_GLBUF_COUNT
};


enum app_glvao_name
{
	APP_GLVAO1,
	APP_GLVAO_COUNT
};


enum app_mesh_name
{
	APP_MESH1,
	APP_MESH2,
	APP_MESH_COUNT
};


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







struct Asset
{
	struct BUFV (GLuint, buf);
	struct BUFV (GLuint, vao);
	struct BUFV (GLuint, program);
	struct BUFV (GLint, uform);
	struct BUFV (char const *, program_src);
	struct BUFV (struct Mesh, mesh);
	struct BUFV (struct Cam, cam);
};


void as_init (struct Asset * a, SDL_Window * window)
{
	BUFV_CALLOC (GLuint, a->vao, 10);
	BUFV_CALLOC (GLuint, a->buf, 10);
	BUFV_CALLOC (GLuint, a->program, 10);
	BUFV_CALLOC (GLint, a->uform, 10);
	BUFV_CALLOC (char const *, a->program_src, 10);
	BUFV_CALLOC (struct Mesh, a->mesh, 10);
	BUFV_CALLOC (struct Cam, a->cam, 1);
	
	
	a->vao.n = a->vao.c;
	glGenVertexArrays (a->vao.n, a->vao.p);
	
	
	a->buf.n = a->buf.c;
	glGenBuffers (a->buf.n, a->buf.p);
	
	
	char const ** name;
	GLuint * program;
	GLint * uform;
	BUFV_PUSH (a->program, 1, program);
	BUFV_PUSH (a->program_src, 1, name);
	BUFV_PUSH (a->uform, 1, uform);
	name [0] = "src/project_pp/shader.glvs;src/project_pp/shader.glfs";
	program [0] = gl_program_from_filename (name [0]);
	uform [0] = glGetUniformLocation (program [0], "mvp");
	gl_program_debug (a->program.n, a->program.p);
	glUseProgram (a->program.p [0]);
	
	
	struct Cam * cam;
	BUFV_PUSH (a->cam, 1, cam);
	cam_init (cam, window);
}





