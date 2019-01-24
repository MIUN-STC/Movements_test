#include "SDLGL.h"
#include "camera.h"
#include "shader.h"
#include "mesharray.h"

#include "app.h"
#include "pp.h"




struct PP
{
	char const * filename;
	uint32_t n;
	struct pp_v4f32_txy1 * pos;
	struct pp_v4f32_txy1 * pos1;
	uint32_t * next;
	uint32_t * next2;
	float * dmin;
	
	uint32_t en;
	struct pp_v4f32_txy1 * edge;
};


void pp_load (struct PP * pp, uint32_t n)
{
	if (pp->edge != NULL) {free (pp->edge);}
	if (pp->pos != NULL) {free (pp->pos);}
	if (pp->pos1 != NULL) {free (pp->pos1);}
	if (pp->next != NULL) {free (pp->next);}
	if (pp->next2 != NULL) {free (pp->next2);}
	if (pp->dmin != NULL) {free (pp->dmin);}
	
	pp->n = n;
	pp->en = n * 10;
	pp->dmin = calloc (pp->n, sizeof (float));
	pp->pos = calloc (pp->n, sizeof (struct pp_v4f32_txy1));
	pp->pos1 = calloc (pp->n, sizeof (struct pp_v4f32_txy1));
	pp->edge = calloc (pp->en, sizeof (struct pp_v4f32_txy1));
	pp->next = calloc (pp->n, sizeof (uint32_t));
	pp->next2 = calloc (pp->n, sizeof (uint32_t));
	for (uint32_t i = 0; i < pp->n; ++ i) {pp->next [i] = UINT32_MAX;}
	for (uint32_t i = 0; i < pp->n; ++ i) {pp->next2 [i] = UINT32_MAX;}
	
	
	FILE * f = fopen (pp->filename, "r");
	ASSERT (f != NULL);
	uint32_t ni = 0;
	for (ni = ni; ni < pp->n; ++ ni)
	{
		struct pp_v4f32_txy1 * v = (pp->pos + ni);
		int r = fscanf (f, "%f%f%f", &v->t, &v->x, &v->y);
		if (r == EOF) {break;}
	}
	TRACE_F ("%i", ni);
	pp->n = ni;
	fclose (f);
	
	
	for (uint32_t i = 0; i < pp->n; ++ i)
	{
		pp->pos1 [i].t = pp->pos [i].t / 10.0f;
		pp->pos1 [i].x = pp->pos [i].x / 1000.0f;
		pp->pos1 [i].y = pp->pos [i].y / 1000.0f;
		pp->pos1 [i].w = 1.0f;
	}
	
	for (uint32_t i = 0; i < pp->n; ++ i)
	{
		pp->dmin [i] = FLT_MAX;
		if (pp->next [i] != UINT32_MAX) {continue;}
		pp_conesearch (pp->n, pp->pos1, pp->dmin, pp->next, i, 0.3f, 0.5f, 0.3f);
	}
	
	pp_deforkocf (pp->n, pp->next, pp->next2);
	
	generate_nextchain_edge 
	(
		&pp->en, 
		pp->edge, 
		pp->n, 
		pp->next2, 
		pp->pos1, 
		sizeof (struct pp_v4f32_txy1)
	);
	
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
	size_t vcount = pp->n;
	GLenum const primitive = GL_POINTS;
	GLenum const target = GL_ARRAY_BUFFER;
	GL_CHECK_ERROR;
	glBindBuffer (target, vbo);
	GLintptr const offset = (*vfirst) * sizeof (float) * dim;
	GLsizeiptr const size = vcount * sizeof (float) * dim;
	glBufferSubData (target, offset, size, pp->pos1);
	mesh_addv (m, vfirst, vcount, primitive);
}


void mesh_add_edges (struct Mesh * m, GLint * vfirst, GLuint vbo, struct PP * pp)
{
	size_t const dim = 4;
	size_t vcount = pp->en;
	GLenum const primitive = GL_LINES;
	GLenum const target = GL_ARRAY_BUFFER;
	GL_CHECK_ERROR;
	glBindBuffer (target, vbo);
	GLintptr const offset = (*vfirst) * sizeof (float) * dim;
	GLsizeiptr const size = vcount * sizeof (float) * dim;
	glBufferSubData (target, offset, size, pp->edge);
	mesh_addv (m, vfirst, vcount, primitive);
}




int main (int argc, char *argv[])
{
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
	
	program = gl_program_from_filename ("src/project_pp/shader.glvs;src/project_pp/shader.glfs");
	gl_program_debug (1, &program);
	glUseProgram (program);
	umvp = glGetUniformLocation (program, "mvp");
	cam_init (&cam, window);
	
	size_t vcount = 100000;
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
	
	
	struct PP pp = {0};
	//pp.filename = "data/c1_20180627_090007.txt";
	pp.filename = "data/c1_20180626_190007_b1.txt";
	//pp.filename = "data/blob1.txt";
	//pp.filename = "data/v1.txt";
	pp_load (&pp, 10000);
	SDL_Delay (2);
	mesh_add_triangle (mesh + 0, &vfirst, buf [0]);
	mesh_add_points   (mesh + 1, &vfirst, buf [0], &pp);
	mesh_add_edges    (mesh + 2, &vfirst, buf [0], &pp);
	
	
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










