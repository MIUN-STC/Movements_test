#pragma once

#include "v4.h"
#include "classy.h"


// XXXXYYYYZZZZ XXXXYYYYZZZZ
// X   Y   Z
// 

void vertex_layout (GLuint program, GLuint * vao, GLuint * vbo, size_t size8, void * data)
{
	glGenVertexArrays (1, vao);
	glGenBuffers (1, vbo);
	GLuint loc;
	loc = glGetAttribLocation (program, "pos");
	ASSERT_F (loc >= 0, "glGetAttribLocation no attribute found.");
	glBindVertexArray (*vao);
	glBindBuffer (GL_ARRAY_BUFFER, *vbo);
	glBufferData (GL_ARRAY_BUFFER, size8, data, GL_DYNAMIC_DRAW);
	glVertexAttribPointer (loc, 3, GL_FLOAT, GL_FALSE, sizeof (struct v4f32_txyz), (void*) 0);
	glEnableVertexAttribArray (loc);
}






