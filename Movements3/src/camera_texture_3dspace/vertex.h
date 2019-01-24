#pragma once

#include <SDL2/SDL_opengles2.h>


struct Vertex
{
	float pos [4];
	float col [4];
	float tex [2];
};


void gen_grid (struct Vertex * verts, size_t count, float x1, float x2, float y1, float y2, float r)
{
	float xx;
	float yy;
	//float const xd = (x2 - x1);
	//float const yd = (y2 - y1);
	size_t j = 0;
	for (size_t i = 0; i < count; i = i + 1)
	{

		if (j == count){return;}
		xx = x1 + r * (float)i;
		yy = y1;
		V4_SET (verts [j].pos, xx, -0.5f, yy, 1.0f);
		V4_SET (verts [j].col, 1.0f, 0.0f, 0.0f, 1.0f);
		j = j + 1;
		
		if (j == count){return;}
		xx = x1 + r * (float)i;
		yy = y2;
		V4_SET (verts [j].pos, xx, -0.5f, yy, 1.0f);
		V4_SET (verts [j].col, 1.0f, 0.0f, 0.0f, 1.0f);
		j = j + 1;

		if (j == count){return;}
		xx = x1;
		yy = y1 + r * (float)i;
		V4_SET (verts [j].pos, xx, -0.5f, yy, 1.0f);
		V4_SET (verts [j].col, 1.0f, 0.0f, 0.0f, 1.0f);
		j = j + 1;
		
		if (j == count){return;}
		xx = x2;
		yy = y1 + r * (float)i;
		V4_SET (verts [j].pos, xx, -0.5f, yy, 1.0f);
		V4_SET (verts [j].col, 1.0f, 0.0f, 0.0f, 1.0f);
		j = j + 1;
		
	}
}


void gl_setup_vertex_layout (GLuint program)
{
	// Bind vertex position attribute
	GLint pos_attr_loc = glGetAttribLocation (program, "in_Position");
	printf ("pos_attr_loc %i \n", pos_attr_loc);
	glVertexAttribPointer (pos_attr_loc, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray (pos_attr_loc);
	
	GLint col_attr_loc = glGetAttribLocation (program, "in_Color");
	printf ("col_attr_loc %i \n", col_attr_loc);
	glVertexAttribPointer (col_attr_loc, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (void*)(4*sizeof(GLfloat)));
	glEnableVertexAttribArray (col_attr_loc);

	// Bind vertex texture coordinate attribute
	GLint tex_attr_loc = glGetAttribLocation (program, "in_Texcoord");
	printf ("tex_attr_loc %i \n", tex_attr_loc);
	glVertexAttribPointer (tex_attr_loc, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (void*)(8*sizeof(GLfloat)));
	glEnableVertexAttribArray (tex_attr_loc);
}
