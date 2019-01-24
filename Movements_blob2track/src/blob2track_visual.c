#include "SDLGL.h"
#include "camera.h"
#include "shader.h"
#include "mesharray.h"

#include "app.h"
#include "pp.h"
#include "option.h"




struct PP
{
	uint32_t nb;
	struct pp_v4f32_txy1 * pos0;
	struct pp_v4f32_txy1 * pos1;
	uint32_t * next;
	uint32_t * next2;
	float * dmin;
	uint32_t * nbf;
	
	uint32_t ne;
	struct pp_v4f32_txy1 * edge;

	//Blob diameter
	float * D;
};


void pp_load (struct PP * pp, struct Option * opt, FILE * f)
{
	pp->nb = opt->nb;
	pp->dmin = calloc (pp->nb, sizeof (float));
	pp->D = calloc (pp->nb, sizeof (float));
	pp->pos0 = calloc (pp->nb, sizeof (struct pp_v4f32_txy1));
	pp->pos1 = calloc (pp->nb, sizeof (struct pp_v4f32_txy1));
	pp->next = calloc (pp->nb, sizeof (uint32_t));
	pp->next2 = calloc (pp->nb, sizeof (uint32_t));
	pp->nbf = calloc (pp->nb, sizeof (uint32_t));
	vu32_set1 (pp->nb, pp->next, UINT32_MAX);
	
	pp->ne = opt->nb * 100;
	pp->edge = calloc (pp->nb, sizeof (struct pp_v4f32_txy1));
	
	for (uint32_t i = 0; i < pp->nb; ++ i) {pp->next [i] = UINT32_MAX;}
	for (uint32_t i = 0; i < pp->nb; ++ i) {pp->next2 [i] = UINT32_MAX;}
	
	pp_fread_all (f, &pp->nb, pp->pos0, pp->D, pp->nbf);
	TRACE_F ("nb: %i", pp->nb);
	
	
	for (uint32_t i = 0; i < pp->nb; ++ i)
	{
		pp->pos1 [i].t = pp->pos0 [i].t / 10.0f;
		pp->pos1 [i].x = pp->pos0 [i].x / 1000.0f;
		pp->pos1 [i].y = pp->pos0 [i].y / 1000.0f;
		pp->pos1 [i].w = 1.0f;
	}
	
	for (uint32_t i = 0; i < pp->nb; ++ i)
	{
		pp->dmin [i] = FLT_MAX;
		if (pp->next [i] != UINT32_MAX) {continue;}
		pp_conesearch 
		(
			pp->nb, 
			pp->pos0, 
			pp->D, 
			pp->dmin, 
			pp->next, 
			i, 
			opt->tplus,
			opt->r0, 
			opt->rk,
			opt->sx,
			opt->sy,
			opt->st,
			opt->sD
		);
	}
	
	pp_deforkocf (pp->nb, pp->next, pp->next2);
	
	generate_nextchain_edge 
	(
		&pp->ne, 
		pp->edge, 
		pp->nb, 
		pp->next2, 
		pp->pos1, 
		sizeof (struct pp_v4f32_txy1)
	);
	
	TRACE_F ("ne: %i", pp->ne);
	
}












void mesh_add_triangle (struct Mesh * m, GLint * vfirst, GLuint vbo)
{
	float v [] =
	{
		-1.0f,  1.0f, 0.0f, 1.0f, // top left
		 1.0f,  1.0f, 0.0f, 1.0f, // top right
		 1.0f, -1.0f, 0.0f, 1.0f, // bottom right 
		 1.0f, -1.0f, 0.0f, 1.0f, // bottom right 
		-1.0f, -1.0f, 0.0f, 1.0f, // bottom left
		-1.0f,  1.0f, 0.0f, 1.0f, // top left
	};
	size_t const dim = 4;
	size_t const vcount = 6;
	GLenum const primitive = GL_LINES;
	GLenum const target = GL_ARRAY_BUFFER;
	glBindBuffer (target, vbo);
	GLintptr const offset = (*vfirst) * sizeof (float) * dim;
	GLsizeiptr const size = vcount * sizeof (float) * dim;
	glBufferSubData (target, offset, size, v);
	mesh_addv (m, vfirst, vcount, primitive);
}


void mesh_add_points (struct Mesh * m, GLint * vfirst, GLuint vbo, struct PP * pp)
{
	size_t const dim = 4;
	size_t vcount = pp->nb;
	GLenum const primitive = GL_POINTS;
	GLenum const target = GL_ARRAY_BUFFER;
	GL_CHECK_ERROR;
	glBindBuffer (target, vbo);
	GLintptr const offset = (*vfirst) * sizeof (float) * dim;
	GLsizeiptr const size = vcount * sizeof (float) * dim;
	glBufferSubData (target, offset, size, pp->pos1);
	mesh_addv (m, vfirst, vcount, primitive);
}


