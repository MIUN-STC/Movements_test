#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>

#include <opencv2/xfeatures2d.hpp>

//Common
#include "SDLGL.h"
#include "SDLCV.h"
#include "option.h"
#include "debug.h"

//Application
#include "plotb.h"




struct v4f32_txy0
{
	float t;
	float x;
	float y;
	float w;
};



int fread_rs 
(
	FILE * f,
	uint32_t * n,
	uint32_t r_start [],
	uint32_t r_current [],
	uint32_t r_next [],
	uint32_t r_quantity []
)
{
	uint32_t i = 0;
	for (i = 0; i < (*n); ++ i)
	{
		int res = fscanf 
		(
			f, 
			"%u%u%u%u", 
			r_start    + i,
			r_current  + i,
			r_next     + i,
			r_quantity + i
		);
		if (res == EOF) 
		{
			TRACE ("Reached EOF before buffer is full");
			break;
		}
	}
	(*n) = i;
}


int fread_blob 
(
	FILE * f,
	uint32_t * n,
	v4f32_txy0 pos [],
	float diameter [],
	uint32_t nbf []
)
{
	uint32_t i = 0;
	for (i = 0; i < (*n); ++ i)
	{
		int res = fscanf 
		(
			f, 
			"%f%f%f%f%u", 
			&pos [i].t,
			&pos [i].x,
			&pos [i].y,
			diameter + i,
			nbf      + i
		);
		//TRACE_F ("%i : %f %f %f %f %u", res,pos [i].t,pos [i].x,pos [i].y,diameter [i],nbf [i]);
		if (res == EOF) 
		{
			TRACE ("Reached EOF before buffer is full");
			break;
		}
	}
	(*n) = i;
}


struct blobbington
{
	uint32_t n;
	struct v4f32_txy0 * pos;
	float * D;
	uint32_t * nbf;
};


struct relationship
{
	uint32_t n;
	uint32_t * r_start;
	uint32_t * r_current;
	uint32_t * r_next;
	uint32_t * r_quantity;
};


void the_init 
(
	struct blobbington * bt, 
	struct relationship * rs,
	FILE * fblob,
	FILE * frel
)
{
	bt->pos = (struct v4f32_txy0 *) calloc (bt->n, sizeof (struct v4f32_txy0));
	bt->D   = (float *)             calloc (bt->n, sizeof (float));
	bt->nbf = (uint32_t *)          calloc (bt->n, sizeof (uint32_t));
	
	rs->r_start    = (uint32_t *) calloc (rs->n, sizeof (uint32_t));
	rs->r_current  = (uint32_t *) calloc (rs->n, sizeof (uint32_t));
	rs->r_next     = (uint32_t *) calloc (rs->n, sizeof (uint32_t));
	rs->r_quantity = (uint32_t *) calloc (rs->n, sizeof (uint32_t));
	
	ASSERT (bt->pos);
	ASSERT (bt->D);
	ASSERT (bt->nbf);
	ASSERT (rs->r_start);
	ASSERT (rs->r_current);
	ASSERT (rs->r_next);
	ASSERT (rs->r_quantity);
	
	fread_blob (fblob, &bt->n, bt->pos, bt->D, bt->nbf);
	fread_rs (frel, &rs->n, rs->r_start, rs->r_current, rs->r_next, rs->r_quantity);
	TRACE_F ("bt N: %i", bt->n);
	TRACE_F ("rs N: %i", rs->n);
}



void draw_blobs (cv::Mat &img, uint32_t iframe, uint32_t n, struct v4f32_txy0 pos [])
{
	for (uint32_t i = 0; i < n; ++i)
	{
		if ((uint32_t)pos [i].t != iframe) {continue;}
		//TRACE_F ("%f %f", pos [i].x, pos [i].y);
		cv::Point2f p (pos [i].x, pos [i].y);
		cv::circle (img, p, 30.0f, cv::Scalar (0, 255, 0), 2);
	}
}


void draw_blobs1 
(
	cv::Mat &img, 
	uint32_t iframe, 
	uint32_t nr,
	uint32_t r_start [], 
	uint32_t r_current [], 
	uint32_t r_next [], 
	uint32_t nb, 
	struct v4f32_txy0 pos []
)
{
	for (uint32_t ir = 0; ir < nr; ++ir)
	{
		uint32_t ib0 = r_start [ir];
		uint32_t ib1 = r_current [ir];
		uint32_t ib2 = r_next [ir];
		if ((uint32_t)pos [ib1].t != iframe) {continue;}
		//TRACE_F ("%f %f", pos [i].x, pos [i].y);
		cv::Point2f p0 (pos [ib0].x, pos [ib0].y);
		cv::Point2f p1 (pos [ib1].x, pos [ib1].y);
		srand (ib0);
		uint32_t r = rand () % 256;
		uint32_t g = rand () % 256;
		uint32_t b = rand () % 256;
		cv::circle (img, p0, 30.0f, cv::Scalar (r, g, b), 2);
		cv::circle (img, p1, 30.0f, cv::Scalar (r, g, b), 2);
		cv::arrowedLine (img, p0, p1, cv::Scalar (r, g, b), 2);
		char buf [100];
		snprintf (buf, 100, "%i", ib0);
		cv::putText (img, buf, p1, 0, 2, cv::Scalar (r, g, b), 3);
		if (ib2 < nb)
		{
			cv::Point2f p2 (pos [ib2].x, pos [ib2].y);
			cv::arrowedLine (img, p1, p2, cv::Scalar (r, g, b), 2);
		}
	}
}


