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
#include "v2.h"

//Application
#include "bcone.h"


void kp_to_v2f32 (uint32_t *n, float v [], float D [], std::vector <cv::KeyPoint> const & kp)
{
	uint32_t i;
	for (i = 0; i < (*n) && i < kp.size (); ++i)
	{
		v [i*2 + 0] = kp [i].pt.y;
		v [i*2 + 1] = kp [i].pt.x;
		D [i] = kp [i].size;
	}
	(*n) = i;
}


struct particle_model
{
	uint32_t cap;
	uint32_t n;
	float * pos;
	float * D;
};


void pm_init (struct particle_model * m)
{
	m->pos = (float *) calloc (m->cap, sizeof (float) * 2);
	m->D = (float *) calloc (m->cap, sizeof (float));
}


void pm_random (struct particle_model * m)
{
	for (uint32_t i = 0; i < m->n; ++i)
	{
		m->pos [i*2+0] = (float) rand () / (float) RAND_MAX;
		m->pos [i*2+1] = (float) rand () / (float) RAND_MAX;
		m->pos [i*2+0] *= 1080*2;
		m->pos [i*2+1] *= 1920*2;
	}
}


void draw_circle 
(
	cv::Mat &img,
	uint32_t n, 
	float pos [], 
	float D [],  
	uint8_t r, 
	uint8_t g, 
	uint8_t b
)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		cv::Point2f p (pos [i*2 + 1], pos [i*2 + 0]);
		cv::circle (img, p, D [i], cv::Scalar (r, g, b), 2);
	}
}


void pm_update 
(
	uint32_t n0,
	float pos0 [],
	float D0 [],

	uint32_t n1,
	float const pos1 [],
	float const D1 []
)
{
	for (uint32_t i0 = 0; i0 < n0; ++i0)
	{
		float Dsum = 0;
		for (uint32_t i1 = 0; i1 < n1; ++i1)
		{
			float d [2];
			v2f32_sub (d, pos0 + (i0*2), pos1 + (i1*2));
			Dsum += 10000.0f * D1 [i1] * (1.0f / v2f32_dot (d, d));
		}
		//TRACE_F ("%f", Dsum);
		D0 [i0] = Dsum;
	}
}


int main (int argc, char** argv)
{
	struct Option opt;
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	if (opt.mode & OPT_HELP) {opt_help (&opt);return 0;}
	if (opt.src == NULL) {return 0;}

	cv::VideoCapture cap (opt.src);
	double w = cap.get (cv::CAP_PROP_FRAME_WIDTH);
	double h = cap.get (cv::CAP_PROP_FRAME_HEIGHT);
	
	cv::Mat f0 (h, w, CV_8UC3);
	cv::Mat f1 (h, w, CV_8UC3);
	cv::Mat mask (h, w, CV_8UC1);
	
	cv::Ptr<cv::BackgroundSubtractorKNN> bgfs;
	bgfs = cv::createBackgroundSubtractorKNN ();
	bgfs->setHistory (100);
	bgfs->setDist2Threshold (2000.0);
	bgfs->setDetectShadows (false);
	
	
	cv::SimpleBlobDetector::Params blobparams;
	blobparams.minDistBetweenBlobs = 20;
	blobparams.minRepeatability = 2;
	blobparams.minThreshold = 200;
	blobparams.maxThreshold = 255;
	blobparams.filterByArea = false;
	blobparams.minArea = 40;
	blobparams.maxArea = 10000;
	blobparams.filterByColor = false;
	blobparams.blobColor = 255;
	blobparams.filterByCircularity = false;
	blobparams.minCircularity = 0.7;
	blobparams.maxCircularity = 1.0;
	blobparams.filterByConvexity = false;
	blobparams.minConvexity = 0.4;
	blobparams.filterByInertia = false;
	blobparams.minInertiaRatio = 0.3;
	
	cv::Ptr<cv::SimpleBlobDetector> blobber = cv::SimpleBlobDetector::create (blobparams);
	std::vector <cv::KeyPoint> kp;
	
	int morph_elem = cv::MorphShapes::MORPH_ELLIPSE;
	int morph_size = 4;
	cv::Mat element = cv::getStructuringElement (morph_elem, cv::Size (2*morph_size + 1, 2*morph_size+1), cv::Point (morph_size, morph_size) );
	
	
	struct particle_model sm;
	struct particle_model pm;
	sm.cap = 100;
	pm.cap = 1000;
	sm.n = 0;
	pm.n = 1000;
	pm_init (&sm);
	pm_init (&pm);
	pm_random (&pm);
	
	ASSERT (SDL_Init (SDL_INIT_EVERYTHING) == 0);
	SDL_Window * window = NULL;
	SDL_Renderer * renderer = NULL;
	ASSERT (SDL_CreateWindowAndRenderer (800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer) == 0);
	
	uint32_t flags = 0;
	flags |= cap.read (f0) ? 0 : QUIT;
	uint32_t iframe = cap.get (cv::CAP_PROP_POS_FRAMES);
	SDL_Texture * texture = SDLCV_CreateTexture (renderer, f0);
	SDL_Event event;
	
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
					
					case SDLK_k:
					flags ^= SEMIAUTO;
					if (flags & SEMIAUTO) {TRACE ("SEMIAUTO ON");}
					else {TRACE ("SEMIAUTO OFF");}
					break;
					
					case SDLK_RIGHT:
					flags |= FORWARD;
					break;
				}
				break;
			}
		}
		if (flags & QUIT) {break;}
		if (flags & PAUSE) {continue;}
	
		if (flags & FORWARD) 
		{
			flags |= cap.read (f0) ? 0 : QUIT;
			if (flags & QUIT) {break;}
			cv::morphologyEx (f0, f1, cv::MORPH_GRADIENT, element);
			bgfs->apply (f1, mask);
			blobber->detect (mask, kp);
			f0.copyTo (f1);
			sm.n = sm.cap;
			kp_to_v2f32 (&sm.n, sm.pos, sm.D, kp);
			pm_update (pm.n, pm.pos, pm.D, sm.n, sm.pos, sm.D);
			//pm_random (&pm);
			draw_circle (f1, pm.n, pm.pos, pm.D, 255, 0, 0);
			draw_circle (f1, sm.n, sm.pos, sm.D, 0, 255, 0);
			
		}
		
		if (flags & SEMIAUTO) 
		{
			flags &= ~(FORWARD | BACKWARD);
		}
		

		
		
		SDLCV_CopyTexture (texture, f1);
		SDL_RenderClear (renderer);
		SDL_RenderCopy (renderer, texture, NULL, NULL);
		SDL_RenderPresent (renderer);
	}
	
	return 0;
}

