#pragma once

//Common simple c functions
#include "debug.h"
#include "avgl.h"
#include "glwrap.h"
#include "xxav.h"
#include "v.h"
#include "v4.h"
#include "gen.h"

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H 


#define APP_WINDOW_WIDTH 1024
#define APP_WINDOW_HEIGHT 768
#define APP_TITLE "Video and plot"


#define APP_QUIT       0x0001
#define APP_ERROR      0x0002
#define APP_VIDEO_END  0x0004
#define APP_MOUSE_DRAG 0x0008
#define APP_NEXT       0x0010
#define APP_PREV       0x0020
#define APP_ARBI       0x0040
#define APP_PLAY       0x0080
#define APP_VID0ENB    0x0100
#define APP_VID1ENB    0x0200


void app_init (uint32_t * flags)
{
	{
		int r = SDL_Init (SDL_INIT_VIDEO);
		ASSERT_F (r == 0, "SDL_Init %i\n", SDL_GetError ());
	}



	// Initialize the lavf/lavc library and register all the formats, codecs and protocols.
	// http://ffmpeg.org/doxygen/trunk/group__lavf__core.html#ga917265caec45ef5a0646356ed1a507e3
	av_register_all ();
	//avcodec_register_all();
	*flags = 0;
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


//first: Specifies the starting index in the enabled arrays.
//count: Specifies the number of indices to be rendered.
void app_draw 
(
	uint32_t n, 
	GLenum const primitive [], 
	uint32_t first [], 
	uint32_t count []
)
{
	ASSERT (primitive);
	ASSERT (first);
	ASSERT (count);
	while (n--)
	{
		glDrawArrays (primitive [n], (GLint)first [n], (GLsizei)count [n]);
	}
}


void * app_map_glbuffer 
(
	GLuint vbo,
	uint32_t vfirst, 
	uint32_t vcapacity
)
{
	
	ASSERT (glIsBuffer (vbo));
	GLenum const target = GL_ARRAY_BUFFER;
	GLbitfield const access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
	uint32_t const vdim = 4;
	uint32_t const vsize = sizeof (float) * vdim;
	GLintptr const offset = vfirst * vsize;
	GLsizeiptr const length = vcapacity * vsize;
	TRACE_F ("offset %i", offset);
	TRACE_F ("length %i", length);
	glBindBuffer (target, vbo);
	void * data = glMapBufferRange (target, offset, length, access);
	GL_CHECK_ERROR;
	return data;
}


void app_unmap_glbuffer (GLuint vbo)
{
	GLenum const target = GL_ARRAY_BUFFER;
	glBindBuffer (target, vbo);
	glUnmapBuffer (target);
	GL_CHECK_ERROR;
}


void update_square_tex
(
	GLuint vbo, 
	uint32_t voffset, 
	uint32_t n,
	float layer []
)
{
	ASSERT (glIsBuffer (vbo));
	float * v = app_map_glbuffer (vbo, voffset, n*6);
	while (n--)
	{
		uint32_t const dim = 4;
		uint32_t const vcount = 6;
		gen_square_tex (v, layer [n]);
		v += dim * vcount;
	}
	app_unmap_glbuffer (vbo);
}


void update_square_pos 
(
	GLuint vbo, 
	uint32_t vfirst, 
	uint32_t vcapacity,
	float x,
	float y,
	float w,
	float h
)
{
	ASSERT (glIsBuffer (vbo));
	float * v = app_map_glbuffer (vbo, vfirst, vcapacity);
	gen_square_pos (v, x, y, w, h);
	app_unmap_glbuffer (vbo);
}


void update_pos_gridcell 
(
	GLuint vbo, 
	uint32_t vfirst, 
	uint32_t vcapacity,
	uint32_t x,
	uint32_t y,
	uint32_t w,
	uint32_t h,
	uint32_t i
)
{
	ASSERT (glIsBuffer (vbo));
	float * v = app_map_glbuffer (vbo, vfirst + 6 * i, vcapacity);
	gen_square_pos_gridcell (v, x, y, w, h);
	app_unmap_glbuffer (vbo);
}


void update_pos_line_xy 
(
	GLuint vbo, 
	uint32_t vfirst, 
	uint32_t vcapacity,
	float x0,
	float y0,
	float x1,
	float y1
)
{
	ASSERT (glIsBuffer (vbo));
	float * v = app_map_glbuffer (vbo, vfirst, vcapacity);
	gen_pos_line_xy (v, x0, y0, x1, y1);
	app_unmap_glbuffer (vbo);
}


void update_col_random
(
	GLuint vbo,
	uint32_t vfirst, 
	uint32_t vcapacity
)
{
	ASSERT (glIsBuffer (vbo));
	float * v = app_map_glbuffer (vbo, vfirst, vcapacity);
	v4f32_repeat 
	(
		vcapacity, 
		v, 
		(float)rand () / (float)RAND_MAX,
		(float)rand () / (float)RAND_MAX,
		(float)rand () / (float)RAND_MAX,
		1.0
	);
	app_unmap_glbuffer (vbo);
}


void update_repeat4
(
	GLuint vbo,
	uint32_t vfirst, 
	uint32_t vcapacity,
	float x0,
	float x1,
	float x2,
	float x3
)
{
	ASSERT (glIsBuffer (vbo));
	float * v = app_map_glbuffer (vbo, vfirst, vcapacity);
	v4f32_repeat (vcapacity, v, x0, x1, x2, x3);
	app_unmap_glbuffer (vbo);
}


void update_grid 
(
	GLuint vbo, 
	uint32_t vfirst, 
	uint32_t vcapacity,
	uint32_t r, 
	uint32_t c, 
	float x, 
	float y,
	float w,
	float h
)
{
	float * v = app_map_glbuffer (vbo, vfirst, vcapacity);
	gen_grid (v, r, c, x, y, w, h);
	app_unmap_glbuffer (vbo);
}


void update_randcurve 
(
	GLuint vbo, 
	uint32_t vfirst, 
	uint32_t vcapacity,
	float ymin,
	float ymax,
	float x,
	float w
)
{
	float * v = app_map_glbuffer (vbo, vfirst, vcapacity);
	srand (1);
	gen_randcurve (v, vcapacity, ymin, ymax, x, w);
	app_unmap_glbuffer (vbo);
}


void update_curve 
(
	GLuint vbo, 
	uint32_t vfirst, 
	uint32_t * vlength,
	char const * filename,
	float ymin,
	float ymax,
	float x,
	float w
)
{
	FILE * f = fopen (filename, "r");
	float * v = app_map_glbuffer (vbo, vfirst, *vlength);
	gen_curve (v, vlength, f, ymin, ymax, x, w);
	app_unmap_glbuffer (vbo);
	fclose (f);
}


void gpu_setup_vertex1 (GLuint vbo [3], uint32_t vn)
{
	GLenum const target [] = {GL_ARRAY_BUFFER, GL_ARRAY_BUFFER, GL_ARRAY_BUFFER};
	uint32_t const index [] = {0, 1, 2};
	GLenum const type [] = {GL_FLOAT, GL_FLOAT, GL_FLOAT};
	GLboolean const normalized [] = {GL_FALSE, GL_FALSE, GL_FALSE};
	uint32_t const offset [] = {0, 0, 0};
	uint32_t const dim [] = {4, 4, 4};
	uint32_t const stride [] = {sizeof (float) * dim [0], sizeof (float) * dim [1], sizeof (float) * dim [2]};
	GLbitfield const flags [] = {GL_MAP_WRITE_BIT, GL_MAP_WRITE_BIT, GL_MAP_WRITE_BIT};
	
	xxgl_layout 
	(
		3,
		index,
		dim,
		type,
		normalized,
		stride,
		offset,
		target,
		vbo
	);
	
	xxgl_allocate 
	(
		3,
		target,
		vbo,
		stride,
		NULL,
		flags,
		vn
	);
}


void setup_test_texture (GLuint tex)
{
	GLsizei const width = 2;
	GLsizei const height = 2;
	GLsizei const layerCount = 2;
	GLsizei const mipLevelCount = 1;
	// Read you texels here. In the current example, we have 2*2*2 = 8 texels, with each texel being 4 GLubytes.
	GLubyte texels[32] = 
	{
		 // Texels for first image.
		 0,   0,   0,   255,
		 255, 0,   0,   255,
		 0,   255, 0,   255,
		 0,   0,   255, 255,
		 // Texels for second image.
		 255, 255, 255, 255,
		 255, 255,   0, 255,
		 0,   255, 255, 255,
		 255, 0,   255, 255,
	};
	//glActiveTexture (GL_TEXTURE0 + 1);
	glBindTexture (GL_TEXTURE_2D_ARRAY, tex);
	// Allocate the storage.
	glTexStorage3D (GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGBA8, width, height, layerCount);
	// Upload pixel data.
	// The first 0 refers to the mipmap level (level 0, since there's only 1)
	// The following 2 zeroes refers to the x and y offsets in case you only want to specify a subrectangle.
	// The final 0 refers to the layer index offset (we start from index 0 and have 2 levels).
	// Altogether you can specify a 3D box subset of the overall texture, but only one mip level at a time.
	glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layerCount, GL_RGBA, GL_UNSIGNED_BYTE, texels);
	// Always set reasonable texture parameters
	glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


void gen4x6_square_tex1 (float v [24], float w, float h, float l)
{
	float v0 [24] =
	{
		0.0f, 0.0f, l, 0.0f,
		0.0f, h   , l, 0.0f,
		w   , h   , l, 0.0f,
		0.0f, 0.0f, l, 0.0f,
		w   , h   , l, 0.0f,
		w   , 0.0f, l, 0.0f       
	};
	memcpy (v, v0, sizeof (v0));
}



struct character_info
{
	float ax; // advance.x
	float ay; // advance.y
	float bw; // bitmap.width;
	float bh; // bitmap.rows;
	float bl; // bitmap_left;
	float bt; // bitmap_top;
	float tx; // x offset of glyph in texture coordinates
};


void setup_font 
(
	GLuint tex, 
	FT_Face face, 
	struct character_info ci []
)
{
	GLenum const target = GL_TEXTURE_2D_ARRAY;
	GLsizei const width = 50;
	GLsizei const height = 50;
	GLsizei const layerCount = 128;
	GLsizei const mipLevelCount = 1;
	GLenum const internalformat = GL_R8;
	glBindTexture (target, tex);
	glTexStorage3D (target, mipLevelCount, internalformat, width, height, layerCount);
	GL_CHECK_ERROR;
	
	//IMPORTANT. Disable byte-alignment restriction.
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	
	for (uint8_t c = 0; c < 128; c++)
	{
		{
			int r = FT_Load_Char (face, c, FT_LOAD_RENDER);
			ASSERT_F (r == 0, "ERROR::FREETYTPE: Failed to load Glyph %i", r);
			if (r) {continue;}
		}
		
		ci [c].ax = face->glyph->advance.x >> 6;
		ci [c].ay = face->glyph->advance.y >> 6;
		ci [c].bl = face->glyph->bitmap_left;
		ci [c].bt = face->glyph->bitmap_top;
		ci [c].bw = face->glyph->bitmap.width;
		ci [c].bh = face->glyph->bitmap.rows;
		
		GLint xoffset = 0;
		GLint yoffset = 0;
		GLint zoffset = c;
		//TRACE_F ("glyph %i %i", face->glyph->bitmap.width, face->glyph->bitmap.rows);
		glTexSubImage3D 
		(
			target, 
			0, 
			xoffset, 
			yoffset, 
			zoffset, 
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			1, 
			GL_RED,
			GL_UNSIGNED_BYTE, 
			face->glyph->bitmap.buffer
		);
		GL_CHECK_ERROR;
	}
	
	// Always set reasonable texture parameters
	glTexParameteri (target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri (target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri (target, GL_TEXTURE_MAX_LEVEL, 0);
	//Single channel textures stores pixel values in the red channel.
	//It is better to store it in the alpha channel because 
	//then we can choose the color from the vertex color.
	//https://www.g-truc.net/post-0734.html
	//https://www.khronos.org/opengl/wiki/GLAPI/glTexParameter
	glTexParameteri (target, GL_TEXTURE_SWIZZLE_A, GL_RED);
	glTexParameteri (target, GL_TEXTURE_SWIZZLE_R, GL_ZERO);
}



enum PD_VBO
{
	PD_VBO_POS,
	PD_VBO_COL,
	PD_VBO_TEX,
	PD_VBO_N
};



struct ProDraw
{
	uint32_t n;
	uint32_t * offset;
	uint32_t * length;
	uint32_t * capacity;
	GLenum * primitive;
	
	
	GLuint vao [1];
	//pos_xyzw, col_rgba, tex_uvl0
	GLuint vbo [PD_VBO_N];
};


uint32_t pd_cap (struct ProDraw * pd)
{
	if (pd->n == 0) {return 0;}
	uint32_t last = pd->n - 1;
	return pd->offset [last] + pd->capacity [last];
}


void pd_init (struct ProDraw * pd)
{
	glGenBuffers      (PD_VBO_N, pd->vbo);
	glGenVertexArrays (1, pd->vao);
	glBindVertexArray (pd->vao [0]);
	
	pd->offset = calloc (pd->n, sizeof (uint32_t));
	pd->length = calloc (pd->n, sizeof (uint32_t));
	pd->capacity = calloc (pd->n, sizeof (uint32_t));
	pd->primitive = calloc (pd->n, sizeof (GLenum));
}


void pd_draw (struct ProDraw * pd, uint32_t i)
{
	ASSERT (pd->primitive + i);
	ASSERT (pd->offset + i);
	ASSERT (pd->length + i);
	glDrawArrays (pd->primitive [i], (GLint)pd->offset [i], (GLsizei)pd->length [i]);
}
