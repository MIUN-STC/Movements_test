#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glcorearb.h>

#include <stdio.h>
#include <unistd.h>

#include "debug.h"
#include "debug_gl.h"
#include "mat.h"
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "global.h"
 
#define FMT_INT TCOL (TCOL_NORMAL, TCOL_YELLOW, TCOL_DEFAULT) "%02i " TCOL_RESET
#define FMT_FLT TCOL (TCOL_NORMAL, TCOL_CYAN, TCOL_DEFAULT) "%04.1f " TCOL_RESET
#define APP_WINDOW_WIDTH 1024
#define APP_WINDOW_HEIGHT 768
#define APP_TITLE "My OpenGL test window"



int load_plot (FILE * f, struct Vertex * v, size_t n)
{
	int i;
	int r;
	for (i = 0; i < n; i = i + 1)
	{
		if (n == 0) {return i;};
		char c;
		r = fscanf (f, "%f%f%f%c", v->pos + 0, v->pos + 2, v->pos + 1, &c);
		V4_SET (v->col, 1.0f, 0.4f, 0.4f, 1.0f);
		V4_MUL_SCALAR (v->pos, v->pos, 1.0f/100.0f);
		v->pos [3] = 1.0f;
		v->pos [1] = -v->pos [1];
		v->pos [0] = v->pos [0] * 10.0f;
		v->pos [1] += 25.0f;//z
		v->pos [0] -= 15.0f;//x
		v->pos [2] -= 15.0f;//y
		TRACE_F ("%f %f %f %f", v->pos [0], v->pos [1], v->pos [2], v->pos [3]);
		if (r == EOF) {return i;}
		v = v + 1;
	}
	return i;
}




