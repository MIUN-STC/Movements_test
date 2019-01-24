#pragma once


struct Shader
{
	GLuint handle;
	GLenum kind;
	char * filename;
};


void gl_create_shader (struct Shader * shader)
{
	ASSERT_F (shader->filename != NULL, "filename is NULL%s", "");
	ASSERT_F 
	(
		(shader->kind == GL_VERTEX_SHADER) || 
		(shader->kind == GL_FRAGMENT_SHADER), 
		"Unsupported shader kind%s", ""
	);
	shader->handle = glCreateShader (shader->kind);
	ASSERT_F (shader != 0, "glCreateShader error%s", "");
	struct stat fileinfo;
	{
		int r = stat (shader->filename, &fileinfo);
		ASSERT_F (r == 0, "stat error%s", "");
	}
	FILE * file = fopen (shader->filename, "r");
	char * buffer = malloc (fileinfo.st_size + 1);
	//witout memset we get a strange gl compile error.
	memset (buffer, 0, fileinfo.st_size + 1);
	{
		size_t r = fread (buffer, fileinfo.st_size, 1, file);
		ASSERT_F (r == 1, "fread error %i %i", (int)r, (int)fileinfo.st_size);
	}
	fclose (file);
	errno = 0;
	glShaderSource (shader->handle, 1, (const GLchar **) &buffer, NULL);
	free (buffer);
	glCompileShader (shader->handle);
	GLint status;
	glGetShaderiv (shader->handle, GL_COMPILE_STATUS, &status);
	char err_buf [512];
	if (status != GL_TRUE)
	{
		glGetShaderInfoLog (shader->handle, sizeof (err_buf), NULL, err_buf);
		err_buf [sizeof (err_buf) - 1] = '\0';
	}
	ASSERT_F (status == GL_TRUE, "Vertex shader compilation failed: %s", err_buf);
}


GLuint gl_create_program (struct Shader * shaders, size_t count)
{
	GLuint program;
	program = glCreateProgram ();
	ASSERT_F (program != 0, "glCreateProgram error%s", "");
	for (size_t i = 0; i < count; i = i + 1)
	{
		gl_create_shader (shaders + i);
		glAttachShader (program, shaders [i].handle);
	}
	glLinkProgram (program);
	//The shader objects are needed only for linking the program.
	//We can delete them after the program is linked.
	for (size_t i = 0; i < count; i = i + 1)
	{
		glDetachShader (program, shaders [i].handle);
		glDeleteShader (shaders [i].handle);
		shaders [i].handle = 0;
	}
	return program;
}
