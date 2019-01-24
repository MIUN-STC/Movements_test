#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glcorearb.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h>

#include "global.h"
#include "debug.h"
#include "debug_gl.h"
#include "misc.h"
#include "camera.h"
#include "gen.h"
#include "shader.h"
#include "vertex.h"
#include "app.h"
#include "q.h"
#include "poi.h"
#include "map.h"


void setup_texture (GLuint tex, size_t w, size_t h)
{
	//GL_RGBA
	size_t d = 4;
	glBindTexture (GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	uint8_t * data = malloc (w * h * d);
	//gen_pixmap_sample (data, w, h, d);
	uint8_t pallete [100 * 4];
	gen_mandelbrot_pallete (pallete, 100, 4);
	gen_mandelbrot_pixmap (data, pallete, w, h, d, 1000);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap (GL_TEXTURE_2D);
	free (data);
}




GLuint setup_verts ()
{
	float vertices [] = 
	{
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices [] = 
	{
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	glGenVertexArrays (1, &vao);
	glGenBuffers (1, &vbo);
	glGenBuffers (1, &ebo);
	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray (0);

	// color attribute
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray (1);

	// texture coord attribute
	glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray (2);

	return vao;
}






int main (int argc, char *argv[])
{
	struct Application app;
	app_init (&app, argc, argv);
	
	GLuint vao = setup_verts ();
	GLuint tex [1];
	glGenTextures (COUNTOF (tex), tex);
	setup_texture (tex [0], 4000, 4000);
	//setup_texture (tex [1], 10, 10);
	
	
	
	while (1)
	{
		if (app.flags & APP_QUIT) {break;}
		if (app.flags & APP_OPENGL_ERROR) {break;}
		app_frame_begin (&app);
		while (SDL_PollEvent (&app.event))
		{
			app_handle_input (&app);
		}
		

		app_update_camera (&app);
		
        glActiveTexture (GL_TEXTURE0);
        glBindTexture (GL_TEXTURE_2D, tex [0]);
        glBindVertexArray (vao);
        glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
		app_frame_end (&app);
	}
	app_destroy (&app);
	return 0;
}