void draw_blobs2 
(
	cv::Mat &img, 
	uint32_t iframe, 
	uint32_t nr,
	uint32_t r_start [], 
	uint32_t r_current [], 
	uint32_t r_next [], 
	uint32_t nb, 
	struct v4f32_txy0 pos [],
	uint32_t * ir0
)
{
	uint32_t ir;
	for (ir = 0; ir < nr; ++ir)
	{
		uint32_t ib0 = r_start [ir];
		uint32_t ib1 = r_current [ir];
		uint32_t ib2 = r_next [ir];
		srand (ib0);
		uint32_t r = rand () % 256;
		uint32_t g = rand () % 256;
		uint32_t b = rand () % 256;
			
		//if ((uint32_t)pos [ib0].t >= iframe && (uint32_t)pos [ib1].t <= iframe) 
		//if ((uint32_t)pos [ib0].t <= iframe) 
		if ((uint32_t)pos [ib0].t <= iframe && (uint32_t)pos [ib1].t >= iframe) 
		//if ((uint32_t)pos [ib0].t <= iframe) 
		{
			//TRACE_F ("%i %i", ib0, iframe);

			cv::Point2f p0 (pos [ib0].x, pos [ib0].y);
			cv::Point2f p1 (pos [ib1].x, pos [ib1].y);
			cv::circle (img, p0, 10.0f, cv::Scalar (r, g, b), -1);
			if (ib2 < nb)
			{
				cv::Point2f p2 (pos [ib2].x, pos [ib2].y);
				cv::arrowedLine (img, p1, p2, cv::Scalar (r, g, b), 2);
			}
		}
		
		
		if ((uint32_t)pos [ib1].t == iframe)
		{
			cv::Point2f p0 (pos [ib0].x, pos [ib0].y);
			cv::Point2f p1 (pos [ib1].x, pos [ib1].y);
			cv::arrowedLine (img, p0, p1, cv::Scalar (r, g, b), 2);
		}
		
	}
	//if ((uint32_t)pos [r_start [*ir0]].t > iframe) {}
}


int main (int argc, char** argv)
{
	struct Option opt;
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	if (opt.mode & OPT_HELP) {opt_help (&opt);return 0;}
	ASSERT (opt.src_vid);
	ASSERT (opt.src_blob);
	ASSERT (opt.src_rel);

	FILE * fblob = fopen (opt.src_blob, "r");
	FILE * frel = fopen (opt.src_rel, "r");
	ASSERT (frel);
	ASSERT (fblob);
	
	struct relationship rs = {0};
	struct blobbington bt = {0};
	rs.n = 100000;
	bt.n = 100000;
	the_init (&bt, &rs, fblob, frel);

	cv::VideoCapture cap (opt.src_vid);
	double w = cap.get (cv::CAP_PROP_FRAME_WIDTH);
	double h = cap.get (cv::CAP_PROP_FRAME_HEIGHT);
	
	cv::Mat f0 (h, w, CV_8UC3);


	ASSERT (SDL_Init (SDL_INIT_EVERYTHING) == 0);
	SDL_Window * window = NULL;
	SDL_Renderer * renderer = NULL;
	ASSERT (SDL_CreateWindowAndRenderer (800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer) == 0);
	
	uint32_t flags = 0;
	flags |= cap.read (f0) ? 0 : QUIT;
	SDL_Texture * texture = SDLCV_CreateTexture (renderer, f0);
	
	uint32_t ir0 = 0;
	SDL_Event event;
	for (;;)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
				flags |= QUIT;
				break;
				
				case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
					flags |= QUIT;
					break;

					case SDLK_p:
					flags ^= PAUSE;
					break;
				}
				break;
			}
		}
		if (flags & QUIT) {break;}
		if (flags & PAUSE) {continue;}
		

		flags |= cap.read (f0) ? 0 : QUIT;
		if (flags & QUIT) {break;}
		uint32_t iframe = cap.get (cv::CAP_PROP_POS_FRAMES);
		//draw_blobs (f0, iframe, bt.n, bt.pos);
		draw_blobs2 
		(
			f0, 
			iframe, 
			rs.n, 
			rs.r_start, 
			rs.r_current, 
			rs.r_next, 
			bt.n, 
			bt.pos,
			&ir0
		);
		SDLCV_CopyTexture (texture, f0);
		SDL_RenderClear (renderer);
		SDL_RenderCopy (renderer, texture, NULL, NULL);
		SDL_RenderPresent (renderer);
	}
	
	fclose (frel);
	fclose (fblob);
	
	return 0;
}

