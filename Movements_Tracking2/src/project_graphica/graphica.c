#include <stdio.h>
#include <unistd.h>

#include "SDLGL.h"
#include "debug.h"
#include "misc.h"
#include "debug_gl.h"
#include "graphics.h"

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

	struct GLH glh;
	BUFV_CALLOC (GLuint, glh.program, 1);
	BUFV_CALLOC (char const *, glh.program_name, 1);
	BUFV_CALLOC (GLuint, glh.vao, 1);
	BUFV_CALLOC (GLuint, glh.vbo, 1);
	
	glh.program_name.p [0] = 
	"src/project_graphica/shader.glvs;"
	"src/project_graphica/shader.glfs";
	
	glh_init (&glh);

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
					{
						struct GLC com [1];
						com [0].command = GLC_UPDATE_PROGRAM;
						com [0].program_name.iprogram = 0;
						com [0].program_name.iname = 0;
						glc_exec (&glh, 1, com);
					}
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
