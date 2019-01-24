#include "app.h"
#include "grid.h"


struct Buffer
{
	void * base;
	void * base_guard;
	size_t base_size8;
	
	void * last;
	size_t last_size8;
};


void buffer_calloc_vertices (struct Buffer * b, size_t dim, size_t n)
{
	b->base_size8 = sizeof (float) * dim * n;
	//TRACE_F ("base_size8 %i", (int) b->base_size8);
	b->base = calloc (b->base_size8, sizeof (uint8_t));
	b->base_guard = (uint8_t *)b->base + b->base_size8;
	b->last = b->base;
	b->last_size8 = 0;
}


void buffer_add_bytes (struct Buffer * b, size_t n)
{
	size_t size8 = b->last_size8 + n;
	//TRACE_F ("%i %i", (int) size8, (int) b->base_size8);
	ASSERT (size8 <= b->base_size8);
	b->last_size8 = size8;
	b->last = (uint8_t *) b->last + n;
}


void buffer_add_vertices (struct Buffer * b, size_t dim, size_t n)
{
	buffer_add_bytes (b, sizeof (float) * dim * n);
	//TRACE_F ("buffer %i", (int) b->last_size8 / (sizeof (float)));	
	TRACE_F ("Adding %i pcs of %i dimensional vertices to buffer", (int) n, (int) dim);
}





struct Drawer
{
	uint32_t cap;
	uint32_t n;
	GLenum * mode;
	uint32_t * count;
	uint32_t * first;
};


void drawer_calloc (struct Drawer * d, uint32_t cap)
{
	d->cap = cap;
	d->n = 0;
	d->mode = calloc (cap, sizeof (GLenum));
	d->count = calloc (cap, sizeof (uint32_t));
	d->first = calloc (cap, sizeof (uint32_t));
}


void drawer_add_vertices (struct Drawer * d, GLenum mode, uint32_t count)
{
	ASSERT (d->cap > 0);
	ASSERT (d->cap > d->n);
	d->mode [d->n] = mode;
	d->count [d->n] = count;
	d->first [d->n + 1] = d->first [d->n] + count;
	d->n ++;
}


void drawer_draw (struct Drawer * drawer)
{
	for (size_t i = 0; i < drawer->n; ++ i)
	{
		GLenum const mode = drawer->mode [i];
		GLint const first = drawer->first [i];
		GLsizei const count = drawer->count [i];
		glDrawArrays (mode, first, count);
	}	
}







/*
GLuint load (const char * imagepath)
{
	GLuint tex;
	glGenTextures (1, &tex);

	//Set OpenGL global state GL_TEXTURE_2D to (tex).
	//All texture functions will modify the current bound texture.
	glBindTexture (GL_TEXTURE_2D, tex);

	// Read the file, call glTexImage2D with the right parameters
	glfwLoadTexture2D (imagepath, 0);
	
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap (GL_TEXTURE_2D);
	
	return textureID;
}
*/


void add_square (struct Buffer b [2], struct Drawer * d, float z)
{
	//Setup (n) pcs of (dim) dimensional vertices for one triangle.
	size_t const n = 6;
	size_t const dpos = 4;
	size_t const duv = 2;
	
	//Get the last avialable memory of the buffer 
	//which is where the triangle will be stored.
	float * v = (float *) b [0].last;
	float * uv = (float *) b [1].last;
	
	//Tell the buffer that we need (n) pcs of (dim) dimensional vertices.
	buffer_add_vertices (b + 0, dpos, n);
	buffer_add_vertices (b + 1, duv, n);
	drawer_add_vertices (d, GL_TRIANGLES, n);
	
	//v : (xyzw xyzw xyzw)
	vf32_setl (v, dpos, 0.0,  0.0, (double) z, 1.0);v += dpos;
	vf32_setl (v, dpos, 0.0,  0.5, (double) z, 1.0);v += dpos;
	vf32_setl (v, dpos, 0.5,  0.0, (double) z, 1.0);v += dpos;
	
	vf32_setl (v, dpos, 0.5,  0.5, (double) z, 1.0);v += dpos;
	vf32_setl (v, dpos, 0.0,  0.5, (double) z, 1.0);v += dpos;
	vf32_setl (v, dpos, 0.5,  0.0, (double) z, 1.0);
	
	vf32_setl (uv, duv, 0.0,  1.0);uv += duv;
	vf32_setl (uv, duv, 0.0,  0.0);uv += duv;
	vf32_setl (uv, duv, 1.0,  1.0);uv += duv;
	
	vf32_setl (uv, duv, 1.0,  0.0);uv += duv;
	vf32_setl (uv, duv, 0.0,  0.0);uv += duv;
	vf32_setl (uv, duv, 1.0,  1.0);
}


