#pragma once

#include "bufv.h"
#include "shader.h"

//OpenGL Command.
#define GLC_CHANGE_VBO     1
#define GLC_CHANGE_VAO     2
#define GLC_CHANGE_PROGRAM 3
#define GLC_DRAW_ARRAY     4
#define GLC_UPDATE_DATA    5
#define GLC_UPDATE_PROGRAM 6
#define GLC_DELETE_PROGRAM 7


//OpenGL Handler
struct GLH
{
	struct BUFV (GLuint, vao);
	struct BUFV (GLuint, vbo);
	struct BUFV (GLuint, program);
	struct BUFV (char const *, program_name);
};

struct GLC_Draw
{
	GLenum mode;
	GLint first;
	GLsizei count;
};

struct GLC_Data
{
	void * memory;
	GLenum target;
	GLintptr offset;
	GLsizeiptr size;
};

struct GLC_Program_Name
{
	int iprogram;
	int iname;
};

struct GLC
{
	uint32_t command;
	union
	{
		int iprogram;
		int ivao;
		int ivbo;
		struct GLC_Draw draw;
		struct GLC_Data data;
		struct GLC_Program_Name program_name;
	};
};





void glh_init (struct GLH * g)
{
	/*
	glGenVertexArrays (g->vao.n, g->vao.p);
	glGenBuffers (g->vbo.n, g->vbo.p);
	for (size_t i = 0; i < g->program.n; ++ i)
	{
		g->program.p [i] = glCreateProgram ();
	}
	*/
}


void glc_update_program (struct GLH * h, struct GLC_Program_Name const * p)
{
	//int const        iprogram = p->iprogram;
	int const          iname = p->iname;
	char const **      names = h->program_name.p;
	char const *        name = names [iname];
	GLuint program = gl_program_from_filename (name);
	gl_program_debug (1, &program);
}




void glc_exec (struct GLH * h, size_t n, struct GLC * c)
{
	for (size_t i = 0; i < n; ++ i)
	{
		uint32_t const command = c [i].command;
		/*
		GLuint program = c [i].program;
		GLuint vao = c [i].vao;
		struct GLC_VBO * vbo = &c [i].vbo;
		struct GLC_Draw * draw = &c [i].draw;
		struct GLC_Data * data = &c [i].data;
		struct GLC_LProgram * lprogram = &c [i].lprogram;
		*/

		
		switch (command)
		{
			/*
			case GLC_CHANGE_PROGRAM:
			glUseProgram (program);
			break;
			
			case GLC_CHANGE_VAO:
			glBindVertexArray (vao);
			break;
			
			case GLC_CHANGE_VBO:
			glBindBuffer (vbo->target, vbo->handle);
			break;
			
			case GLC_DRAW_ARRAY:
			glDrawArrays (draw->mode, draw->first, draw->count);
			break;
			
			case GLC_UPDATE_DATA:
			glBufferSubData (data->target, data->offset, data->size, data->memory);
			break;
			*/
			
			case GLC_UPDATE_PROGRAM:
			{
				TRACE ("GLC_UPDATE_PROGRAM");
				glc_update_program (h, &c->program_name);

			}
			//glc_create_shader (lprogram->filenames);
			break;
			



		}
	}
}


/*
GLC_CHANGE_VBO (1)
GLC_UPDATE     (GL_ARRAY_BUFFER 0 10 data)
GLC_CHANGE_VAO (2)
GLC_DRAW_ARRAYS (GL_POINTS 0 10)
GLC_DRAW_ARRAYS (GL_POINTS 20 25)


*/




