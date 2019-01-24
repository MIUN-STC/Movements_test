#include "SDLGL.h"
#include "camera.h"
#include "shader.h"
#include "mesharray.h"
#include "app.h"




void app_setup_vattribute (GLuint buf [])
{
	//glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, buf [APP_GLBUF_ELE1]);
	glBindBuffer (GL_ARRAY_BUFFER, buf [APP_GLBUF_V1]);
	glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, buf [APP_GLBUF_C1]);
	glVertexAttribPointer (1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray (1);
}
















int main (int argc, char *argv[])
{
	uint32_t flags = 0;
	SDL_Window * window;
	SDL_GLContext context;
	SDL_Event event;
	struct Asset as;

	
	app_init (&flags);
	
	window = SDL_CreateWindow 
	(
		APP_TITLE, 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		APP_WINDOW_WIDTH, 
		APP_WINDOW_WIDTH, 
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);
	ASSERT_F (window != NULL, "SDL_CreateWindow: %s", SDL_GetError());
	uint8_t const * keyboard = SDL_GetKeyboardState (NULL);
	context = SDL_GL_CreateContext_CC (window);
	
	
	//OpenGL settings
	glDebugMessageCallback (glDebugOutput, 0);
	glDebugMessageControl (GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	glEnable (GL_DEBUG_OUTPUT);
	glEnable (GL_DEBUG_OUTPUT_SYNCHRONOUS);
	//glEnable (GL_DEPTH_TEST);
	glClearColor (0.1f, 0.1f, 0.2f, 0.0f);
	glPointSize (4.0f);
	
	
	as_init (&as, window);
	
	


	
	
	while (1)
	{
		if (flags & APP_QUIT) {break;}
		if (flags & APP_ERROR) {break;}
		
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
				flags |= APP_QUIT;
				break;
				
				case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
					printf ("SDL_WINDOWEVENT_CLOSE");
					break;
				
					case SDL_WINDOWEVENT_RESIZED:
					printf ("Window %d resized to %dx%d\n", event.window.windowID, event.window.data1, event.window.data2);
					gl_update_projection (window, as.cam.p [0].P);
					glViewport (0, 0, event.window.data1, event.window.data2);
					glScissor (0, 0, event.window.data1, event.window.data2);
					break;
				}
				break;
				
				case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
					event.type = SDL_QUIT;
					SDL_PushEvent (&event);
					break;
					
					case SDLK_n:
					break;
				}
				break;
			}
		}
		
		app_update_camera (as.cam.p + 0, keyboard);
		glUniformMatrix4fv (as.uform.p [0], 1, GL_FALSE, as.cam.p [0].PVM);
		
		vf32_print2 (stdout, as.cam.p [0].PVM, 4, 4, "%6.2f ");
		printf ("\033[A");
		printf ("\033[A");
		printf ("\033[A");
		printf ("\033[A");
		fflush (stdout);
		
		
		
		glClear (GL_COLOR_BUFFER_BIT);
		//glDrawElements (GL_LINES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays (GL_POINTS, 0, 4);
		mesh_draw (as.mesh.n, as.mesh.p);
		
		SDL_GL_SwapWindow (window);
	}
	
//quit:
	
	SDL_GL_DeleteContext (context);
	SDL_DestroyWindow (window);

	return 0;
}










