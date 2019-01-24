#include <stdio.h>
#include <unistd.h>

#include "SDLGL.h"

#include "debug.h"
#include "debug_gl.h"

#include "misc.h"
#include "shader.h"

#define APP_ERROR 1 << 0
#define APP_QUIT 1 << 1

struct App
{
	uint32_t flags;
};


int main (int argc, char* argv[])
{
	struct App app;
	app.flags = 0;
	SDL_Window * window;
	SDL_GLContext context;
	SDL_Event event;
	SDL_Init (SDL_INIT_VIDEO);
	window = SDL_CreateWindow 
	(
		"An SDL2 window",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		SDL_WINDOW_OPENGL
	);
	ASSERT_F (window != NULL, "SDL_CreateWindow: %s", SDL_GetError());
	context = SDL_GL_CreateContext_CC (window);

	
	#define PROGRAM_COUNT 10
	struct
	{
		GLuint       handles [PROGRAM_COUNT];
		char const * filenames [PROGRAM_COUNT];
	} program = {0};
	
	program.filenames [0] =
	"src/project_graphica/shader.glvs;"
	"src/project_graphica/shader.glfs";
	program.handles [0] = gl_program_from_filename (program.filenames [0]);
	program.handles [1] = gl_program_from_filename (program.filenames [0]);
	gl_program_debug (PROGRAM_COUNT, program.handles);
	gl_program_free_shaders (program.handles [0]);
	gl_program_free_shaders (program.handles [1]);

	while (1)
	{
		if (app.flags & APP_ERROR) {break;}
		if (app.flags & APP_QUIT) {break;}
		
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
				app.flags |= APP_QUIT;
				break;
				
				case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
					event.type = SDL_QUIT;
					SDL_PushEvent (&event);
					break;
					
					case SDLK_z:
					SDL_ShowSimpleMessageBox (SDL_MESSAGEBOX_ERROR, "Missing file", "Hello!", NULL);
					break;
					
					case SDLK_k:
					break;
				}
				break;
			}
		}
		
		
	}

	SDL_GL_DeleteContext (context);
	SDL_DestroyWindow (window);
	SDL_Quit();
	return 0;
}
