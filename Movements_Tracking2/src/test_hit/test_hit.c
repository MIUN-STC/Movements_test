#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glcorearb.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h>

#include "global.h"
#include "debug.h"
#include "debug_gl.h"
#include "misc.h"
#include "camera.h"
#include "gmesh.h"
#include "gbuf.h"
#include "gen.h"
#include "shader.h"
#include "vertex.h"
#include "app.h"
#include "q.h"
#include "poi.h"
#include "map.h"
 





int main (int argc, char *argv[])
{
	struct Application app;
	app_init (&app, argc, argv);
	
	while (1)
	{
		if (app.flags & APP_QUIT) {break;}
		if (app.flags & APP_OPENGL_ERROR) {break;}
		
		app_frame_begin (&app);
		
		while (SDL_PollEvent (&app.event))
		{
			app_handle_input (&app);
		}

		app_frame_end (&app);
	}

	app_destroy (&app);

	return 0;
}










