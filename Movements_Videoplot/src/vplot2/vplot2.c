/*
https://gist.github.com/rcolinray/7552384
http://hasanaga.info/tag/ffmpeg-libavcodec-sws_scale-example/
http://dranger.com/ffmpeg/tutorial07.html
*/

//Common
#include "SDLGL.h"
#include "glwrap.h"
#include "debug.h"
#include "debug_gl.h"
#include "global.h"
#include "shader.h"
#include "v.h"
#include "c.h"
#include "av_playarray.h"
#include "gl_drawarray.h"
#include "camera.h"

#include "vplot2.h"

#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H 




enum tex_index
{
	TEX_VID,
	TEX_GLYPH,
	TEX_TEST,
	TEX_TEST1,
	TEX_TEST2,
	TEX_N
};


enum draw_index
{
	DI_VID0,
	DI_VID1,
	DI_TLINE,
	DI_GRID,
	DI_RCURVE,
	DI_RCURVEP,
	DI_TEXT0,
	DI_TEXT1,
	DI_N
};


void app_update_viewrect (struct v4u32_xywh rect [5], SDL_Window * window)
{
	int w;
	int h;
	int margin = 5;
	SDL_GetWindowSize (window, &w, &h);
	//w = abs (w);
	//h = abs (h);
	//TRACE_F ("%i", w);
	
	w = MAX (margin*2, w);
	h = MAX (margin*2, h);
	
	rect [0].x = 0;
	rect [0].y = 0;
	rect [0].w = w;
	rect [0].h = h;
	
	rect [1].x = 0;
	rect [1].y = 0;
	rect [1].w = w / 2 - margin;
	rect [1].h = h / 2 - margin;
	
	rect [2].x = w / 2 + margin;
	rect [2].y = 0;
	rect [2].w = w / 2 - margin;
	rect [2].h = h / 2 - margin;
	
	rect [3].x = 0;
	rect [3].y = h / 2 + margin;
	rect [3].w = w / 2 - margin;
	rect [3].h = h / 2 - margin;
	
	rect [4].x = w / 2 + margin;
	rect [4].y = h / 2 + margin;
	rect [4].w = w / 2 - margin;
	rect [4].h = h / 2 - margin;
}


struct gl_view
{
	struct v4u32_xywh rect [5];
	struct v4f32_RGBA color [5];
};


void app_begin_view (struct gl_view * view, uint32_t i)
{
	glViewport (view->rect [i].x, view->rect [i].y, view->rect [i].w, view->rect [i].h);
	glScissor (view->rect [i].x, view->rect [i].y, view->rect [i].w, view->rect [i].h);
	glClearColor (view->color [i].r, view->color [i].g, view->color [i].b, view->color [i].a);
	glClear (GL_COLOR_BUFFER_BIT);
}


float * glMapBufferRange_4fv (uint32_t offset, uint32_t length)
{
	GLenum const target = GL_ARRAY_BUFFER;
	GLbitfield const access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT;
	uint32_t const vdim = 4;
	uint32_t const vsize = sizeof (float) * vdim;
	GLintptr const offset8 = offset * vsize;
	GLsizeiptr const length8 = length * vsize;
	float * buffer = glMapBufferRange (target, offset8, length8, access);
	ASSERT (buffer);
	return buffer;
}