void add_triangle (struct Buffer * b, struct Drawer * d, float z)
{
	//Setup (n) pcs of (dim) dimensional vertices for one triangle.
	size_t const n = 3;
	size_t const dim = 4;
	
	//Get the last avialable memory of the buffer 
	//which is where the triangle will be stored.
	float * v = (float *) b->last;
	
	//Tell the buffer that we need (n) pcs of (dim) dimensional vertices.
	buffer_add_vertices (b, dim, n);
	drawer_add_vertices (d, GL_TRIANGLES, n);
	
	//v : (xyzw xyzw xyzw)
	vf32_setl (v, dim, 0.0,  0.0, (double) z, 1.0);
	v += dim;
	vf32_setl (v, dim, 0.0,  0.5, (double) z, 1.0);
	v += dim;
	vf32_setl (v, dim, 0.5,  0.0, (double) z, 1.0);
}


void add_grid (struct Buffer * b, struct Drawer * d)
{
	size_t const n = 4*20;
	size_t const dim = 4;
	
	//Get the last avialable memory of the buffer 
	//which is where the grid will be stored.
	float * v = (float *) b->last;
	
	//Tell the buffer that we need (n) pcs of (dim) dimensional vertices.
	buffer_add_vertices (b, dim, n);
	drawer_add_vertices (d, GL_LINES, n);
	
	struct Grid grid;
	vf32_setl (grid.p , dim, -1.0, 0.0, -1.0, 1.0);
	vf32_setl (grid.d1, dim,  0.1, 0.0,  0.0, 0.0);
	vf32_setl (grid.d2, dim,  0.0, 0.0,  0.1, 0.0);
	gen_grid (n, v, &grid);
}



void add_axis (struct Buffer * b, struct Drawer * d)
{
	size_t const n = 2;
	size_t const dim = 4;
	
	//Get the last avialable memory of the buffer 
	//which is where the axis will be stored.
	float * v = (float *) b->last;
	
	//Tell the buffer that we need (n) pcs of (dim) dimensional vertices.
	buffer_add_vertices (b, dim, 2);
	drawer_add_vertices (d, GL_LINES, n);
	
	vf32_setl (v, dim, 0.0, -0.5, 0.0, 1.0);
	v += 4;
	vf32_setl (v, dim, 0.0,  0.5, 0.0, 1.0);
}


void wrap_glVertexAttribPointer 
(
	GLuint program,
	char const * name,
	uint32_t dim,
	GLenum type,
	GLboolean normalized,
	uint32_t stride,
	uint32_t offset
)
{
	GLint loc = glGetAttribLocation (program, name);
	ASSERT_F (loc >= 0, "glGetAttribLocation (%i, %s) no attribute found", (int) program, "name");
	glVertexAttribPointer (loc, (GLint) dim, type, normalized, (GLsizei) stride, (const GLvoid *) (uintptr_t) offset);
	glEnableVertexAttribArray (loc);
}


void init_scene1 (GLuint program, GLuint vao, struct Drawer * drawer)
{
	struct Buffer buf [1];
	drawer_calloc (drawer, 100);
	buffer_calloc_vertices (buf, 4, 1000 * 1000);
	add_triangle (buf, drawer, 1.0f);
	add_triangle (buf, drawer, 2.0f);
	add_grid (buf, drawer);
	add_axis (buf, drawer);
	GLuint vbo [1];
	glGenBuffers (COUNTOF (vbo), vbo);
	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, vbo [0]);
	glBufferData (GL_ARRAY_BUFFER, buf->last_size8, buf->base, GL_DYNAMIC_DRAW);
	wrap_glVertexAttribPointer (program, "pos", 3, GL_FLOAT, GL_FALSE, sizeof (float) * 4, 0);
}


