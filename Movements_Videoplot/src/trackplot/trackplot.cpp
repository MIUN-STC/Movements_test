#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>

//Common
#include "SDLGL.h"
#include "SDLCV.h"
#include "option.h"
#include "debug.h"

//Application
#include "trackplot.h"




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
	
	//Relationship array:
	uint32_t nr,
	uint32_t r_start [], 
	uint32_t r_current [], 
	uint32_t r_next [], 
	//TODO: Optimization. Use this for start index for relationship index.
	uint32_t * ir0,
	
	//Blob array:
	uint32_t          nb, 
	struct v4f32_txy0 b_pos [],
	float             b_diameter []
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
		if ((uint32_t)b_pos [ib0].t <= iframe && (uint32_t)b_pos [ib1].t >= iframe) 
		//if ((uint32_t)pos [ib0].t <= iframe) 
		{
			//TRACE_F ("%i %i", ib0, iframe);

			cv::Point2f p0 (b_pos [ib0].x, b_pos [ib0].y);
			cv::Point2f p1 (b_pos [ib1].x, b_pos [ib1].y);
			cv::circle (img, p0, 10.0f, cv::Scalar (r, g, b), -1);
			if (ib2 < nb)
			{
				cv::Point2f p2 (b_pos [ib2].x, b_pos [ib2].y);
				cv::arrowedLine (img, p1, p2, cv::Scalar (r, g, b), 2);
			}
		}
		
		
		if ((uint32_t)b_pos [ib1].t == iframe)
		{
			cv::Point2f p0 (b_pos [ib0].x, b_pos [ib0].y);
			cv::Point2f p1 (b_pos [ib1].x, b_pos [ib1].y);
			cv::arrowedLine (img, p0, p1, cv::Scalar (r, g, b), 2);
		}
		
	}
	//if ((uint32_t)pos [r_start [*ir0]].t > iframe) {}
}



void visual_loop 
(
	cv::VideoCapture & cap,
	struct relationship & rs,
	struct blobbington & bt,
	uint32_t & flags
)
{
	double w = cap.get (cv::CAP_PROP_FRAME_WIDTH);
	double h = cap.get (cv::CAP_PROP_FRAME_HEIGHT);
	cv::Mat f0 (h, w, CV_8UC3);
	
	ASSERT (SDL_Init (SDL_INIT_EVERYTHING) == 0);
	SDL_Window * window = NULL;
	SDL_Renderer * renderer = NULL;
	ASSERT (SDL_CreateWindowAndRenderer (800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer) == 0);	
	
	
	
	flags |= cap.read (f0) ? 0 : ERROR;
	SDL_Texture * texture = SDLCV_CreateTexture (renderer, f0);
	
	uint32_t ir0 = 0;
	SDL_Event event;
	
	//Breaks when user quits.
	//Breaks when reading video stream fails.
	while (1)
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
		if (flags & ERROR) {break;}
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
			&ir0,
			bt.n, 
			bt.pos,
			bt.D
		);
		SDLCV_CopyTexture (texture, f0);
		SDL_RenderClear (renderer);
		SDL_RenderCopy (renderer, texture, NULL, NULL);
		SDL_RenderPresent (renderer);
	}
}



void make_video 
(
	cv::VideoCapture & cap,
	struct relationship & rs,
	struct blobbington & bt,
	uint32_t & flags,
	char const * out,
	uint32_t nf
)
{

    
	double w = cap.get (cv::CAP_PROP_FRAME_WIDTH);
	double h = cap.get (cv::CAP_PROP_FRAME_HEIGHT);
	cv::Mat f0 (h, w, CV_8UC3);
	cv::Mat f1;
	cv::resize (f0, f1, cv::Size(), 0.5, 0.5);
	
	cv::VideoWriter writer;
	//int codec = cv::VideoWriter::fourcc ('M', 'J', 'P', 'G');
	int codec = cv::VideoWriter::fourcc ('m', 'p', '4', 'v');
	double fps = 5.0;
	writer.open (out, codec, fps, f1.size (), true);
	flags |= cap.read (f0) ? 0 : ERROR;
	
	uint32_t ir0 = 0;
	
	//Breaks when user quits.
	//Breaks when reading video stream fails.
	while (1)
	{
		if (flags & ERROR) {break;}
		if (flags & QUIT) {break;}
		if (flags & PAUSE) {continue;}
		
		flags |= cap.read (f0) ? 0 : QUIT;
		if (flags & QUIT) {break;}
		
		uint32_t iframe = cap.get (cv::CAP_PROP_POS_FRAMES);
		if ((nf != 0) && (iframe >= nf)) {break;}
		//draw_blobs (f0, iframe, bt.n, bt.pos);
		draw_blobs2 
		(
			f0, 
			iframe, 
			rs.n, 
			rs.r_start, 
			rs.r_current, 
			rs.r_next, 
			&ir0,
			bt.n, 
			bt.pos,
			bt.D
		);
		cv::resize (f0, f1, f1.size ());
		writer.write (f1);
	}
	writer.release ();
}




int main (int argc, char** argv)
{
	struct Option opt;
	opt.nb = 1000000;
	opt.nr = 1000000;
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
	//Number of elements in relationship and blob arrays to allocate
	rs.n = opt.nr;
	bt.n = opt.nb;
	the_init (&bt, &rs, fblob, frel);
	fclose (frel); frel = NULL;
	fclose (fblob); fblob = NULL;
	
	cv::VideoCapture cap (opt.src_vid);


	uint32_t flags = 0;

	if (opt.des == NULL)
	{
		visual_loop (cap, rs, bt, flags);
	}
	else
	{
		make_video (cap, rs, bt, flags, opt.des, opt.nf);

	}

	

	
	return 0;
}

