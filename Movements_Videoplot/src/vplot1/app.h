#pragma once

#include "debug.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "extra.h"
#include "grid.h"




#define APP_WINDOW_WIDTH 1024
#define APP_WINDOW_HEIGHT 768
#define APP_TITLE "Video and plot"

/*
#define APP_QUIT 1U << 0
#define APP_ERROR 1U << 1
#define APP_VIDEO_END 1U << 2
*/

enum App_Flags
{
	APP_QUIT = 1U << 0,
	APP_ERROR = 1U << 1,
	APP_VIDEO_END = 1U << 2,
	APP_MOUSE_DRAG = 1U << 3
};

#define APP_VALUES_MAX 100000
#define APP_SHADER_FILES_XYUV "src/vplot1/vplot1.glvs;src/vplot1/vplot1.glfs"
#define APP_SHADER_XYUV 0
#define APP_UNIFORM_TRANSFORM 0
#define APP_UNIFORM_COLOR 1
#define APP_BUF_SIZE (1000000*sizeof (float)*4)










void app_init ()
{
	int r = SDL_Init (SDL_INIT_VIDEO);
	ASSERT_F (r == 0, "SDL_Init %i\n", SDL_GetError ());
}


SDL_Window * app_create_window ()
{
	SDL_Window * window = NULL;
	window = SDL_CreateWindow 
	(
		APP_TITLE, 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		APP_WINDOW_WIDTH, 
		APP_WINDOW_HEIGHT, 
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);
	ASSERT_F (window != NULL, "SDL_CreateWindow: %s", SDL_GetError());
	return window;
}


void app_setup_texture (uint32_t * flags, cv::VideoCapture & c, GLuint tex)
{
	cv::Mat m;
	int r = c.read (m);
	if (!r) 
	{
		(*flags) |= APP_VIDEO_END; 
		return;
	}
	gltex_setup_from_cvmat (tex, m);
}


void app_update_texture (uint32_t * flags, cv::VideoCapture & c, GLuint tex, double * frame)
{
	cv::Mat m;
	int r = c.read (m);
	if (!r) 
	{
		(*flags) |= APP_VIDEO_END; 
		return;
	}
	(*frame) = c.get (CV_CAP_PROP_POS_FRAMES);
	gltex_update_from_cvmat (tex, m);
}


















enum App_TexName
{
	APP_TEX_NONE,
	APP_TEX_VID1,
	APP_TEX_VID2,
	APP_TEX_COUNT
};


enum App_RenderBoxName
{
	APP_RENDERBOX_VID1,
	APP_RENDERBOX_VID2,
	APP_RENDERBOX_PLOT1,
	APP_RENDERBOX_PLOT2,
	APP_RENDERBOX_100,
	APP_RENDERBOX_1,
	APP_RENDERBOX_COUNT
};


enum App_RenderColorName
{
	APP_RENDERCOLOR_ZERO,
	APP_RENDERCOLOR_1,
	APP_RENDERCOLOR_2,
	APP_RENDERCOLOR_3,
	APP_RENDERCOLOR_GRAY,
	APP_RENDERCOLOR_BLUE,
	APP_RENDERCOLOR_WHITE,
	APP_RENDERCOLOR_COUNT
};

enum App_Transform
{
	APP_TRANSFORM_ZERO,
	APP_TRANSFORM_PLOT,
	APP_TRANSFORM_COUNT
};


enum App_MeshName
{
	APP_MESH_SQUARE,
	APP_MESH_VLINE,
	APP_MESH_PLOT1,
	APP_MESH_PLOTP1,
	APP_MESH_PLOT2,
	APP_MESH_PLOTP2,
	APP_MESH_GRID,
	APP_MESH_COUNT
};


enum App_VidStreamName
{
	APP_VIDSTREAM1,
	APP_VIDSTREAM2,
	APP_VIDSTREAM_COUNT
};


enum App_Vid2TexName
{
	APP_VID2TEX_1,
	APP_VID2TEX_2,
	APP_VID2TEX_COUNT
};


struct RenderTransform
{
	float x;
	float y;
	float dx;
	float dy;
};


struct RenderBox
{
	float x;
	float y;
	float w;
	float h;
};


struct RenderColor
{
	float r;
	float g;
	float b;
	float a;
};



//Relationship between
//OpenGL texture (tex),
//OpenGL fragment custom color (col1)
//OpenGL vertex transformation (transform)
//OpenGL mesh (ma)
struct RenderCommand
{
	size_t i_tex;
	size_t i_col1;
	size_t i_transform;
	size_t i_ma;
};


