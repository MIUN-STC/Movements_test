/*
https://askubuntu.com/questions/244133/how-do-i-get-egl-and-opengles-libraries-for-ubuntu-running-on-virtualbox
https://gist.github.com/SuperV1234/5c5ad838fe5fe1bf54f9
https://www.haroldserrano.com/blog/how-to-apply-textures-to-a-character-in-ios
https://www.sallyx.org/sally/c/opengl/fps
http://www.learnopengles.com/tag/linmath-h/

https://github.com/KhronosGroup/Vulkan-LoaderAndValidationLayers/blob/master/demos/cube.c


https://wiki.libsdl.org/SDL_RegisterEvents
*/
#include <SDL2/SDL.h>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengles2.h>

#include <stdio.h>
#include <sys/stat.h>

//getopt()
#include <unistd.h>

#include "logo.h"
#include "debug.h"
#include "mat.h"
#include "cam.h"
#include "common_gl.h"
#include "option.h"
#include "camera.h"
#include "vertex.h"

#define APP_SHORT_NAME "Short app name"
#define APP_WINDOW_X SDL_WINDOWPOS_UNDEFINED
#define APP_WINDOW_Y SDL_WINDOWPOS_UNDEFINED
#define APP_WINDOW_HEIGHT 1080
#define APP_WINDOW_WIDTH 1920
#define APP_WINDOW_ASPECT (APP_WINDOW_WIDTH / APP_WINDOW_HEIGHT)
#define APP_WINDOW_FOV (45.0f * M_PI / 180.0f)
#define APP_CAMERA_DEVICE_READY 1
#define APP_4K_ASPECT (3840.0f/2160.0f)

struct Vertex verts [4] = 
{
	//   x      y     s      t
	{ -1.0f, -1.0f, 0.0f,  1.0f }, // BL
	{ -1.0f,  1.0f, 0.0f,  0.0f }, // TL
	{  1.0f,  1.0f, 1.0f,  0.0f }, // TR
	{  1.0f, -1.0f, 1.0f,  1.0f } // BR
};


const GLint indicies[] = 
{
	0, 1, 2, 0, 2, 3
};


GLuint app_create_texture (GLuint program, struct Gev_Cam * cam)
{
	uint32_t width;
	uint32_t height;
	
	int r;
	r = cam_get_u32 (cam, "Width", &width);
	if (r) {goto error;}
	r = cam_get_u32 (cam, "Height", &height);
	if (r) {goto error;}
	
	GLuint texture;
	glGenTextures (1, &texture);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, texture);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);
	ASSERT_F (glGetError () == GL_NO_ERROR, "Creating GPU texture failed%s", "");
	glUniform1i (glGetUniformLocation (program, "tex"), 0);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	//glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0);
	//ASSERT_F (glGetError () == GL_NO_ERROR, "GL_TEXTURE_MAX_ANISOTROPY_EXT%s", "");
	
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei (GL_PACK_ALIGNMENT, 1);
	return texture;
error:
	return 0;
}


void app_update_texture (GLuint program, struct Gev_Image * image)
{
	ASSERT_F (program != 0, "Invalid GL program%s", "");
/*
void glTexSubImage2D
(
	GLenum target,
	GLint level,
	GLint xoffset,
	GLint yoffset,
	GLsizei width,
	GLsizei height,
	GLenum format,
	GLenum type,
	const GLvoid * pixels
);
*/
	glGetError ();
	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, image->w, image->h, GL_ALPHA, GL_UNSIGNED_BYTE, image->address);
	ASSERT_F (glGetError () == GL_NO_ERROR, "Updating GPU texture failed%s", "");
}


struct Option opt;
struct Gev_Cam cam;

int app_cam_thread_run = 1;
static int app_cam_thread (void *ptr)
{
	int r;
	r = cam_open (&cam);
	if (r) {goto error;}
	r = cam_load (&cam, opt.config_file);
	if (r) {goto error;}	
	r = cam_allocate_frame_buffer (&cam, 10);
	if (r) {goto error;}
	r = cam_start (&cam);
	if (r) {goto error;}
	
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.code = APP_CAMERA_DEVICE_READY;
	SDL_PushEvent (&event);
	
	return 0;
error:
	TRACE_F ("Error%s", "");
	cam_close (&cam);
	return 1;
}


