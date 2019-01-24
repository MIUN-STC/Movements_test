#pragma once

#include "v.h"
#include "v4.h"
#include "c.h"
#include "bufv.h"
#include "debug.h"
#include "alg.h"


struct Vertex
{
	float p [4];
	float c [4];
};


void vertex_layout (GLuint program, GLuint * vao, GLuint * vbo, size_t size8, void * data)
{
	glGenVertexArrays (1, vao);
	glGenBuffers (1, vbo);
	GLint l [2];
	l [0] = glGetAttribLocation (program, "pos");
	l [1] = glGetAttribLocation (program, "col");
	ASSERT_F (l [0] >= 0, "%s", "glGetAttribLocation no attribute found.");
	ASSERT_F (l [1] >= 0, "%s", "glGetAttribLocation no attribute found.");
	glBindVertexArray (*vao);
	glBindBuffer (GL_ARRAY_BUFFER, *vbo);
	glBufferData (GL_ARRAY_BUFFER, size8, data, GL_DYNAMIC_DRAW);
	glVertexAttribPointer (l [0], 4, GL_FLOAT, GL_FALSE, sizeof (struct Vertex), (void*) offsetof (struct Vertex, p));
	glEnableVertexAttribArray (l [0]);
	glVertexAttribPointer (l [1], 4, GL_FLOAT, GL_FALSE, sizeof (struct Vertex), (void*) offsetof (struct Vertex, c));
	glEnableVertexAttribArray (l [1]);
}



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
		//vf32_print (f, (float*) v, 8, "%2.2f ");
		fflush (f);
	}
}


void vertex_sample1 (size_t * n_io, struct Vertex * v)
{
	ASSERT (n_io [0] >= 3);
	v [0] = Vertex ( 0.5f,  0.5f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v [1] = Vertex ( 0.5f, -0.5f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v [2] = Vertex (-0.5f, -0.5f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	n_io [0] = 3;
}







void vertex_from_poi 
(
	struct Vertex * vertex, 
	struct POI const * poi
)
{
	float * v = vertex->p;
	float * c = vertex->c;
	float const * p = poi->p;
	size_t const g = poi->g;
	v [0] = p [0];
	v [1] = p [1];
	v [2] = p [2];
	v [3] = 1.0f;
	rgb_random (c, g);
	c [3] = 1.0f; // Alpha
	//TRACE_F ("%f %f %f %li", c [0], c [1], c [2], g);
}




void vertex_array_from_poi 
(
	size_t n, 
	struct Vertex v [], 
	struct POI const p []
)
{
	for (size_t i = 0; i < n; ++ i)
	{
		//vf32_print (stdout, p [i].p, 3, "%f ");
		vertex_from_poi (v + i, p + i);
	}
}




void vertex_array_from_poi_edge 
(
	size_t * vn, 
	struct Vertex v [], 
	size_t pn,
	struct POI const p []
)
{
	size_t j = 0;
	for (size_t i = 0; i < pn; ++ i)
	{
		struct POI const * a = p + i;
		struct POI const * b = a->next;
		if (b == NULL) {continue;}
		//if (b->q < 5) {continue;}
		//if (b->dotavg < 0.01) {continue;}
		//printf ("%f\n", b->dotavg);
		
		if (j >= (*vn)) {break;}
		vertex_from_poi (v + j, a);
		//vf32_print (stdout, v [j].p, 4, "%2.2f ");
		
		j ++;
		if (j >= (*vn)) {break;}
		vertex_from_poi (v + j, b);
		//vf32_print (stdout, v [j].p, 4, "%2.2f ");
		
		j ++;
		
		
		
	}
	(*vn) = j;
	TRACE_F ("n : %i", j);
}