int main (int argc, char *argv[])
{
	ASSERT_F (argc == 1, "No arg allowed");
	ASSERT_F (argv != NULL, "NULL error");
	
	{
		int r = SDL_Init (SDL_INIT_VIDEO);
		ASSERT_F (r == 0, "SDL_Init %i\n", SDL_GetError ());
	}

	SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 5);

	//App variables:
	SDL_Window * window;
	SDL_GLContext context;
	GLuint program;
	GLuint uniform_mvp;
	struct Camera cam;
	
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
	
	context = SDL_GL_CreateContext (window);
	ASSERT_F (context != NULL, "SDL_GL_CreateContext: %s", SDL_GetError());

	printf ("Using opengl version %s....\n", glGetString (GL_VERSION));
	printf ("Using glsl version %s.....\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

	//During init, enable debug output
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_DEBUG_OUTPUT);
    glEnable (GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //glEnable (GL_POINT_SMOOTH);
    GL_CHECK_ERROR;
    
	//glDebugMessageCallback (MessageCallback, 0);
	glDebugMessageCallback (glDebugOutput, 0);
	glDebugMessageControl (GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	
	struct Shader shaders [2] = 
	{
		{0, GL_VERTEX_SHADER, "shader.glvs"},
		{0, GL_FRAGMENT_SHADER, "shader.glfs"}
	};
	program = program_create (shaders, COUNTE (shaders));
	uniform_mvp = glGetUniformLocation (program, "mvp");
	ASSERT_F (uniform_mvp >= 0, "glGetUniformLocation no uniform found.");

	
	struct Vertex tri_data [3] = 
	{
		[0].pos = { -0.5f, -0.5f, 0.0f, 1.0f}, [0].col = { 1.0f, 0.0f, 0.0f, 1.0f },
		[1].pos = {  0.5f, -0.5f, 0.0f, 1.0f}, [1].col = { 0.0f, 1.0f, 0.0f, 1.0f },
		[2].pos = {  0.0f,  0.5f, 0.0f, 1.0f}, [2].col = { 0.0f, 0.0f, 1.0f, 1.0f }
	};

	struct Vertex sqr_data [6] = 
	{
		[0].pos = { -0.5f, -0.5f, -1.0f, 1.0f }, [0].col = { 1.0f, 0.0f, 0.0f, 1.0f },
		[1].pos = {  0.5f, -0.5f, -1.0f, 1.0f }, [1].col = { 0.0f, 1.0f, 0.0f, 1.0f },
		[2].pos = {  0.5f,  0.5f, -1.0f, 1.0f }, [2].col = { 0.0f, 0.0f, 1.0f, 1.0f },
		[3].pos = {  0.5f,  0.5f, -1.0f, 1.0f }, [3].col = { 0.0f, 0.0f, 1.0f, 1.0f },
		[4].pos = { -0.5f,  0.5f, -1.0f, 1.0f }, [4].col = { 0.0f, 1.0f, 0.0f, 1.0f },
		[5].pos = { -0.5f, -0.5f, -1.0f, 1.0f }, [5].col = { 1.0f, 0.0f, 0.0f, 1.0f }
	};
	
	#define GRID_COUNT 840
	struct Vertex grid_data [GRID_COUNT] = {0};
	gen_grid (grid_data, GRID_COUNT, 1.0f, -100.0f, 100.0f, -100.0f, 100.0f, 10.0f);
	

	struct Vertex plot_data [10000] = {0};
	FILE * plot_f = fopen ("poi11.txt", "r");
	int plot_n = load_plot (plot_f, plot_data, COUNTOF (plot_data));
	TRACE_F ("plot_n %i", plot_n);
	fclose (plot_f);
	
	struct Mesh triangle;
	struct Mesh square;
	struct Mesh grid;
	struct Mesh plot;
	
	triangle.mode = GL_TRIANGLES;
	triangle.vn = 3;
	triangle.vv = tri_data;
	triangle.vao = gpu_load_verts (program, triangle.vv, triangle.vn);
	
	square.mode = GL_TRIANGLES;
	square.vn = 6;
	square.vv = sqr_data;
	square.vao = gpu_load_verts (program, square.vv, square.vn);
	
	grid.mode = GL_LINES;
	grid.vn = GRID_COUNT;
	grid.vv = grid_data;
	grid.vao = gpu_load_verts (program, grid.vv, GRID_COUNT);
	
	plot.mode = GL_POINTS;
	plot.vn = plot_n;
	plot.vv = plot_data;
	plot.vao = gpu_load_verts (program, plot.vv, plot_n);	
	
	const Uint8 * keyboard = SDL_GetKeyboardState (NULL);



	camera_init (&cam, window);
	//M4_PRINT (cam.mp, FMT_FLT);

	//printf ("main loop:\n");
	
	glPointSize (2.0f);
	glClearColor (0.0, 0.0, 0.1, 0);
	while (1)
	{
		
		SDL_Event event;
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
				goto main_quit;
				
				case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
					printf ("SDL_WINDOWEVENT_CLOSE");
					break;
				
					case SDL_WINDOWEVENT_RESIZED:
					//printf ("Window %d resized to %dx%d\n",event.window.windowID, event.window.data1, event.window.data2);
					gl_update_projection (window, cam.mp);
					break;
				}
				break;
				
				case SDL_MOUSEMOTION:
				sdl_get_mouse (window);
				break;


				case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
					//Maybe SDL_PushEvent should be used for error handling?
					event.type = SDL_QUIT;
					SDL_PushEvent (&event);
					break;
					
					case SDLK_r:
					//CLR_V (4*4, mvp);
					case SDLK_a:
					case SDLK_d:
					case SDLK_w:
					case SDLK_s:

					case SDLK_SPACE:
					case SDLK_LCTRL:
					case SDLK_LEFT:
					case SDLK_RIGHT:
					case SDLK_UP:
					case SDLK_DOWN:
					M4_PRINT (cam.mvp, FMT_FLT);
					printf ("\n");
					break;
				}
				break;
			}
		}
		
		camera_update (&cam, keyboard);
		
		
		glUseProgram (program);
		glUniformMatrix4fv (uniform_mvp, 1, GL_FALSE, cam.mvp);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
		
		GL_CHECK_ERROR;
		
		
		mesh_draw (&triangle);
		mesh_draw (&square);
		mesh_draw (&grid);
		mesh_draw (&plot);
		
		//printf ("glGetError %i\n", glGetError ());
		fflush (stdout);
		SDL_Delay (1);
		SDL_GL_SwapWindow (window);
	}
	
main_quit:
	
	SDL_GL_DeleteContext (context);
	SDL_DestroyWindow (window);

	return 0;
}