void mesh_add_edges (struct Mesh * m, GLint * vfirst, GLuint vbo [], struct PP * pp)
{
	size_t const dim = 4;
	size_t vcount = pp->ne;
	GLenum const primitive = GL_LINES;
	GLenum const target = GL_ARRAY_BUFFER;
	GL_CHECK_ERROR;
	GLintptr const offset = (*vfirst) * sizeof (float) * dim;
	GLsizeiptr const size = vcount * sizeof (float) * dim;
	glBindBuffer (target, vbo [0]);
	glBufferSubData (target, offset, size, pp->edge);
	mesh_addv (m, vfirst, vcount, primitive);
	glBindBuffer (target, vbo [1]);
	float * v = glMapBufferRange (target, offset, size, GL_MAP_WRITE_BIT);
	for (uint32_t i = 0; i < vcount; ++ i)
	{
		/*
		v [0] = (float)rand () / (float)RAND_MAX;
		v [1] = (float)rand () / (float)RAND_MAX;
		v [2] = (float)rand () / (float)RAND_MAX;
		v [3] = (float)rand () / (float)RAND_MAX;
		*/
		v [0] = 1.0f;
		v [1] = 1.0f;
		v [2] = 1.0f;
		v [3] = 1.0f;
		v += 4;
	};
	glUnmapBuffer (target);
}




int main (int argc, char *argv[])
{
	struct Option opt;
	opt.r0 = 100.0f;
	opt.rk = 100.0f;
	opt.tplus = 4.0f;
	opt.sx = 0.1f;
	opt.sy = 0.1f;
	opt.st = 1.0f;
	opt.sD = 1.0f;
	opt.nb = 100000;
	//opt.np = 10000;
	opt.nbf_max = 100;
	opt.qmin = 2;
	opt.Dpass = 20;
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	if (opt.mode & OPT_HELP) {opt_help (&opt);return 0;}
	
	uint32_t flags = 0;
	SDL_Window * window;
	SDL_GLContext context;
	SDL_Event event;
	struct Cam cam;
	GLint umvp;
	GLuint program;
	struct Mesh mesh [3] = {0};
	
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
	
	program = gl_program_from_filename ("src/shader.glvs;src/shader.glfs");
	gl_program_debug (1, &program);
	glUseProgram (program);
	umvp = glGetUniformLocation (program, "mvp");
	cam_init (&cam, window);
	
	size_t vcount = 1000000;
	GLint vfirst = 0;
	GLuint vao [1];
	GLuint buf [3];
	glGenVertexArrays (1, vao);
	glGenBuffers (3, buf);
	glBindVertexArray (vao [0]);
	glBindBuffer (GL_ARRAY_BUFFER, buf [0]);
	glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, buf [1]);
	glVertexAttribPointer (1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray (1);
	//load_file ("data/c2_20180627_180006.txt", &vcount, vao [0]);
	
	glBindBuffer (GL_ARRAY_BUFFER, buf [0]);
	glBufferData (GL_ARRAY_BUFFER, vcount * sizeof (struct v4f32_XYZW), NULL, GL_STATIC_DRAW);
	glBindBuffer (GL_ARRAY_BUFFER, buf [1]);
	glBufferData (GL_ARRAY_BUFFER, vcount * sizeof (struct v4f32_XYZW), NULL, GL_STATIC_DRAW);
	
	FILE * f = fopen (opt.src, "r");
	ASSERT (f != NULL);
	
	struct PP pp = {0};
	pp_load (&pp, &opt, f);
	SDL_Delay (2);
	mesh_add_triangle (mesh + 0, &vfirst, buf [0]);
	mesh_add_points   (mesh + 1, &vfirst, buf [0], &pp);
	mesh_add_edges    (mesh + 2, &vfirst, buf, &pp);
	
	vf32_setl (cam.p, 4, -0.175754, -0.523679, -0.222534, 0.959602);
	vf32_setl (cam.q, 4, 0.548396, 0.070182, -0.035894, 0.832495);

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
					gl_update_projection (window, cam.P);
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
					vf32_print (stdout, cam.p, 4, "%f, ");
					vf32_print (stdout, cam.q, 4, "%f, ");
					break;
				}
				break;
			}
		}
		
		app_update_camera (&cam, keyboard);
		glUniformMatrix4fv (umvp, 1, GL_FALSE, cam.PVM);
		
		/*
		vf32_print2 (stdout, cam.PVM, 4, 4, "%6.2f ");
		printf ("\033[A");
		printf ("\033[A");
		printf ("\033[A");
		printf ("\033[A");
		fflush (stdout);
		*/
		
		
		glClear (GL_COLOR_BUFFER_BIT);
		//glDrawElements (GL_LINES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays (GL_POINTS, 0, vcount);
		//glDrawArrays (GL_TRIANGLES, 0, vcount);
		mesh_draw (3, mesh);
		
		SDL_GL_SwapWindow (window);
	}
	
//quit:
	
	SDL_GL_DeleteContext (context);
	SDL_DestroyWindow (window);

	return 0;
}










