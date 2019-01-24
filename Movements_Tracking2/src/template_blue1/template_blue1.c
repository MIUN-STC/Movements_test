#include "app.h"
#include "glwrap.h"



enum App_GLProg
{
	APP_GLPROG_S1,
	APP_GLPROG_UV,
	APP_GLPROG_MANDELBROT,
	APP_GLPROG_N
};


char const * APP_GLPROG_FILES [] =
{
	"src/template_blue1/shader.glvs;src/template_blue1/shader.glfs",
	"src/template_blue1/uv.glvs;src/template_blue1/uv.glfs",
	"src/template_blue1/mandelbrot.glvs;src/template_blue1/mandelbrot.glfs"
};

enum App_GLBuf
{
	APP_GLBUF_EXAMPLE1,
	APP_GLBUF_N
};


enum App_GLTex
{
	APP_GLTEX_EXAMPLE1,
	APP_GLTEX_N,
};

enum App_GLVAO
{
	APP_GLVAO_XYUV,
	APP_GLVAO_XYZW_UV,
	APP_GLVAO_N
};


struct GL_Mesh
{
	GLenum mode;     //GL_TRIANGLE etc...
	uint32_t vfirst; //vertex first
	uint32_t vcount; //vertex count
};


struct GL_Mesharray
{
	size_t cap;
	size_t n;
	struct GL_Mesh * meshes;
};


void gl_mesharray_init (struct GL_Mesharray * m, size_t cap)
{
	m->cap = cap;
	m->n = 0;
	m->meshes = calloc (cap + 1, sizeof (struct GL_Mesh));
}


void gl_mesharray_draw (struct GL_Mesharray * m)
{
	for (size_t i = 0; i < m->n; ++ i)
	{
		GLenum const mode = m->meshes [i].mode;
		GLint const vfirst = m->meshes [i].vfirst;
		GLsizei const vcount = m->meshes [i].vcount;
		glDrawArrays (mode, vfirst, vcount);
	}
}


void gl_mesharray_add (struct GL_Mesharray * m, size_t vcount, GLenum mode)
{
	ASSERT (m->n < m->cap);
	m->meshes [m->n].mode = mode;
	m->meshes [m->n].vcount = vcount;
	m->meshes [m->n + 1].vfirst = m->meshes [m->n].vfirst;
	m->n ++;
}






void update_square (struct GL_Mesh * m)
{
	float v [] =
	{
		 0.5f,  0.5f, 0.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f, 1.0f, // bottom right
		-0.5f, -0.5f, 0.0f, 1.0f, // bottom left
		
		 0.5f, -0.5f, 0.0f, 1.0f, // bottom right
		-0.5f, -0.5f, 0.0f, 1.0f, // bottom left
		-0.5f,  0.5f, 0.0f, 1.0f  // top left 
	};
	GLenum const target = GL_ARRAY_BUFFER;
	GLintptr const offset = 0;
	GLsizeiptr const length = sizeof (v);
	GLbitfield const access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
	void * buffer = glMapBufferRange (target, offset, length, access);
	memcpy (buffer, v, sizeof (v));
	glUnmapBuffer (target);
	
}






struct Container1
{
	GLuint prog [APP_GLPROG_N];
	GLint umvp [APP_GLPROG_N];
	GLuint buf [APP_GLBUF_N];
	GLuint tex [APP_GLTEX_N];
	GLuint vao [APP_GLVAO_N];
};


void container1_init (struct Container1 * c)
{
	GLuint * prog = c->prog;
	GLint * umvp = c->umvp;
	GLuint * buf = c->buf;
	GLuint * tex = c->tex;
	GLuint * vao = c->vao;
	
	gl_programs_from_filenames (APP_GLPROG_N, prog, APP_GLPROG_FILES);
	gl_program_debug           (APP_GLPROG_N, prog);
	glGenBuffers               (APP_GLBUF_N, buf);
	glGenTextures              (APP_GLTEX_N, tex);
	glGenVertexArrays          (APP_GLVAO_N, vao);
	
	umvp [APP_GLPROG_S1] = glGetUniformLocation (prog [APP_GLPROG_S1], "mvp");
	ASSERT (umvp [APP_GLPROG_S1] >= 0);
}






int main (int argc, char *argv[])
{
	struct Application app;
	app_init (&app, argc, argv);
	

	struct Container1 c1;
	container1_init (&c1);
	
	glBindVertexArray (c1.vao [APP_GLVAO_XYUV]);
	glBindBuffer (GL_ARRAY_BUFFER, c1.buf [APP_GLBUF_EXAMPLE1]);
	glVertexAttribPointer_byname (c1.prog [APP_GLPROG_S1], "pos", 3, GL_FLOAT, GL_FALSE, sizeof (float) * 4, 0);
	glBufferData (GL_ARRAY_BUFFER, 10000, 0, GL_DYNAMIC_DRAW);
	
	glEnable (GL_SCISSOR_TEST);
	
	struct GL_Mesharray ma;
	gl_mesharray_init (&ma, 1);
	gl_mesharray_add (&ma, 6,   GL_TRIANGLE_FAN);
	update_square (ma.meshes + 0);

	float xx = 0.0f;

	TRACE ("main loop");
	while (1)
	{
		app_frame_begin (&app);
		if (app.flags & APP_QUIT) {break;}
		if (app.flags & APP_OPENGL_ERROR) {break;}
		while (SDL_PollEvent (&app.event))
		{
			app_handle_input (&app);
			switch (app.event.type)
			{
				case SDL_KEYDOWN:
				switch (app.event.key.keysym.sym)
				{
					case SDLK_c:
					vf32_print (stdout, app.cam.p, 4, "%f ");
					vf32_print (stdout, app.cam.q, 4, "%f ");
					break;
					
					case SDLK_x:
					xx += 0.1f;
					break;
				}
				break;
			}
		}
		app_update_camera (&app);
		
		glUseProgram (c1.prog [APP_GLPROG_S1]);
		glUniformMatrix4fv (c1.umvp [APP_GLPROG_S1], 1, GL_FALSE, app.cam.PVM);
		
		glViewport (0, 0, app.w, app.h);
		
		//glViewport (0, 0, 0.5f * app.w, 0.5f * app.h);
		glScissor (0, 0, 0.5f * app.w, 0.5f * app.h);
		glClearColor (0.0, 0.0, 1.0, 1.0);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gl_mesharray_draw (&ma);
		
		//glViewport (0.5f * app.w, 0, 0.5f * app.w, 0.5f * app.h);
		glScissor (0.5f * app.w, 0, 0.5f * app.w, 0.5f * app.h);
		glClearColor (0.0, 1.0, 0.0, 1.0);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gl_mesharray_draw (&ma);
		
		glViewport (0.5f * app.w, 0.5f * app.h, 0.5f * app.w, 0.5f * app.h);
		glScissor (0.5f * app.w, 0.5f * app.h, 0.5f * app.w, 0.5f * app.h);
		glClearColor (0.0, 1.0, 1.0, 1.0);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gl_mesharray_draw (&ma);
		
		glViewport (0.0, (0.5f-xx) * app.h, 0.5f * app.w, 0.5f * app.h);
		glScissor (0.0, (0.5f-xx) * app.h, 0.5f * app.w, 0.5f * app.h);
		glClearColor (1.0, 0.0, 0.0, 1.0);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gl_mesharray_draw (&ma);
		
		app_frame_end (&app);
	}
	app_destroy (&app);
	return 0;
}