void RenderText 
(
	struct ProDraw * draw,
	uint32_t drawi,
	char const * text, 
	struct character_info ci [128],
	float x, 
	float y, 
	float scale
)
{
	//Vertices iterator
	float * v;
	
	//Text iterator
	char const * c;
	
	draw->length [drawi] = MIN (draw->capacity [drawi], strlen (text)*6);
	
	//Generate the position for each character
	//Map all or part of a buffer object's data store into the client's address space
	glBindBuffer (GL_ARRAY_BUFFER, draw->vbo [PD_VBO_POS]);
	v = glMapBufferRange_4fv (draw->offset [drawi], draw->length [drawi]);
	c = text;
	while (*c)
	{
		//Each character is also a index to a table. TODO: Explain more.
		uint8_t i = *c;
		//Calculate the character xywh position 
		//depending on this character bearing configuration.
		float xx = x + ci [i].bl * scale;
		float yy = y - (ci [i].bh - ci [i].bt) * scale;
		float w = ci [i].bw * scale;
		float h = ci [i].bh * scale;
		//TRACE_F ("x %f y %f", (double)xx, (double)yy);
		gen_square_pos (v, xx, yy, w, h);
		//Goto next square vertex memory slot.
		v += 24;
		//Set next character x coordinate after current character, 
		//reading from left to right.
		x += ci [i].ax * scale;
		//Goto next character
		c ++;
	}
	glUnmapBuffer (GL_ARRAY_BUFFER);
	//TRACE ("");
	
	
	/*
	glBindBuffer (GL_ARRAY_BUFFER, vbo [PD_VBO_COL]);
	v = glMapBufferRange_4fv (offset, length);
	srand (1);
	v4f32_repeat_random (length, v);
	v4f32_repeat_channel (length, v, 3, 1.0f);
	glUnmapBuffer (GL_ARRAY_BUFFER);
	*/
	
	//Generate texture coordinate for each character.
	//The texture coordinate structure is (x,y,layer).
	//The layer is which glyph to draw.
	//Map all or part of a buffer object's data store into the client's address space
	glBindBuffer (GL_ARRAY_BUFFER, draw->vbo [PD_VBO_TEX]);
	v = glMapBufferRange_4fv (draw->offset [drawi], draw->length [drawi]);
	c = text;
	while (*c)
	{
		//Each character is also a index to a table. TODO: Explain more.
		uint8_t i = *c;
		gen4x6_square_tex1 (v, ci [i].bw / 50.0f, ci [i].bh / 50.0f, i);
		//Goto next square vertex memory slot.
		v += 24;
		//Goto next character
		c ++;
	}
	//gen4x6_square_tex (strlen (text), v, (uint8_t const *)text);
	glUnmapBuffer (GL_ARRAY_BUFFER);
	
	
	//glDrawArrays (GL_TRIANGLES, offset, strlen (text)*6);
	//glDrawArrays (GL_LINES, offset, 6*5);
}








