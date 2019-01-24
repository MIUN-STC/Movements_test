#include "SDLGL.h"
#include "glwrap.h"
#include "debug.h"
#include "debug_gl.h"
#include "global.h"
#include "shader.h"
#include "v.h"
#include "mesharray.h"

#include "app.h"
#include "extra.h"
#include "grid.h"
#include "option.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>


void bcom_draw
(
	struct BoxCommand * bc, 
	struct RenderColor cv [],
	struct RenderBox bv []
)
{
	struct RenderColor * cr = cv + bc->i_colr;
	struct RenderBox * b = bv + bc->i_box;
	glViewport (b->x, b->y, b->w, b->h);
	glScissor  (b->x, b->y, b->w, b->h);
	glClearColor (cr->r, cr->g, cr->b, cr->a);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void rcom_draw 
(
	struct RenderCommand * rc, 
	struct RenderColor cv [],
	struct RenderBox bv [],
	struct RenderTransform tv [],
	struct Mesh * ma,
	GLuint tex [],
	GLint uniform []
)
{
	struct RenderColor * c1 = cv + rc->i_col1;
	struct RenderTransform * t = tv + rc->i_transform;
	glUniform4fv (uniform [APP_UNIFORM_TRANSFORM], 1, (float *)t);
	glUniform4fv (uniform [APP_UNIFORM_COLOR], 1, (float *)c1);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, tex [rc->i_tex]);
	mesh_draw_one (ma + rc->i_ma);
}


void draw_test
(
	struct RenderColor cv [],
	struct RenderBox bv [],
	struct RenderTransform tv [],
	struct Mesh ma [], 
	GLuint tex [], 
	GLint uniform []
)
{
	struct RenderCommand rc;
	struct BoxCommand bc [APP_RENDERBOX_COUNT];
	bc [APP_RENDERBOX_VID1].i_box = APP_RENDERBOX_VID1;
	bc [APP_RENDERBOX_VID1].i_colr = APP_RENDERCOLOR_ZERO;
	bc [APP_RENDERBOX_VID2].i_box = APP_RENDERBOX_VID2;
	bc [APP_RENDERBOX_VID2].i_colr = APP_RENDERCOLOR_ZERO;
	bc [APP_RENDERBOX_PLOT1].i_box = APP_RENDERBOX_PLOT1;
	bc [APP_RENDERBOX_PLOT1].i_colr = APP_RENDERCOLOR_WHITE;
	bc [APP_RENDERBOX_PLOT2].i_box = APP_RENDERBOX_PLOT2;
	bc [APP_RENDERBOX_PLOT2].i_colr = APP_RENDERCOLOR_WHITE;
	bc [APP_RENDERBOX_100].i_box = APP_RENDERBOX_100;
	bc [APP_RENDERBOX_100].i_colr = APP_RENDERCOLOR_ZERO;
	
	
	bcom_draw (bc + APP_RENDERBOX_100, cv, bv);
	
	
	bcom_draw (bc + APP_RENDERBOX_VID1, cv, bv);
	rc.i_col1 = APP_RENDERCOLOR_ZERO;
	rc.i_tex = APP_TEX_VID1;
	rc.i_ma = APP_MESH_SQUARE;
	rc.i_transform = 0;
	rcom_draw (&rc, cv, bv, tv, ma, tex, uniform);
	
	bcom_draw (bc + APP_RENDERBOX_VID2, cv, bv);
	rc.i_col1 = APP_RENDERCOLOR_ZERO;
	rc.i_tex = APP_TEX_VID2;
	rc.i_ma = APP_MESH_SQUARE;
	rc.i_transform = 0;
	rcom_draw (&rc, cv, bv, tv, ma, tex, uniform);
	




	bcom_draw (bc + APP_RENDERBOX_PLOT1, cv, bv);
	
	rc.i_col1 = APP_RENDERCOLOR_1;
	rc.i_tex = APP_TEX_NONE;
	rc.i_ma = APP_MESH_PLOT1;
	rc.i_transform = APP_TRANSFORM_PLOT;
	rcom_draw (&rc, cv, bv, tv, ma, tex, uniform);
	
	rc.i_col1 = APP_RENDERCOLOR_2;
	rc.i_tex = APP_TEX_NONE;
	rc.i_ma = APP_MESH_PLOTP1;
	rc.i_transform = APP_TRANSFORM_PLOT;
	rcom_draw (&rc, cv, bv, tv, ma, tex, uniform);
	
	rc.i_col1 = APP_RENDERCOLOR_BLUE;
	rc.i_tex = APP_TEX_NONE;
	rc.i_ma = APP_MESH_VLINE;
	rc.i_transform = 0;
	rcom_draw (&rc, cv, bv, tv, ma, tex, uniform);
	
	rc.i_col1 = APP_RENDERCOLOR_GRAY;
	rc.i_tex = APP_TEX_NONE;
	rc.i_ma = APP_MESH_GRID;
	rc.i_transform = 0;
	rcom_draw (&rc, cv, bv, tv, ma, tex, uniform);
	
	
	
	
	
	
	

	bcom_draw (bc + APP_RENDERBOX_PLOT2, cv, bv);
	
	rc.i_col1 = APP_RENDERCOLOR_1;
	rc.i_tex = APP_TEX_NONE;
	rc.i_ma = APP_MESH_PLOT2;
	rc.i_transform = APP_TRANSFORM_PLOT;
	rcom_draw (&rc, cv, bv, tv, ma, tex, uniform);
	
	rc.i_col1 = APP_RENDERCOLOR_2;
	rc.i_tex = APP_TEX_NONE;
	rc.i_ma = APP_MESH_PLOTP2;
	rc.i_transform = APP_TRANSFORM_PLOT;
	rcom_draw (&rc, cv, bv, tv, ma, tex, uniform);
	
	
	rc.i_col1 = APP_RENDERCOLOR_BLUE;
	rc.i_tex = APP_TEX_NONE;
	rc.i_ma = APP_MESH_VLINE;
	rc.i_transform = 0;
	rcom_draw (&rc, cv, bv, tv, ma, tex, uniform);
	
	rc.i_col1 = APP_RENDERCOLOR_GRAY;
	rc.i_tex = APP_TEX_NONE;
	rc.i_ma = APP_MESH_GRID;
	rc.i_transform = 0;
	rcom_draw (&rc, cv, bv, tv, ma, tex, uniform);
}


