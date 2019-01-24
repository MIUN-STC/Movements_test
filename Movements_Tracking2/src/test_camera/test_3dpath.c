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
#include "poi.h"
#include "mat.h"
#include "map.h"
 
#define FMT_INT TCOL (TCOL_NORMAL, TCOL_YELLOW, TCOL_DEFAULT) "%02i " TCOL_RESET
#define FMT_FLT TCOL (TCOL_NORMAL, TCOL_CYAN, TCOL_DEFAULT) "%04.1f " TCOL_RESET
#define APP_WINDOW_WIDTH 1920
#define APP_WINDOW_HEIGHT 1200
#define APP_TITLE "Testing path finding"

#define APP_MESH_GRID 0
#define APP_MESH_VL 1
#define APP_MESH_VR 2
#define APP_MESH_EL 3
#define APP_MESH_ER 4


void convert_poi_to_vertex (struct POI * p, struct Vertex * v, size_t n, float r, float g, float b, float a)
{
	for (size_t i = 0; i < n; i = i + 1)
	{
		v [i].pos [0] = p [i].p [POI_T];
		v [i].pos [1] = p [i].p [POI_X];
		v [i].pos [2] = p [i].p [POI_Y];
		v [i].pos [3] = 1.0f;
		if (p [i].l < 4)
		{
			r = 0.0f;
			g = 0.0f;
			b = 1.0f;
		}
		V4_SET (v [i].col, r, g, b, a);
	}
}


int draw1_poi_next 
(
	struct POI_Context * ctx, 
	struct POI    * p0, size_t pn, 
	struct Vertex * v0, size_t vn
)
{
	size_t vi = 0;
	for (size_t pi = 0; pi < pn; pi ++)
	{
		//TRACE_F ("%i %i", pi, vi);
		
		struct POI * a = p0 + pi;
		struct POI * b = a->next;
		if (b == NULL) {continue;}
		if (a->g == 0) {continue;}
		if (a->flags & POI_REMOVED) {continue;}
		if (b->flags & POI_REMOVED) {continue;}
		
		srand (a->g);
		float c [4];
		c [0] = map_lin_float (rand (), 0, RAND_MAX, 0.4f, 1.0f);
		c [1] = map_lin_float (rand (), 0, RAND_MAX, 0.4f, 1.0f);
		c [2] = map_lin_float (rand (), 0, RAND_MAX, 0.4f, 1.0f);
		c [3] = 1.0f;
		//v3f_normalize (c, c);
		
		
		if (vi > vn) {break;}
		v0 [vi].pos [0] = a->p [POI_T];
		v0 [vi].pos [1] = a->p [POI_X];
		v0 [vi].pos [2] = a->p [POI_Y];
		v0 [vi].pos [3] = 1.0f;
		V4_SET (v0 [vi].col, c [0], c [1], c [2], c [3]);
		vi ++;
		
		if (vi > vn) {break;}
		v0 [vi].pos [0] = b->p [POI_T];
		v0 [vi].pos [1] = b->p [POI_X];
		v0 [vi].pos [2] = b->p [POI_Y];
		v0 [vi].pos [3] = 1.0f;
		V4_SET (v0 [vi].col, c [0], c [1], c [2], c [3]);
		vi ++;
	}
	//TRACE_F ("%i", );
	return vi;
}



#define TEMP_RELINK (1 << 0)
#define TEMP_FILTER (1 << 1)



void temp_update1 
(
	struct Mesh * m, 
	struct POI_Context * ctx, 
	struct POI * x0,
	size_t n,
	uint32_t flags
)
{
	ctx->g = 1;
	poi_reset (x0, n);
	//dev_search6_merge (ctx, x0, n);
	dev_search1_distance (ctx, x0, n);
	dev_search3_backtrack (ctx, x0, n);
	if (flags & TEMP_RELINK)
	{
		dev_search4_relink (ctx, x0, n);	
	}
	dev_search2_length (ctx, x0, n);
	if (flags & TEMP_FILTER)
	{
		dev_search8_filter (ctx, x0, n);
	}
	dev_search5_group (ctx, x0, n);
	m->count = draw1_poi_next (ctx, x0, n, m->data, m->count_max);
	m->flags |= MESH_UPDATE_ONCE;
	TRACE_F ("Edge count %i", m->count);
}