//Relationship between view box and clear color.
struct BoxCommand
{
	size_t i_box;
	size_t i_colr;
};


//Relationship between the OpenCV capturing objects and OpenGL textures.
//This can be used to copy a video frame from (cap) to (tex).
struct Vid2Tex
{
	size_t cap;
	size_t tex;
};



struct Assets
{
	//RenderBox (bv2) is normalized.
	//RenderBox (bv1) must be adapted to the window dimensions.
	struct RenderBox bv1 [APP_RENDERBOX_COUNT];
	struct RenderBox bv2 [APP_RENDERBOX_COUNT];
	struct RenderColor cv [APP_RENDERCOLOR_COUNT];
	struct RenderTransform tv [APP_TRANSFORM_COUNT];
	
	//OpenGL program. Only use one for now.
	GLuint program [1];
	char const * programs_files [1];
	
	//OpenGL buffer objects.
	GLuint buf [2];
	
	//OpenGL texture objects.
	GLuint tex [APP_TEX_COUNT] = {0};
	
	GLuint vao [2];
	GLint uniform [2];
	
	
	float * values1 [APP_VIDSTREAM_COUNT];
	size_t values1_n [APP_VIDSTREAM_COUNT];
	
	//Video player stuff.
	cv::VideoCapture cap [APP_VIDSTREAM_COUNT];
	GLuint cap_tex       [APP_VIDSTREAM_COUNT];
	uint32_t video_position;
	uint32_t video_position_max;
	
	struct Vid2Tex vid2tex [APP_VID2TEX_COUNT];
	
	//Array of meshes.
	struct Mesh ma [APP_MESH_COUNT];
};


uint32_t cap_min_frame_count (size_t n, cv::VideoCapture cap [])
{
	double v = DBL_MAX;
	for (size_t i = 0; i < n; ++i)
	{
		double c = cap [i].get (CV_CAP_PROP_FRAME_COUNT);
		if (c < v)
		{
			v = c;
		}
	}
	ASSERT (v != DBL_MAX);
	return (uint32_t) v;
}


void mesh_add_grid (struct Mesh m [], GLint * vfirst, GLuint vbo, GLenum primitive)
{
	size_t const dim = 4;
	size_t const vcount = 1000;
	GLenum const target = GL_ARRAY_BUFFER;
	float * v = (float *) mesh_add_glMapBufferRange (m, vfirst, vbo, vcount, dim, primitive, target);
	struct Grid grid;
	vf32_setl (grid.p, 4, -1.0, -1.0, 0.0, 0.0);
	vf32_setl (grid.d1, 4, 0.1, 0.0, 0.0, 0.0);
	vf32_setl (grid.d2, 4, 0.0, 0.1, 0.0, 0.0);
	gen_grid (vcount, v, &grid);
	glUnmapBuffer (target);
}


void render_box_update (size_t n, struct RenderBox des [], struct RenderBox src [], float w, float h)
{
	for (size_t i = 0; i < n; ++ i)
	{
		des [i].x = src [i].x * w;
		des [i].y = src [i].y * h;
		des [i].w = src [i].w * w;
		des [i].h = src [i].h * h;
	}
}