int main(int argc, char *argv[])
{
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	if (opt.help_flag)
	{
		opt_help (&opt);
		return 0;
	}
	

	SDL_Window * window;
	SDL_GLContext context;
	const Uint8 * keyboard;
	GLuint program;
	GLuint texture;
	GLuint uniform_time;
	GLuint m_vbo;
	GLuint m_ebo;
    SDL_Thread * cam_thread;
    int present_image = 0;
    
    
	{
		int r = SDL_Init (SDL_INIT_VIDEO);
		ASSERT_F (r == 0, "SDL_Init: %s", SDL_GetError());
	}
	
	
	cam_thread = SDL_CreateThread (app_cam_thread, "TestThread", (void *)NULL);
	
	window = SDL_CreateWindow (APP_SHORT_NAME, APP_WINDOW_X, APP_WINDOW_Y, APP_WINDOW_WIDTH, APP_WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
	ASSERT_F (window != NULL, "SDL_CreateWindow: %s", SDL_GetError());
	
	keyboard = SDL_GetKeyboardState (NULL);
	
	// Initialize rendering context
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
	
	context = SDL_GL_CreateContext (window);
	ASSERT_F (context != NULL, "SDL_GL_CreateContext: %s", SDL_GetError());

	// Use VSYNC
	SDL_GL_SetSwapInterval (1);
	
	struct Shader shaders [2] = 
	{
		{0, GL_VERTEX_SHADER, "src/camera_texture_2dspace/cam.glvs"},
		{0, GL_FRAGMENT_SHADER, "src/camera_texture_2dspace/cam.glfs"}
	};
	program = gl_create_program (shaders, 2);
	
	glUseProgram (program);
	
	// Populate vertex buffer
	glGenBuffers (1, &m_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	// Populate element buffer
	glGenBuffers (1, &m_ebo);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

	gl_setup_vertex_layout (program);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	uniform_time = glGetUniformLocation (program, "time");
	ASSERT_F (context != NULL, "glGetUniformLocation: %s", SDL_GetError());
	
	
	float time = 0;
	
	printf("Running...\n");
	while (1)
	{
		SDL_Event event;
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
				app_cam_thread_run = 0;
				goto main_quit;
				
				case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
					printf ("SDL_WINDOWEVENT_CLOSE");
					break;
				}
				break;
				
				case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:

					event.type = SDL_QUIT;
					SDL_PushEvent (&event);
				}
				break;
				
				case SDL_USEREVENT:
				if (event.user.code == APP_CAMERA_DEVICE_READY)
				{
					texture = app_create_texture (program, &cam);
					present_image = 1;
				}
				break;
			}
		}
		if (present_image)
		{
			struct Gev_Image * image = NULL;
			int r = cam_read (&cam, &image);
			if (r)
			{
				//event.type = SDL_QUIT;
				//SDL_PushEvent (&event);
			}
			if (image != NULL)
			{
				time += 0.1f;
				glUniform1f (uniform_time, time);
				ASSERT_F (glGetError () == GL_NO_ERROR, "glUniform1f%s", "");
				//TRACE_F ("image id %i %i %i %f", image->id, (int)image->w, (int)image->h, time);
				app_update_texture (program, image);
				cam_release (&cam, image);
			}
		}
		glUniform1f (uniform_time, time);
		glClear (GL_COLOR_BUFFER_BIT);
		glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		SDL_GL_SwapWindow (window);
	}
	
main_quit:

	printf ("Exiting...\n");
	glUseProgram (0);
	glDisableVertexAttribArray (0);
	glDeleteProgram (program);
	glDeleteTextures (1, &texture);
	glDeleteBuffers (1, &m_ebo);
	glDeleteBuffers (1, &m_vbo);

	SDL_GL_DeleteContext (context);
	SDL_DestroyWindow (window);
	{
		int r;
		SDL_WaitThread (cam_thread, &r);
		cam_close (&cam);
		printf ("thread return %i\n", r);
	}
	TRACE_F ("SDL_Quit%s", "");
	SDL_Quit();
	return 0;
}