void temp_filter 
(
	struct Mesh * m, 
	struct POI_Context * ctx, 
	struct POI * x0,
	size_t n,
	uint32_t flags
)
{
	for (size_t i = 0; i < n; i = i + 1)
	{
		if (x0 [i].l < 6)
		{
			x0 [i].g = 0;
		}
	}
	m->count = draw1_poi_next (ctx, x0, n, m->data, m->count_max);
	mesh_update (m, 1);
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
		//SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
		SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN
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
	
	struct POI_Context ctx;
	ctx.g = 1;
	ctx.s [POI_T] = 1.0f;
	ctx.s [POI_X] = 1.0f;
	ctx.s [POI_Y] = 1.0f;
	struct POI p1v [10000];
	struct POI p1v_backup [10000];
	struct POI p2v [10000];
	struct POI p2v_backup [10000];
	FILE * p1f = fopen ("poi1.txt", "r");
	FILE * p2f = fopen ("poi2.txt", "r");
	int p1n = poi_load (p1f, p1v_backup, COUNTOF (p1v));
	int p2n = poi_load (p2f, p2v_backup, COUNTOF (p2v));
	fclose (p1f);
	fclose (p2f);
	
	poi_reset (p1v_backup, p1n);
	poi_reset (p2v_backup, p2n);
					
	memcpy (p1v, p1v_backup, sizeof (p1v));
	memcpy (p2v, p2v_backup, sizeof (p2v));
	
	
	
	
	

	struct Mesh m [6] = {0};
	
	m [APP_MESH_GRID].flags = MESH_MALLOC | MESH_DRAW | MESH_UPDATE_ONCE;
	m [APP_MESH_GRID].mode = GL_LINES;
	m [APP_MESH_GRID].data = NULL;
	m [APP_MESH_GRID].offset = 0;
	m [APP_MESH_GRID].count = 20 * 20 * 2;
	
	m [APP_MESH_VL].flags = MESH_MALLOC | MESH_DRAW | MESH_UPDATE_ONCE;
	m [APP_MESH_VL].mode = GL_POINTS;
	m [APP_MESH_VL].count = p1n;
	m [APP_MESH_VL].data = malloc (m [APP_MESH_VL].count * sizeof (struct Vertex));
	m [APP_MESH_VL].offset = 0;
	
	m [APP_MESH_VR].flags = MESH_MALLOC | MESH_DRAW | MESH_UPDATE_ONCE;
	m [APP_MESH_VR].mode = GL_POINTS;
	m [APP_MESH_VR].count = p2n;
	m [APP_MESH_VR].data = malloc (m [APP_MESH_VR].count * sizeof (struct Vertex));
	m [APP_MESH_VR].offset = 0;
	
	m [APP_MESH_EL].flags = MESH_MALLOC | MESH_DRAW;
	m [APP_MESH_EL].mode = GL_LINES;
	m [APP_MESH_EL].count = 1000 * 1000;
	m [APP_MESH_EL].data = NULL;
	m [APP_MESH_EL].offset = 0;
	
	m [APP_MESH_ER].flags = MESH_MALLOC | MESH_DRAW;
	m [APP_MESH_ER].mode = GL_LINES;
	m [APP_MESH_ER].count = 1000 * 1000;
	m [APP_MESH_ER].data = NULL;
	m [APP_MESH_ER].offset = 0;
	
	convert_poi_to_vertex (p1v, m [APP_MESH_VL].data, m [APP_MESH_VL].count, 0.7f, 0.1f, 0.1f, 1.0f);
	convert_poi_to_vertex (p2v, m [APP_MESH_VR].data, m [APP_MESH_VR].count, 0.1f, 0.7f, 0.1f, 1.0f);
	
	mesh_init_load (m, COUNTOF (m), program);
	
	gen_grid (m [APP_MESH_GRID].data, m [APP_MESH_GRID].count, -1.0f, -10.0f, 10.0f, -10.0f, 10.0f, 0.1f);
	
	mesh_update (m, COUNTOF (m));
	
	const Uint8 * keyboard = SDL_GetKeyboardState (NULL);
	camera_init (&cam, window);
	cam.move_speed = 0.01f;
	cam.angle_speed = 0.006f;
	glPointSize (3.0f);
	glClearColor (0.0, 0.0, 0.1, 0);
	
	float stretch = 300.0f;
	
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
				//sdl_get_mouse (window);
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
					memcpy (p1v, p1v_backup, sizeof (p1v));
					memcpy (p2v, p2v_backup, sizeof (p2v));
					convert_poi_to_vertex (p1v, m [APP_MESH_VL].data, m [APP_MESH_VL].count, 0.7f, 0.1f, 0.1f, 1.0f);
					convert_poi_to_vertex (p2v, m [APP_MESH_VR].data, m [APP_MESH_VR].count, 0.1f, 0.7f, 0.1f, 1.0f);
					m [APP_MESH_VL].flags |= MESH_UPDATE_ONCE;
					m [APP_MESH_VR].flags |= MESH_UPDATE_ONCE;
					break;
					
					case SDLK_h:
					dev_search6_merge (&ctx, p1v, p1n);
					dev_search6_merge (&ctx, p2v, p2n);
					convert_poi_to_vertex (p1v, m [APP_MESH_VL].data, m [APP_MESH_VL].count, 0.7f, 0.1f, 0.1f, 1.0f);
					convert_poi_to_vertex (p2v, m [APP_MESH_VR].data, m [APP_MESH_VR].count, 0.1f, 0.7f, 0.1f, 1.0f);
					m [APP_MESH_VL].flags |= MESH_UPDATE_ONCE;
					m [APP_MESH_VR].flags |= MESH_UPDATE_ONCE;
					break;
					
					
					
					case SDLK_KP_PLUS:
					ctx.s [POI_T] += 1;
					TRACE_F ("ctx.s [POI_T] %f", ctx.s [POI_T]);
					temp_update1 (m + APP_MESH_EL, &ctx, p1v, p1n, 0);
					temp_update1 (m + APP_MESH_ER, &ctx, p2v, p2n, 0);
					break;
					
					case SDLK_KP_MINUS:
					ctx.s [POI_T] -= 1;
					TRACE_F ("ctx.s [POI_T] %f", ctx.s [POI_T]);
					temp_update1 (m + APP_MESH_EL, &ctx, p1v, p1n, 0);
					temp_update1 (m + APP_MESH_ER, &ctx, p2v, p2n, 0);
					break;
					
					
					case SDLK_1:
					temp_update1 (m + APP_MESH_EL, &ctx, p1v, p1n, 0);
					temp_update1 (m + APP_MESH_ER, &ctx, p2v, p2n, 0);
					break;
					
					case SDLK_2:
					temp_update1 (m + APP_MESH_EL, &ctx, p1v, p1n, TEMP_RELINK);
					temp_update1 (m + APP_MESH_ER, &ctx, p2v, p2n, TEMP_RELINK);
					break;
					
					case SDLK_3:
					temp_update1 (m + APP_MESH_EL, &ctx, p1v, p1n, TEMP_RELINK | TEMP_FILTER);
					temp_update1 (m + APP_MESH_ER, &ctx, p2v, p2n, TEMP_RELINK | TEMP_FILTER);
					break;
					
					
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
					//M4_PRINT (cam.mvp, FMT_FLT);
					//printf ("\n");
					break;
				}
				break;
			}
		}
		
		
		mesh_update (m, COUNTOF (m));
		
		camera_update (&cam, keyboard);
		glUseProgram (program);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
		GL_CHECK_ERROR;
		
		//mesh_draw (m, COUNTOF (m));
		
		M4_IDENTITY (cam.mm);
		//M4_ROTX (cam.mm, 10.0f);
		cam.mm [M4_S0] = 1.0f;
		cam.mm [M4_S1] = 1.0f;
		cam.mm [M4_S2] = 1.0f;
		
		camera_mvp_update (&cam);
		glUniformMatrix4fv (uniform_mvp, 1, GL_FALSE, cam.mvp);
		mesh_draw (m + APP_MESH_GRID, 1);
		
		stretch += (keyboard [SDL_SCANCODE_P] - keyboard [SDL_SCANCODE_O]);
		
		M4_CLR (cam.mm);
		cam.mm [M4_V0 + 0] =  1.0f / stretch;
		cam.mm [M4_V1 + 2] =  1.0f / 3000.0f;
		cam.mm [M4_V2 + 1] = -1.0f / 3000.0f;
		cam.mm [15] = 1.0f;
		
		camera_mvp_update (&cam);
		glUniformMatrix4fv (uniform_mvp, 1, GL_FALSE, cam.mvp);
		mesh_draw (m + APP_MESH_VL, 1);
		mesh_draw (m + APP_MESH_VR, 1);
		mesh_draw (m + APP_MESH_EL, 1);
		mesh_draw (m + APP_MESH_ER, 1);
		
		//printf ("glGetError %i\n", glGetError ());
		//fflush (stdout);
		SDL_Delay (1);
		SDL_GL_SwapWindow (window);
	}
	
main_quit:
	
	SDL_GL_DeleteContext (context);
	SDL_DestroyWindow (window);

	return 0;
}