void asset_init (struct Assets * as, uint32_t * flags)
{
	memset (&as->ma, 0, sizeof (struct Mesh) * APP_MESH_COUNT);
	
	glDebugMessageCallback (glDebugOutput, 0);
	glDebugMessageControl (GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	//glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	glEnable (GL_SCISSOR_TEST);
	//glEnable (GL_STENCIL_TEST);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_DEBUG_OUTPUT);
	glEnable (GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glClearColor (0.1f, 0.1f, 0.2f, 0.0f);
	glPointSize (4.0f);
	
	glGenBuffers (COUNTOF (as->buf), as->buf);
	glGenTextures (APP_TEX_COUNT - 1, as->tex + 1);
	glGenVertexArrays (COUNTOF (as->vao), as->vao);
	
	as->programs_files [APP_SHADER_XYUV] = APP_SHADER_FILES_XYUV;
	gl_programs_from_filenames (COUNTOF (as->program), as->program, as->programs_files);
	gl_program_debug (COUNTOF (as->program), as->program);
	glUseProgram (as->program [APP_SHADER_XYUV]);
		
	ASSERT (glIsProgram (as->program [APP_SHADER_XYUV]));
	ASSERT (COUNTOF (as->uniform) >= 2);
	as->uniform [APP_UNIFORM_TRANSFORM] = glGetUniformLocation (as->program [APP_SHADER_XYUV], "transform");
	as->uniform [APP_UNIFORM_COLOR] = glGetUniformLocation (as->program [APP_SHADER_XYUV], "u_color");
	ASSERT (as->uniform [APP_UNIFORM_TRANSFORM] >= 0);
	ASSERT (as->uniform [APP_UNIFORM_COLOR] >= 0);
	
	glBindVertexArray (as->vao [0]);
	glBindBuffer (GL_ARRAY_BUFFER, as->buf [0]);
	glVertexAttribPointer_byname (as->program [0], "xyuv", 4, GL_FLOAT, GL_FALSE, sizeof (float) * 4, 0);
	glBufferData (GL_ARRAY_BUFFER, APP_BUF_SIZE, 0, GL_DYNAMIC_DRAW);
	


	
	{
	GLint vfirst = 0;
	mesh_add_square (as->ma + APP_MESH_SQUARE, &vfirst, as->buf [0]);
	mesh_add_line (as->ma + APP_MESH_VLINE, &vfirst, as->buf [0], 0.0f, -1.0f, 0.0f, 1.0f);
	mesh_add_plot (as->ma + APP_MESH_PLOT1, &vfirst, as->buf [0], as->values1_n [0], as->values1 [0], GL_LINE_STRIP);
	mesh_add_plot (as->ma + APP_MESH_PLOTP1, &vfirst, as->buf [0], as->values1_n [0], as->values1 [0], GL_POINTS);
	mesh_add_plot (as->ma + APP_MESH_PLOT2, &vfirst, as->buf [0], as->values1_n [1], as->values1 [1], GL_LINE_STRIP);
	mesh_add_plot (as->ma + APP_MESH_PLOTP2, &vfirst, as->buf [0], as->values1_n [1], as->values1 [1], GL_POINTS);
	
	mesh_add_grid (as->ma + APP_MESH_GRID, &vfirst, as->buf [0], GL_LINES);
	}
	
	

	
	as->video_position = 0;
	as->video_position_max = cap_min_frame_count (APP_VIDSTREAM_COUNT, as->cap);
	

	
	{//Render boxes
	as->bv2 [APP_RENDERBOX_100].x = 0.0;
	as->bv2 [APP_RENDERBOX_100].y = 0.0;
	as->bv2 [APP_RENDERBOX_100].w = 1.0;
	as->bv2 [APP_RENDERBOX_100].h = 1.0;
	
	as->bv2 [APP_RENDERBOX_1].x = 0.1;
	as->bv2 [APP_RENDERBOX_1].y = 0.1;
	as->bv2 [APP_RENDERBOX_1].w = 0.3;
	as->bv2 [APP_RENDERBOX_1].h = 0.3;
	
	as->bv2 [APP_RENDERBOX_VID1].x = 0.0;
	as->bv2 [APP_RENDERBOX_VID1].y = 0.5;
	as->bv2 [APP_RENDERBOX_VID1].w = 0.5;
	as->bv2 [APP_RENDERBOX_VID1].h = 0.5;
	
	as->bv2 [APP_RENDERBOX_VID2].x = 0.5;
	as->bv2 [APP_RENDERBOX_VID2].y = 0.5;
	as->bv2 [APP_RENDERBOX_VID2].w = 0.5;
	as->bv2 [APP_RENDERBOX_VID2].h = 0.5;
	
	as->bv2 [APP_RENDERBOX_PLOT1].x = 0.0;
	as->bv2 [APP_RENDERBOX_PLOT1].y = 0.0;
	as->bv2 [APP_RENDERBOX_PLOT1].w = 0.499;
	as->bv2 [APP_RENDERBOX_PLOT1].h = 0.5;
	
	as->bv2 [APP_RENDERBOX_PLOT2].x = 0.501;
	as->bv2 [APP_RENDERBOX_PLOT2].y = 0.0;
	as->bv2 [APP_RENDERBOX_PLOT2].w = 0.499;
	as->bv2 [APP_RENDERBOX_PLOT2].h = 0.5;
	}
	
	{//Render colors
	as->cv [APP_RENDERCOLOR_ZERO].r = 0.0;
	as->cv [APP_RENDERCOLOR_ZERO].g = 0.0;
	as->cv [APP_RENDERCOLOR_ZERO].b = 0.0;
	as->cv [APP_RENDERCOLOR_ZERO].a = 0.0;
	
	as->cv [APP_RENDERCOLOR_1].r = 1.0;
	as->cv [APP_RENDERCOLOR_1].g = 0.5;
	as->cv [APP_RENDERCOLOR_1].b = 0.0;
	as->cv [APP_RENDERCOLOR_1].a = 0.0;
	
	as->cv [APP_RENDERCOLOR_2].r = 0.1;
	as->cv [APP_RENDERCOLOR_2].g = 0.5;
	as->cv [APP_RENDERCOLOR_2].b = 0.1;
	as->cv [APP_RENDERCOLOR_2].a = 0.0;
	
	as->cv [APP_RENDERCOLOR_3].r = 0.5;
	as->cv [APP_RENDERCOLOR_3].g = 0.1;
	as->cv [APP_RENDERCOLOR_3].b = 0.1;
	as->cv [APP_RENDERCOLOR_3].a = 0.0;
	
	as->cv [APP_RENDERCOLOR_GRAY].r = 0.8;
	as->cv [APP_RENDERCOLOR_GRAY].g = 0.8;
	as->cv [APP_RENDERCOLOR_GRAY].b = 0.8;
	as->cv [APP_RENDERCOLOR_GRAY].a = 0.0;
	
	as->cv [APP_RENDERCOLOR_WHITE].r = 0.9;
	as->cv [APP_RENDERCOLOR_WHITE].g = 0.9;
	as->cv [APP_RENDERCOLOR_WHITE].b = 0.9;
	as->cv [APP_RENDERCOLOR_WHITE].a = 0.0;
	
	as->cv [APP_RENDERCOLOR_BLUE].r = 0.2;
	as->cv [APP_RENDERCOLOR_BLUE].g = 0.2;
	as->cv [APP_RENDERCOLOR_BLUE].b = 0.8;
	as->cv [APP_RENDERCOLOR_BLUE].a = 0.0;
	}
	
	{//Transformations
	as->tv [APP_TRANSFORM_ZERO].x = 0.0;
	as->tv [APP_TRANSFORM_ZERO].y = 0.0;
	as->tv [APP_TRANSFORM_ZERO].dx = 1.0;
	as->tv [APP_TRANSFORM_ZERO].dy = 1.0;
	
	as->tv [APP_TRANSFORM_PLOT].x = 0.0;
	as->tv [APP_TRANSFORM_PLOT].y = 0.0;
	as->tv [APP_TRANSFORM_PLOT].dx = 1.0;
	as->tv [APP_TRANSFORM_PLOT].dy = 1.0;
	}
	
	{//Video to texture relationship
	as->vid2tex [APP_VID2TEX_1].cap = APP_VIDSTREAM1;
	as->vid2tex [APP_VID2TEX_1].tex = APP_TEX_VID1;
	
	as->vid2tex [APP_VID2TEX_2].cap = APP_VIDSTREAM2;
	as->vid2tex [APP_VID2TEX_2].tex = APP_TEX_VID2;
	}
	
	render_box_update (APP_RENDERBOX_COUNT, as->bv1, as->bv2, APP_WINDOW_WIDTH, APP_WINDOW_HEIGHT);
}


void cap_forward (size_t n, cv::VideoCapture cap [], int32_t amount)
{
	for (size_t i = 0; i < n; ++ i)
	{
		double f = cap [i].get (CV_CAP_PROP_POS_FRAMES);
		cap [i].set (CV_CAP_PROP_POS_FRAMES, f + amount);
	}
}


void cap_set1 (size_t n, cv::VideoCapture cap [], int32_t pos)
{
	for (size_t i = 0; i < n; ++ i)
	{
		cap [i].set (CV_CAP_PROP_POS_FRAMES, (double)pos);
		TRACE_F ("%i", (int)pos);
	}
}


void cap_grab (size_t n, cv::VideoCapture cap [])
{
	for (size_t i = 0; i < n; ++ i)
	{
		cap [i].grab ();
	}
}


void update_cap_tex (size_t n, struct Vid2Tex v2t [], struct cv::VideoCapture cap [], GLuint tex [])
{
	for (size_t i = 0; i < n; ++ i)
	{
		cv::Mat m;
		cap [v2t [i].cap].retrieve (m);
		gltex_update_from_cvmat (tex [v2t [i].tex], m);
	}
}


void handle_winresize (struct Assets * as, int32_t w, int32_t h)
{
	TRACE_F ("%i %i", (int)w, (int)h);
	render_box_update (APP_RENDERBOX_COUNT, as->bv1, as->bv2, (float)w, (float)h);
}