void update_pos_plot2vid (struct Assets * as)
{
	float x;
	//Plot position to video position.
	x = as->tv [APP_TRANSFORM_PLOT].x;
	TRACE_F ("%f", (double)x);
	x = 0.5f - x;
	x = x * as->video_position_max;
	as->video_position = roundf (x);
}


void update_pos_vid2plot (struct Assets * as)
{
	float x;
	//Video position to plot position.
	x = 0.5f - ((float)as->video_position / (float)as->video_position_max);
	as->tv [APP_TRANSFORM_PLOT].x = x;
}


void plot_move_rel (struct Assets * as, float dx, float dy)
{
	as->tv [APP_TRANSFORM_PLOT].x += dx * (1.0f/as->tv [APP_TRANSFORM_PLOT].dx);
	as->tv [APP_TRANSFORM_PLOT].y += dy * (1.0f/as->tv [APP_TRANSFORM_PLOT].dy);
}


int main (int argc, char *argv[])
{
	struct Option opt;
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	if (opt.mode & OPT_HELP) 
	{
		opt_help (&opt);
		return 0;
	}
	
	SDL_Window * window;
	SDL_GLContext context;
	const Uint8 * keyboard;
	SDL_Event event;
	uint32_t flags = 0;
	

	
	app_init ();
	window = app_create_window ();
	context = SDL_GL_CreateContext_CC (window);
	keyboard = SDL_GetKeyboardState (NULL);
	
	struct Assets as;
	{
		as.values1 [0] = (float *) malloc (APP_VALUES_MAX * sizeof (float));
		as.values1_n [0] = APP_VALUES_MAX;
		FILE * f = fopen (opt.vall, "r");
		ASSERT (f != NULL);
		read_values (f, 1, &as.values1_n [0], as.values1 [0]);
		fclose (f);
	}
	{
		as.values1 [1] = (float *) malloc (APP_VALUES_MAX * sizeof (float));
		as.values1_n [1] = APP_VALUES_MAX;
		FILE * f = fopen (opt.valr, "r");
		ASSERT (f != NULL);
		read_values (f, 1, &as.values1_n [1], as.values1 [1]);
		fclose (f);
	}
	
	for (size_t i = 0; i < 15900; ++i)
	{
		as.values1 [1] [i] = (float)rand () / (float)RAND_MAX;
		as.values1_n [1] = 15900;
	}
	
	
	//as->cap [APP_VIDSTREAM1].open ("data/c1_20180627_200007.mp4", cv::CAP_FFMPEG);
	//as->cap [APP_VIDSTREAM2].open ("data/c1_20180817_142743.mp4", cv::CAP_FFMPEG);
	as.cap [APP_VIDSTREAM1].open (opt.vidl);
	as.cap [APP_VIDSTREAM2].open (opt.vidr);
	asset_init (&as, &flags);
	app_setup_texture (&flags, as.cap [APP_VIDSTREAM1], as.tex [APP_TEX_VID1]);
	app_setup_texture (&flags, as.cap [APP_VIDSTREAM2], as.tex [APP_TEX_VID2]);
	
	
	TRACE_F ("nframes %i", (int)as.cap [0].get (CV_CAP_PROP_FRAME_COUNT));
	TRACE_F ("nframes %i", (int)as.cap [1].get (CV_CAP_PROP_FRAME_COUNT));
	while (1)
	{
		if (flags & APP_QUIT) {break;}
		if (flags & APP_ERROR) {break;}
		
		while (SDL_PollEvent (&event))
		{

			switch (event.type)
			{
				case SDL_QUIT:
				flags |= APP_QUIT;
				break;
				
				case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
					event.type = SDL_QUIT;
					SDL_PushEvent (&event);
					break;
					
					case SDLK_LEFT:
					as.video_position --;
					update_pos_vid2plot (&as);
					cap_set1 (APP_VIDSTREAM_COUNT, as.cap, as.video_position);
					update_cap_tex (APP_VID2TEX_COUNT, as.vid2tex, as.cap, as.tex);
					break;
					
					case SDLK_RIGHT:
					as.video_position ++;
					update_pos_vid2plot (&as);
					cap_set1 (APP_VIDSTREAM_COUNT, as.cap, as.video_position);
					update_cap_tex (APP_VID2TEX_COUNT, as.vid2tex, as.cap, as.tex);
					break;
					
					case SDLK_0:
					as.video_position = 0;
					update_pos_vid2plot (&as);
					cap_set1 (APP_VIDSTREAM_COUNT, as.cap, as.video_position);
					update_cap_tex (APP_VID2TEX_COUNT, as.vid2tex, as.cap, as.tex);
					break;
				}
				break;
				
				case SDL_MOUSEBUTTONDOWN:
				flags |= APP_MOUSE_DRAG;
				break;
				
				case SDL_MOUSEBUTTONUP:{
				flags &= ~APP_MOUSE_DRAG;
				update_pos_plot2vid (&as);
				update_pos_vid2plot (&as);
				cap_set1 (APP_VIDSTREAM_COUNT, as.cap, as.video_position);
				update_cap_tex (APP_VID2TEX_COUNT, as.vid2tex, as.cap, as.tex);
				//TRACE_F ("%f", (double)x);
				}break;
				
				case SDL_MOUSEMOTION:
				if (flags & APP_MOUSE_DRAG)
				{
					plot_move_rel (&as, 0.002f * event.motion.xrel, -0.002f * event.motion.yrel);
				}
				break;
				
				case SDL_MOUSEWHEEL:
				as.tv [APP_TRANSFORM_PLOT].dx += 0.1f * (float)event.wheel.y;
				as.tv [APP_TRANSFORM_PLOT].dy += 0.1f * (float)event.wheel.y;
				break;
				
				case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
					handle_winresize (&as, event.window.data1, event.window.data2);
					break;
				}
				break;
				

			}
		}
		
		if (!(flags & APP_MOUSE_DRAG))
		{
			as.video_position ++;
			update_pos_vid2plot (&as);
			cap_set1 (APP_VIDSTREAM_COUNT, as.cap, as.video_position);
			update_cap_tex (APP_VID2TEX_COUNT, as.vid2tex, as.cap, as.tex);
		}

		draw_test (as.cv, as.bv1, as.tv, as.ma, as.tex, as.uniform);
		SDL_GL_SwapWindow (window);	
		SDL_Delay (10);
	
	}
	
	
	
	return 0;
}