int main (int argc, char *argv[])
{
	errno = 0;
	//ASSERT_F (argc == 3, "Number of arguments given: %i. Requires two argments of videos filename!", argc);
	
	
	uint32_t flags = 0;
	app_init (&flags);
	
	FT_Library ft;
	FT_Face face;
	{
		int r = FT_Init_FreeType (&ft);
		ASSERT_F (r == 0, "ERROR::FREETYPE: Could not init FreeType Library %i", r);
	}
	{
		int r = FT_New_Face(ft, "/usr/share/fonts/truetype/ubuntu/UbuntuMono-B.ttf", 0, &face);
		ASSERT_F (r == 0, "ERROR::FREETYPE: Failed to load font %i", r);
	}
	FT_Set_Pixel_Sizes (face, 0, 48);
	
	SDL_Window * window;
	SDL_GLContext glcontext;
	const Uint8 * keyboard;
	SDL_Event event;
	
	struct VideoPlayer vp;
	vp.n = 2;
	vp_init (&vp);
	ASSERT (argc >= 3);
	vp.url [0] = argv [1];
	vp.url [1] = argv [2];
	xxav_open 
	(
		vp.n,
		vp.fctx,
		vp.cctx,
		vp.wctx,
		vp.frame0,
		vp.frame1,
		vp.ivid,
		vp.pts,
		vp.url
	);
	
	
	window = app_create_window ();
	glcontext = SDL_GL_CreateContext_CC (window);
	keyboard = SDL_GetKeyboardState (NULL);
	
	{
		char title [1024];
		snprintf (title, 1024, "%s        %s", vp.url [0], vp.url [1]);
		SDL_SetWindowTitle (window, title);
	}
	
	
	{//OpenGL settings
		GLint texture_units;
		glGetIntegerv (GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
		TRACE_F ("texture_units %i", texture_units);
		
		glDebugMessageCallback (glDebugOutput, 0);
		glDebugMessageControl (GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
		//glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
		//glEnable (GL_STENCIL_TEST);
		//glEnable (GL_DEPTH_TEST);
		glEnable (GL_DEBUG_OUTPUT);
		glEnable (GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glEnable (GL_BLEND);
		glEnable (GL_SCISSOR_TEST);
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable (GL_ALPHA_TEST);
		glPointSize (4.0f);
		GL_CHECK_ERROR;
	}
	
	GLuint program = gl_program_from_filename ("src/vplot2/vplot2.glfs;src/vplot2/vplot2.glvs");
	gl_shader_debug1 (program);
	glUseProgram (program);
	
	
	struct gl_view view;
	app_update_viewrect (view.rect, window);
	vf32_setl ((float*)&view.color [0], 4, 0.0, 0.0, 0.0, 1.0);
	vf32_setl ((float*)&view.color [1], 4, 0.99, 0.99, 0.99, 1.0);
	vf32_setl ((float*)&view.color [2], 4, 0.99, 0.99, 0.99, 1.0);
	vf32_setl ((float*)&view.color [3], 4, 1.0, 1.0, 0.0, 1.0);
	vf32_setl ((float*)&view.color [4], 4, 1.0, 0.0, 1.0, 1.0);
	
	
	//ProDraw stores information about vertex indices.
	struct ProDraw pd;
	pd.n = DI_N;
	pd_init (&pd);
	pd.capacity  [DI_VID0]   = 6; //Square
	pd.length    [DI_VID0]   = 6;
	pd.primitive [DI_VID0]   = GL_TRIANGLES;
	pd.capacity  [DI_VID1]   = 6; //Square
	pd.length    [DI_VID1]   = 6;
	pd.primitive [DI_VID1]   = GL_TRIANGLES;
	pd.capacity  [DI_TLINE]  = 2; //Line
	pd.length    [DI_TLINE]  = 2;
	pd.primitive [DI_TLINE]  = GL_LINES;
	pd.capacity  [DI_GRID]   = 2*5*5; //Lines
	pd.length    [DI_GRID]   = 2*5*5;
	pd.primitive [DI_GRID]   = GL_LINES;
	pd.capacity  [DI_RCURVE] = 98; //Line strip
	pd.length    [DI_RCURVE] = 98;
	pd.primitive [DI_RCURVE] = GL_LINE_STRIP;
	pd.capacity  [DI_RCURVEP] = 98; //Line strip
	pd.length    [DI_RCURVEP] = 98;
	pd.primitive [DI_RCURVEP] = GL_POINTS;
	pd.capacity  [DI_TEXT0]  = 6*100; //Squares
	pd.length    [DI_TEXT0]  = 6*100; //Squares
	pd.primitive [DI_TEXT0]  = GL_TRIANGLES;
	pd.capacity  [DI_TEXT1]  = 6*100; //Squares
	pd.length    [DI_TEXT1]  = 6*100; //Squares
	pd.primitive [DI_TEXT1]  = GL_TRIANGLES;
	vu32_ladder (pd.n, pd.offset, pd.capacity);
	gpu_setup_vertex1 (pd.vbo, pd_cap (&pd));
	

	GLuint tex [TEX_N];
	glGenTextures (COUNTOF (tex), tex);
	
	
	//Setup texture storage
	struct character_info charinfo [128];
	setup_font (tex [TEX_GLYPH], face, charinfo);
	setup_test_texture (tex [TEX_TEST]);
	avgl_TextureArrayStorage (tex [TEX_VID], vp.n, vp.frame1);
	avgl_TextureArraySub (tex [TEX_VID], vp.n, vp.frame1);
	
	
	
	//Add the video vertices
	float layer0 [] = {0.0f};
	update_repeat4    (pd.vbo [PD_VBO_COL], pd.offset [DI_VID0], pd.capacity [DI_VID0], 0.0f, 0.0f, 0.0f, 1.0f);
	update_square_pos (pd.vbo [PD_VBO_POS], pd.offset [DI_VID0], pd.capacity [DI_VID0], -1.0f, -1.0f, 2.0f, 2.0f);
	update_square_tex (pd.vbo [PD_VBO_TEX], pd.offset [DI_VID0], 1, layer0);
	float layer1 [] = {1.0f};
	update_repeat4    (pd.vbo [PD_VBO_COL], pd.offset [DI_VID1], pd.capacity [DI_VID1], 0.0f, 0.0f, 0.0f, 1.0f);
	update_square_pos (pd.vbo [PD_VBO_POS], pd.offset [DI_VID1], pd.capacity [DI_VID1], -1.0f, -1.0f, 2.0f, 2.0f);
	update_square_tex (pd.vbo [PD_VBO_TEX], pd.offset [DI_VID1], 1, layer1);
	
	
	//Add the timeline vertices
	update_repeat4     (pd.vbo [PD_VBO_TEX], pd.offset [DI_TLINE], pd.capacity [DI_TLINE], 0.0f, 0.0f, -1.0f, 0.0f);
	update_repeat4     (pd.vbo [PD_VBO_COL], pd.offset [DI_TLINE], pd.capacity [DI_TLINE], 1.0f, 0.0f, 1.0f, 1.0f);
	update_pos_line_xy (pd.vbo [PD_VBO_POS], pd.offset [DI_TLINE], pd.capacity [DI_TLINE], 0.0f, -1.0f, 0.0f, 1.0f);
	
	//Add the grid vertices
	update_repeat4 (pd.vbo [PD_VBO_TEX], pd.offset [DI_GRID], pd.capacity [DI_GRID], 0.0f, 0.0f, -1.0f, 0.0f);
	update_repeat4 (pd.vbo [PD_VBO_COL], pd.offset [DI_GRID], pd.capacity [DI_GRID], 0.8f, 0.8f, 0.8f, 1.0f);
	update_grid 
	(
		pd.vbo [PD_VBO_POS], 
		pd.offset [DI_GRID], 
		pd.capacity [DI_GRID], 
		5, 5,
		-1.0f, -1.0f,
		 2.0f,  2.0f
	);
	

	
	//Add random curve vertices
	update_repeat4   (pd.vbo [PD_VBO_TEX], pd.offset [DI_RCURVE], pd.capacity [DI_RCURVE], 0.0f, 0.0f, -1.0f, 0.0f);
	update_repeat4   (pd.vbo [PD_VBO_COL], pd.offset [DI_RCURVE], pd.capacity [DI_RCURVE], 1.0f, 0.0f, 0.0f, 1.0f);
	//update_randcurve (pd.vbo [PD_VBO_POS], pd.offset [DI_RCURVE], pd.capacity [DI_RCURVE],-0.1f, 0.1f,-0.5f, 1.0f);
	update_repeat4   (pd.vbo [PD_VBO_TEX], pd.offset [DI_RCURVEP], pd.capacity [DI_RCURVEP], 0.0f, 0.0f, -1.0f, 0.0f);
	update_repeat4   (pd.vbo [PD_VBO_COL], pd.offset [DI_RCURVEP], pd.capacity [DI_RCURVEP], 0.7f, 0.3f, 0.3f, 1.0f);
	//update_randcurve (pd.vbo [PD_VBO_POS], pd.offset [DI_RCURVEP], pd.capacity [DI_RCURVEP],-0.1f, 0.1f,-0.5f, 1.0f);
	update_repeat4   (pd.vbo [PD_VBO_COL], pd.offset [DI_TEXT0], pd.capacity [DI_TEXT0], 0.0f, 1.0f, 0.0f, 0.0f);
	update_repeat4   (pd.vbo [PD_VBO_COL], pd.offset [DI_TEXT1], pd.capacity [DI_TEXT1], 0.0f, 1.0f, 0.0f, 0.0f);
	{
		ASSERT (argc >= 4)
		update_curve (pd.vbo [PD_VBO_POS], pd.offset [DI_RCURVE], pd.length + DI_RCURVE, argv [3], -0.1f, 0.1f, -0.5f, 1.0f);
		update_curve (pd.vbo [PD_VBO_POS], pd.offset [DI_RCURVEP], pd.length + DI_RCURVEP, argv [3], -0.1f, 0.1f, -0.5f, 1.0f);
	}
	

	{//glActiveTexture
		//Assign texture objects to texture units.
		glActiveTexture (GL_TEXTURE0 + 0);
		glBindTexture (GL_TEXTURE_2D_ARRAY, tex [0]);
		glActiveTexture (GL_TEXTURE0 + 1);
		glBindTexture (GL_TEXTURE_2D_ARRAY, tex [1]);
		glActiveTexture (GL_TEXTURE0 + 2);
		glBindTexture (GL_TEXTURE_2D_ARRAY, tex [2]);
		glActiveTexture (GL_TEXTURE0 + 3);
		glBindTexture (GL_TEXTURE_2D_ARRAY, tex [3]);
	}
	
	
	GLint utex = glGetUniformLocation (program, "tex");
	GLint u4 = glGetUniformLocation (program, "transform");
	ASSERT (u4 >= 0);
	ASSERT (utex >= 0);
	glUniform4f (u4, 0.0f, 0.0f, 1.0f, 1.0f);
	
	float px = 0.0f;
	float py = 0.0f;
	float dx = 1.0f;
	float dy = 1.0f;
	
	while (1)
	{
		if (flags & APP_QUIT) {break;}
		if (flags & APP_ERROR) {break;}
		
		while (SDL_PollEvent (&event)) {switch (event.type)
		{
			case SDL_QUIT:
			flags |= APP_QUIT;
			break;
			
			case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
				case SDL_WINDOWEVENT_RESIZED:
				app_update_viewrect (view.rect, window);
				break;
			}
			break;
				
			case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
				flags |= APP_QUIT;
				break;
				
				case SDLK_RIGHT:
				flags |= APP_NEXT;
				break;
				
				case SDLK_LEFT:
				flags |= APP_PREV;
				break;
				
				case SDLK_SPACE:
				flags ^= APP_PLAY;
				break;
				
				case SDLK_c:
				//update_col_random (vbo [PD_VBO_COL], pd.offset [DI_VID], pd.capacity [DI_VID]);
				//update_col_random (vbo [PD_VBO_COL], pd.offset [DI_TLINE], pd.capacity [DI_TLINE]);
				break;
				
				case SDLK_u:
				avgl_TextureArrayStorage (tex [TEX_VID], vp.n, vp.frame1);
				avgl_TextureArraySub (tex [TEX_VID], vp.n, vp.frame1);
				flags |= APP_VID0ENB;
				break;
				
			}
			break;
			
			case SDL_MOUSEBUTTONDOWN:
			//TRACE ("SDL_MOUSEBUTTONDOWN");
			flags |= APP_MOUSE_DRAG;
			break;
			
			case SDL_MOUSEBUTTONUP:
			//TRACE ("SDL_MOUSEBUTTONUP");
			flags &= ~APP_MOUSE_DRAG;
			flags |= APP_ARBI;
			break;
			
			case SDL_MOUSEMOTION:
			if (flags & APP_MOUSE_DRAG)
			{
				float x;
				float y;
				SDL_GetOpenGLMouseRel (window, &event, &x, &y);
				//TRACE_F ("%f %f", (double)x, (double)y);
				px += 2.0f * x / dx;
				py += 2.0f * y / dy;
			}
			break;
			
			case SDL_MOUSEWHEEL:
			dx += 0.1f * (float)event.wheel.y;
			dy += 0.1f * (float)event.wheel.y;
			break;
			
		}}
		
		
		if (flags & (APP_NEXT | APP_PLAY))
		{
			xxav_decode (vp.fctx [0], vp.cctx [0], vp.ivid [0], vp.frame0 [0], vp.pts + 0);
			xxav_decode (vp.fctx [1], vp.cctx [1], vp.ivid [1], vp.frame0 [1], vp.pts + 1);
		}
		
		
		if (flags & APP_ARBI)
		{
			int64_t ts = (double)vp.fctx [0]->duration * (double)(0.5f - px);
			ts = MAX (ts, 0);
			TRACE_F ("%10lli", ts);
			//TRACE_F ("ts %f", (double)10*AV_TIME_BASE);
			//In some videos it seek to allways to the first frame!!
			vp_seek (&vp, 0, ts);
			vp_seek (&vp, 1, ts);
			px = -xxav_normtime (vp.fctx [0], vp.ivid [0], vp.pts [0]) + 0.5f;
		}
		
		
		if (flags & (APP_NEXT | APP_PREV | APP_ARBI | APP_PLAY))
		{
			flags &= ~(APP_NEXT | APP_ARBI | APP_PREV);
			sws_scale
			(
				vp.wctx [0], 
				(const unsigned char * const*)vp.frame0 [0]->data, 
				vp.frame0 [0]->linesize, 
				0, 
				vp.frame0 [0]->height, 
				vp.frame1 [0]->data, 
				vp.frame1 [0]->linesize
			);
			sws_scale
			(
				vp.wctx [1], 
				(const unsigned char * const*)vp.frame0 [0]->data, 
				vp.frame0 [1]->linesize, 
				0, 
				vp.frame0 [1]->height, 
				vp.frame1 [1]->data, 
				vp.frame1 [1]->linesize
			);
			//Copy all frames to each texture.
			avgl_TextureArraySub (tex [TEX_VID], vp.n, vp.frame1);
			//TRACE_F ("t: %f", (double) xxav_normtime (vp.fctx [0], vp.ivid [0], vp.pts [0]));
			px = -xxav_normtime (vp.fctx [0], vp.ivid [0], vp.pts [0]) + 0.5f;
			//TRACE_F ("%10lli", vp.pts [0]);
			double s = xxav_dts2sec (vp.fctx [0]->streams [vp.ivid [0]], vp.pts [0]);
			//TRACE_F ("%f", s);
		}
		
		
		{//Update text i.e. frame number, time.
			char buf [40];
			snprintf 
			(
				buf, 
				40, 
				"%5lif %5.1fs", 
				xxav_dts2fnum (vp_getstream (&vp, 1), vp.pts [1]),
				xxav_dts2sec (vp_getstream (&vp, 1), vp.pts [1])
			);
			RenderText (&pd, DI_TEXT0, buf, charinfo, -1.0f, 0.9f, 0.003f);
			snprintf 
			(
				buf, 
				40, 
				"%5lif %5.1fs", 
				xxav_dts2fnum (vp_getstream (&vp, 0), vp.pts [0]),
				xxav_dts2sec (vp_getstream (&vp, 0), vp.pts [0])
			);
			RenderText (&pd, DI_TEXT1, buf, charinfo, -1.0f, 0.9f, 0.003f);
		}

		app_begin_view (&view, 0);
		
		glUniform4f (u4, px, py, dx, dy);
		app_begin_view (&view, 1);
		pd_draw (&pd, DI_RCURVE);
		pd_draw (&pd, DI_RCURVEP);
		glUniform4f (u4, 0.0f, 0.0f, 1.0f, 1.0f);
		pd_draw (&pd, DI_TLINE);
		pd_draw (&pd, DI_GRID);
		
		glUniform4f (u4, px, py, dx, dy);
		app_begin_view (&view, 2);
		pd_draw (&pd, DI_RCURVE);
		pd_draw (&pd, DI_RCURVEP);
		glUniform4f (u4, 0.0f, 0.0f, 1.0f, 1.0f);
		pd_draw (&pd, DI_TLINE);
		pd_draw (&pd, DI_GRID);
		
		glUniform4f (u4, 0.0f, 0.0f, 1.0f, 1.0f);
		app_begin_view (&view, 3);
		glUniform1i (utex, TEX_VID);
		pd_draw (&pd, DI_VID0);
		glUniform1i (utex, TEX_GLYPH);
		pd_draw (&pd, DI_TEXT0);
		
		app_begin_view (&view, 4);
		glUniform1i (utex, TEX_VID);
		pd_draw (&pd, DI_VID1);
		glUniform1i (utex, TEX_GLYPH);
		pd_draw (&pd, DI_TEXT1);
		
		SDL_GL_SwapWindow (window);
		SDL_Delay (10);
	}
	
	SDL_GL_DeleteContext (glcontext);
	
	return 0;
}











