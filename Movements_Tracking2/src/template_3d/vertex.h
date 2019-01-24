#pragma once

#include "v4.h"


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
	glVertexAttribPointer (loc, 3, GL_FLOAT, GL_FALSE, sizeof (float) * 4, (void*) 0);
	glEnableVertexAttribArray (loc);
}







GLuint vertex_layout1 (GLint loc_pos, GLint loc_col, size_t size8, void * data)
{
	ASSERT_F (loc_pos >= 0, "glGetAttribLocation invalid attribute.");
	ASSERT_F (loc_col >= 0, "glGetAttribLocation invalid attribute.");
	GLuint vao [1];
	GLuint vbo [2];
	glGenVertexArrays (COUNTOF (vao), vao);
	glGenBuffers (COUNTOF (vbo), vbo);
	glBindVertexArray (vao [0]);
	
	glBindBuffer (GL_ARRAY_BUFFER, vbo [0]);
	glBufferData (GL_ARRAY_BUFFER, size8, data, GL_DYNAMIC_DRAW);
	glVertexAttribPointer (loc_pos, 3, GL_FLOAT, GL_FALSE, sizeof (float) * 4, (void*) 0);
	glEnableVertexAttribArray (loc_pos);
	
	glBindBuffer (GL_ARRAY_BUFFER, vbo [1]);
	glBufferData (GL_ARRAY_BUFFER, size8, data, GL_DYNAMIC_DRAW);
	glVertexAttribPointer (loc_col, 3, GL_FLOAT, GL_FALSE, sizeof (float) * 4, (void*) 0);
	glEnableVertexAttribArray (loc_col);
	
	return vao [0];
}



void gl_subdata 
(
	GLenum const target, 
	size_t n, 
	GLint const first [],
	GLsizei const count [],
	GLsizeiptr const esize8, 
	GLvoid const * data []
)
{
	for (size_t i = 0; i < n; ++ i)
	{
		GLintptr const offset8 = esize8 * first [i];
		GLsizeiptr const size8 = esize8 * count [i];
		glBufferSubData (target, offset8, size8, data [i]);
	}
}





void gl_draw 
(
	size_t n, 
	GLenum const mode [], 
	GLint const first [],
	GLsizei const count []
)
{
	for (size_t i = 0; i < n; ++ i)
	{
		glDrawArrays (mode [i], first [i], count [i]);
	}
}








