#pragma once

#include <SDL2/SDL.h>
#include "debug.h"
#include "debug_gl.h"
#include "ui.h"


//Soft application exit
#define APP_QUIT 1 << 0

//When assertion is disabled for OpenGL error checking
//this should be used for soft application exit.
#define APP_OPENGL_ERROR 1 << 1

//Remove mouse cursor.
//Use mouse move speed instead of screen coordinates.
#define APP_RELATIVE_MOUSE_MODE 1 << 2



#define FMT_INT TCOL (TCOL_NORMAL, TCOL_YELLOW, TCOL_DEFAULT) "%02i " TCOL_RESET
#define FMT_FLT TCOL (TCOL_NORMAL, TCOL_CYAN, TCOL_DEFAULT) "%04.1f " TCOL_RESET

#define APP_WINDOW_WIDTH 1024
#define APP_WINDOW_HEIGHT 768
#define APP_TITLE "My OpenGL test window"

/*
struct Window
{
	uint32_t flags;
	char const * name;
	uint32_t pos [2];
	uint32_t size [2];
};
*/








struct Application
{
	//SDL_Window * window;
	
	//Can be used for checking if app should exit, etc.
	uint32_t flags;
	
	struct Cam cam;
	
	//How much camera should move.
	float cam_p1 [4];
	
	//How much camera should rotate as axis angle.
	//Should be used for smoothing the camera.
	float cam_a1 [4];
	
	//How much camera should rotate as quaternion.
	float cam_q1 [4];
	
	//Frame stopwatch for calculating (dt).
	float t;
	
	//How many seconds one frame took.
	float dt;
	
	//User input.
	const Uint8 * keyboard;
	SDL_Event event;
	
	SDL_Window * window;
	SDL_GLContext context;
	GLuint program;
	GLuint uniform_mvp;
};


void smooth_mouse (float dt_sec, float const z [2], float x [2])
{
	float springiness = 10.0f; // tweak to taste.
	float dt = 1.0f-exp(log(0.5f)*springiness*dt_sec);
	//TRACE_F ("%f", dt);
	float dx [4];
	//dx := (z - x)
	//dx := dx * dt
	//x := x + dx
	v4f32_sub (z, x, dx);
	v4f32_mul_scalar (dx, dt, dx);
	v4f32_add (dx, x, x);
}










void app_init (struct Application * app, int argc, char * argv [])
{
	ASSERT_F (argc == 1, "No arg allowed");
	ASSERT_F (argv != NULL, "NULL error");
	app->flags = 0;
	{
		int r = SDL_Init (SDL_INIT_VIDEO);
		ASSERT_F (r == 0, "SDL_Init %i\n", SDL_GetError ());
	}
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 5);
	//During init, enable debug output
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_DEBUG_OUTPUT);
	glEnable (GL_DEBUG_OUTPUT_SYNCHRONOUS);
	//glDebugMessageCallback (MessageCallback, 0);
	glDebugMessageCallback (glDebugOutput, 0);
	glDebugMessageControl (GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	app->keyboard = SDL_GetKeyboardState (NULL);
	
	qf32_unit (app->cam_q1);
	v4f32_set (0, app->cam_a1);
	
	app->window = SDL_CreateWindow 
	(
		APP_TITLE, 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		APP_WINDOW_WIDTH, 
		APP_WINDOW_WIDTH, 
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);
	ASSERT_F (app->window != NULL, "SDL_CreateWindow: %s", SDL_GetError());
	
	app->context = SDL_GL_CreateContext (app->window);
	ASSERT_F (app->context != NULL, "SDL_GL_CreateContext: %s", SDL_GetError());

	printf ("Using opengl version %s....\n", glGetString (GL_VERSION));
	printf ("Using glsl version %s.....\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
	
	
	struct Shader shaders [2] = 
	{
		{0, GL_VERTEX_SHADER, "shader.glvs"},
		{0, GL_FRAGMENT_SHADER, "shader.glfs"}
	};
	app->program = program_create ("SimpleShaderProgram1", shaders, COUNTOF (shaders));
	glUseProgram (app->program);
	app->uniform_mvp = glGetUniformLocation (app->program, "mvp");
	ASSERT_F (app->uniform_mvp >= 0, "glGetUniformLocation no uniform found.");
	
	cam_init (&app->cam, app->window);
	
	glClearColor (0.0f, 0.0f, 0.1f, 0.0f);
	glPointSize (4.0f);
}





void app_frame_begin (struct Application * app)
{
	float const ms = 1.0f/1000.0f;
	app->t = (float)SDL_GetTicks () * ms;
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void app_frame_end (struct Application * app)
{
	sdl_get_rotation_vector (app->cam_q1, app->keyboard);
	sdl_get_translation_vector (app->cam_p1, app->keyboard);
	cam_update (&app->cam, app->cam_p1, app->cam_q1);
	qf32_unit (app->cam_q1);
		
	SDL_GL_SwapWindow (app->window);	
	SDL_Delay (1);
	
	if (glGetGraphicsResetStatus() != GL_NO_ERROR)
	{
		app->flags |= APP_OPENGL_ERROR;
	}
	
	float const ms = 1.0f/1000.0f;
	app->dt = ((float)SDL_GetTicks () * ms) - app->t;
}


void app_handle_keydown (struct Application * app)
{
	SDL_Event e = app->event;
	ASSERT (e.type == SDL_KEYDOWN);
	switch (e.key.keysym.sym)
	{
		case SDLK_ESCAPE:
		//Maybe SDL_PushEvent should be used for error handling?
		e.type = SDL_QUIT;
		SDL_PushEvent (&e);
		break;
		
		case SDLK_r:
		app->flags ^= APP_RELATIVE_MOUSE_MODE;
		SDL_SetRelativeMouseMode (app->flags & APP_RELATIVE_MOUSE_MODE);
		break;
	}
}


void app_handle_input (struct Application * app)
{
	SDL_Event e = app->event;
	switch (e.type)
	{
		case SDL_KEYDOWN:
		app_handle_keydown (app);
		break;
		
		case SDL_QUIT:
		app->flags |= APP_QUIT;
		break;

		case SDL_MOUSEMOTION:
		if (app->flags & APP_RELATIVE_MOUSE_MODE)
		{
			ui_mouse_quaternion (&e.motion, app->cam_q1);
		}
		break;
		
		case SDL_WINDOWEVENT:
		switch (app->event.window.event)
		{
			case SDL_WINDOWEVENT_CLOSE:
			printf ("SDL_WINDOWEVENT_CLOSE");
			break;
		
			case SDL_WINDOWEVENT_RESIZED:
			//printf ("Window %d resized to %dx%d\n",event.window.windowID, event.window.data1, event.window.data2);
			gl_update_projection (app->window, app->cam.P);
			break;
		}
		break;
		
		case SDL_MOUSEBUTTONDOWN:
		printf ("Mouse button down\n");
		break;
	}
}


void app_destroy (struct Application * app)
{
	SDL_GL_DeleteContext (app->context);
	SDL_DestroyWindow (app->window);
}


