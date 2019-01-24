#pragma once

#include "v.h"
#include "c.h"


struct Vertex
{
	union
	{
		struct v4f32_XYZW p_xyzw;
		float p [4];
	};
	union
	{
		struct v4f32_RGBA c_rgba;
		float c [4];
	};
};

struct Vertex4
{
	struct Vertex v [4];
};


struct Vertex Vertex (float x, float y, float z, float w, float r, float g, float b, float a)
{
	struct Vertex v;
	v.p [0] = x;
	v.p [1] = y;
	v.p [2] = z;
	v.p [3] = w;
	v.c [0] = r;
	v.c [1] = g;
	v.c [2] = b;
	v.c [3] = a;
	return v;
}


void vertex_print (FILE * f, struct Vertex const * v)
{
	vf32_print (f, (float*) v, 8, "%2.2f");
	fflush (f);
}


void vertex_print_array (FILE * f, size_t n, struct Vertex const v [])
{
	for (size_t i = 0; i < n; ++i)
	{
		vf32_print (f, (float*) v, 8, "%2.2f ");
		fflush (f);
	}
}


void vertex_sample1 (int * n_io, struct Vertex * v)
{
	ASSERT (n_io [0] >= 3);
	v [0] = Vertex ( 0.5f,  0.5f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v [1] = Vertex ( 0.5f, -0.5f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v [2] = Vertex (-0.5f, -0.5f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	n_io [0] = 3;
}


#define MESH_ERROR            1 << 0
#define MESH_UPDATE_ONCE      1 << 1
#define MESH_UPDATE_ALLWAYS   1 << 2
#define MESH_DRAW_ONCE        1 << 3
#define MESH_DRAW_ALLWAYS     1 << 4


struct Mesh
{
	uint32_t flags;
	GLenum mode;
	GLint first;
	GLsizei count;
	void * data;
};


struct VA
{
	GLuint vao;
	GLuint vbo;
	GLuint program;
	size_t count;
	struct Mesh * meshes;
};


void va_init (struct VA * va, GLuint program, size_t n, void * data)
{
	size_t const size8 = n * sizeof (struct Vertex);
	glGenVertexArrays (1, &va->vao);
	glGenBuffers (1, &va->vbo);
	GLuint l [2];
	l [0] = glGetAttribLocation (program, "pos");
	l [1] = glGetAttribLocation (program, "col");
	ASSERT_F (l [0] >= 0, "glGetAttribLocation no attribute found.");
	ASSERT_F (l [1] >= 0, "glGetAttribLocation no attribute found.");
	glBindVertexArray (va->vao);
	glBindBuffer (GL_ARRAY_BUFFER, va->vbo);
	glBufferData (GL_ARRAY_BUFFER, size8, data, GL_DYNAMIC_DRAW);
	glVertexAttribPointer (l [0], 4, GL_FLOAT, GL_FALSE, sizeof (struct Vertex), (void*) offsetof (struct Vertex, p));
	glEnableVertexAttribArray (l [0]);
	glVertexAttribPointer (l [1], 4, GL_FLOAT, GL_FALSE, sizeof (struct Vertex), (void*) offsetof (struct Vertex, c));
	glEnableVertexAttribArray (l [1]);
}


void va_draw (struct VA * va)
{
	size_t const n = va->count;
	struct Mesh * m = va->meshes;
	glBindVertexArray (va->vao);
	for (size_t i = 0; i < n; ++ i)
	{
		uint32_t const flags = m [i].flags;
		if (!(flags & (MESH_DRAW_ALLWAYS | MESH_DRAW_ONCE))) {continue;}
		if (flags & MESH_DRAW_ONCE) {m [i].flags &= ~MESH_DRAW_ONCE;}
		GLenum const mode = m [i].mode;
		GLint const first = m [i].first;
		GLsizei const count = m [i].count;
		glDrawArrays (mode, first, count);
	}
}


void va_update (struct VA * va)
{
	size_t const n = va->count;
	struct Mesh * m = va->meshes;
	GLenum const target = GL_ARRAY_BUFFER;
	glBindBuffer (target, va->vbo);
	for (size_t i = 0; i < n; ++ i)
	{
		uint32_t const flags = m [i].flags;
		if (!(flags & (MESH_UPDATE_ALLWAYS | MESH_UPDATE_ONCE))) {continue;}
		if (flags & MESH_UPDATE_ONCE) {m [i].flags &= ~MESH_UPDATE_ONCE;}
		GLintptr const offset8 = m [i].first * sizeof (struct Vertex);
		GLsizeiptr const size8 = m [i].count * sizeof (struct Vertex);
		GLvoid * data = m [i].data;
		TRACE_F ("mesh %i : glBufferSubData : %i B", i, size8);
		glBufferSubData (target, offset8, size8, data);
	}
}