void init_scene2 (GLuint program, GLuint vao, struct Drawer * drawer)
{
	struct Buffer buf [2];
	drawer_calloc (drawer, 10);
	buffer_calloc_vertices (buf + 0, 4, 1000);
	buffer_calloc_vertices (buf + 1, 2, 1000);
	add_square (buf, drawer, 10.0f);
	GLuint vbo [2];
	glGenBuffers (2, vbo);
	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, vbo [0]);
	glBufferData (GL_ARRAY_BUFFER, buf->last_size8, buf->base, GL_DYNAMIC_DRAW);
	wrap_glVertexAttribPointer (program, "pos", 3, GL_FLOAT, GL_FALSE, sizeof (float) * 4, 0);
	glBindBuffer (GL_ARRAY_BUFFER, vbo [1]);
	glBufferData (GL_ARRAY_BUFFER, buf->last_size8, buf->base, GL_DYNAMIC_DRAW);
	wrap_glVertexAttribPointer (program, "uv", 2, GL_FLOAT, GL_FALSE, sizeof (float) * 2, 0);
	
	uint32_t width = 10;
	uint32_t height = 10;
	uint8_t * image = malloc (width * height);
	for (size_t x = 0; x < width; ++x)
	for (size_t y = 0; y < height; ++y)
	{
		image [width * x + y] = (x * y);
	}
	
	GLuint tex [1];
	glGenTextures (1, tex);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, tex [0]);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap (GL_TEXTURE_2D);
	
	glUseProgram (program);
	glUniform1i (glGetUniformLocation (program, "tex"), 0);
	
	
	GL_CHECK_ERROR;
}


#define RENDER_COUNT 2


struct Render
{
	GLuint vao;
	GLuint program;
	GLint umvp;
	struct Drawer * drawer;
};

void render_update (size_t n, struct Render * r, float PVM [16])
{
	for (size_t i = 0; i < n; ++i)
	{
		glBindVertexArray (r [i].vao);
		glUseProgram (r [i].program);
		glUniformMatrix4fv (r [i].umvp, 1, GL_FALSE, PVM);
		drawer_draw (r [i].drawer);
	}
}


int main (int argc, char *argv[])
{
	struct Application app;
	app_init (&app, argc, argv);
	
	

	struct Drawer drawer [RENDER_COUNT];
	GLuint vao [RENDER_COUNT];
	glGenVertexArrays (RENDER_COUNT, vao);
	GLuint program [RENDER_COUNT];
	GLint umvp [RENDER_COUNT];	
	program [0] = gl_program_from_filename ("src/template_3d/shader.glvs;src/template_3d/shader.glfs");
	program [1] = gl_program_from_filename ("src/template_3d/uv.glvs;src/template_3d/uv.glfs");
	umvp [0] = glGetUniformLocation (program [0], "mvp");
	umvp [1] = glGetUniformLocation (program [1], "mvp");
	ASSERT_F (umvp [0] >= 0, "%s", "glGetUniformLocation no uniform found.");
	ASSERT_F (umvp [1] >= 0, "%s", "glGetUniformLocation no uniform found.");
	init_scene1 (program [0], vao [0], drawer + 0);
	init_scene2 (program [1], vao [1], drawer + 1);
	gl_program_debug (2, program);
	GL_CHECK_ERROR;

	struct Render render [RENDER_COUNT];
	render [0].program = program [0];
	render [0].vao = vao [0];
	render [0].drawer = drawer + 0;
	render [0].umvp = umvp [0];
	render [1].program = program [1];
	render [1].vao = vao [1];
	render [1].drawer = drawer + 1;
	render [1].umvp = umvp [1];
	
	

	//wrap_glVertexAttribPointer (app.program, "uv", 3, GL_FLOAT, GL_FALSE, sizeof (float) * 2, 0);
	
	
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
				}
				break;
			}
		}
		
		app_update_camera (&app);
		render_update (RENDER_COUNT, render, app.cam.PVM);
		app_frame_end (&app);
	}

	app_destroy (&app);

	return 0;
}










