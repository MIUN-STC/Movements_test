#pragma once

#include <SDL2/SDL_opengles2.h>


struct Vertex
{
	float pos [2];
	float tex [2];
};


void gl_setup_vertex_layout (GLuint program)
{
	// Bind vertex position attribute
	GLint pos_attr_loc = glGetAttribLocation (program, "in_Position");
	printf ("pos_attr_loc %i \n", pos_attr_loc);
	glVertexAttribPointer (pos_attr_loc, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray (pos_attr_loc);

	// Bind vertex texture coordinate attribute
	GLint tex_attr_loc = glGetAttribLocation (program, "in_Texcoord");
	printf ("tex_attr_loc %i \n", tex_attr_loc);
	glVertexAttribPointer (tex_attr_loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
	glEnableVertexAttribArray (tex_attr_loc);
}
