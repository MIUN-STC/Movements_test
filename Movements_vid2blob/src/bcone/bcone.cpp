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
	
	float * pos; //Position vector
	float * vel; //Velocity vector
	float * D; //Blob diameter
	float * Davg; //Blob average diameter
	uint32_t * persistence;
	float * A; //Angle of velocity
	uint32_t * duration;
};


void pm_init (struct particle_model * m)
{
	m->pos = (float *) calloc (m->cap, sizeof (float) * 2);
	m->vel = (float *) calloc (m->cap, sizeof (float) * 2);
	m->D = (float *) calloc (m->cap, sizeof (float));
	m->Davg = (float *) calloc (m->cap, sizeof (float));
	m->A = (float *) calloc (m->cap, sizeof (float));
	m->persistence = (uint32_t *) calloc (m->cap, sizeof (uint32_t));
	m->duration = (uint32_t *) calloc (m->cap, sizeof (uint32_t));
}


void pm_random (struct particle_model * m, uint32_t w, uint32_t h)
{
	for (uint32_t i = 0; i < m->n; ++i)
	{
		m->pos [i*2+0] = (float) rand () / (float) RAND_MAX;
		m->pos [i*2+1] = (float) rand () / (float) RAND_MAX;
		m->pos [i*2+0] *= w;
		m->pos [i*2+1] *= h;
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
		cv::circle (img, p, D [i], cv::Scalar (r, g, b), 1);
	}
}


void draw_blob 
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
		cv::circle (img, p, 3, cv::Scalar (r, g, b), -1);
		//cv::circle (img, p, D [i], cv::Scalar (r, g, b), 1);
	}
}


void draw_id 
(
	cv::Mat &img,
	uint32_t n, 
	float pos [], 
	float vel [], 
	float D [], 
	uint32_t pers [],
	uint32_t duration [],
	uint32_t persistance,
	float proximity,
	float grow_cone
)
{
	char buf [10];
	for (uint32_t i = 0; i < n; ++i)
	{
		//snprintf (buf, 10, "%i", pers [i]);
		cv::Point2f p (pos [i*2 + 1], pos [i*2 + 0]);
		cv::Point2f v (vel [i*2 + 1], vel [i*2 + 0]);
		cv::Point2f p1 = p + v*10;
		
		if (pers [i] < persistance)
		{
			snprintf (buf, 10, "%i %i", i, pers [i]);
			float proxy =  proximity + (pers [i] * grow_cone);
			//cv::circle (img, p, D [i], cv::Scalar (255, 0, 255), 1);
			cv::circle (img, p, proxy, cv::Scalar (255, 0, 255), 0.5);
			cv::circle (img, p, 3, cv::Scalar (255, 0, 255), -1);
			cv::putText (img, buf, p, CV_FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar (50, 100, 255), 1);
			cv::arrowedLine (img, p, p1, cv::Scalar (0, 255, 255), 1, 8, 0, 0.1);
		}
		
	}
}






void pm_update 
(
	uint32_t n0,
	float pos0 [],
	float vel0 [],
	float D0 [],
	float Davg0 [],
	uint32_t pers0 [],
	uint32_t duration0 [],

	uint32_t n1,
	float const pos1 [],
	float const D1 [],
	
	uint32_t persistence,
	float proximity,
	float grow_cone
)
{
	for (uint32_t i1 = 0; i1 < n1; ++i1)
	{
		float dmin = FLT_MAX;
		uint32_t imin = UINT32_MAX;
		for (uint32_t i0 = 0; i0 < n0; ++i0)
		{
			float d = v2f32_dist2 (pos0 + (i0*2), pos1 + (i1*2));
			float prox = proximity + ((float)pers0 [i0] * grow_cone);
			if ((pers0 [i0] < persistence) && (d > (prox*prox))) {continue;};
			if (d < dmin)
			{
				dmin = d;
				imin = i0;
			}
		}
		//TRACE_F ("%f", dmin);
		if (imin == UINT32_MAX) {continue;}
		
		if (pers0 [imin] <= 1)
		{
			float delta [2];
			v2f32_sub (delta, pos1 + (i1*2), pos0 + (imin*2));
			vel0 [imin*2 + 0] = (0.9f * vel0 [imin*2 + 0]) + (0.3f * delta [0]);
			vel0 [imin*2 + 1] = (0.9f * vel0 [imin*2 + 1]) + (0.3f * delta [1]);
		}
		
		vf32_cpy (pos0 + (imin*2), pos1 + (i1*2), 2);

		Davg0 [imin] = (1.0f * D1 [i1]) + (0.0f * Davg0 [imin]);
		//pers0 [imin] = 0;
		//pers0 [imin] = MIN (pers0 [imin], persistence);
		pers0 [imin] = pers0 [imin] >> 2;
		duration0 [imin] ++;
		//A0 [imin] = atan2f (deltamin [1], deltamin [0]);	
	}
	
	for (uint32_t i = 0; i < n0; ++i)
	{
		pers0 [i] += 1;
		if (pers0 [i] > persistence) 
		{
			duration0 [i] = 0;
			vel0 [i*2 + 0] = 0;
			vel0 [i*2 + 1] = 0;
		}
		v2f32_add (pos0 + (i*2), pos0 + (i*2), vel0 + (i*2));
		v2f32_mus (vel0 + (i*2), vel0 + (i*2), 0.96f);
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
	blobparams.minDistBetweenBlobs = 30;
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
	pm.cap = 40;
	sm.n = 0;
	pm.n = 40;
	pm_init (&sm);
	pm_init (&pm);
	pm_random (&pm, w, h);
	for (uint32_t i = 0; i < pm.n; ++i) {pm.D [i] = 50.0f;}
	
	
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
					break;
					
					case SDLK_RIGHT:
					flags ^= FORWARD;
					flags |= SEMIAUTO;
					break;
					
					case SDLK_SPACE:
					flags ^= FORWARD;
					flags &= ~SEMIAUTO;
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
			f0.copyTo (f1);
			cv::morphologyEx (f0, f1, cv::MORPH_GRADIENT, element);
			bgfs->apply (f1, mask);
			cv::blur (mask, mask, cv::Size (3, 3));
			
			blobber->detect (mask, kp);
			f0.copyTo (f1);
			//cv::Mat in [3] = {mask, mask, mask};
			//cv::merge (in, 3, f1);
			sm.n = sm.cap;
			kp_to_v2f32 (&sm.n, sm.pos, sm.D, kp);
			
			draw_blob (f1, sm.n, sm.pos, sm.D, 0, 0, 255);
			//draw_circle (f1, pm.n, pm.pos, pm.Davg, 0, 255, 0);
			pm_update 
			(
				pm.n, 
				pm.pos, 
				pm.vel, 
				pm.D, 
				pm.Davg, 
				pm.persistence, 
				pm.duration,
				sm.n, 
				sm.pos, 
				sm.D, 
				7, 
				20.0f,
				7.0f
			);
			//pm_random (&pm);
			//draw_circle (f1, sm.n, sm.pos, sm.D, 0, 255, 0);
			//draw_circle (f1, pm.n, pm.pos, pm.Davg, 255, 0, 255);
			draw_id 
			(
				f1, 
				pm.n, 
				pm.pos, 
				pm.vel, 
				pm.Davg, 
				pm.persistence,
				pm.duration,
				7,
				20.0f,
				7.0f
			);
			
			if (0)
			{
				uint32_t k0 = 0;
				uint32_t k1 = 0;
				uint32_t k2 = 0;
				for (uint32_t i = 0; i < pm.n; ++i) 
				{
					if (pm.persistence [i] == 0)      {k0++;}
					else if (pm.persistence [i] == 4) {k1++;}
					else                              {k2++;}
				}
				char buf [100] = {0};
				snprintf (buf, 100, "b: %03i   k: (%03i %03i %03i)", sm.n, k0, k1, k2);
				cv::Point2f p (10, 20);
				cv::putText (f1, buf, p, CV_FONT_HERSHEY_DUPLEX, 0.76, cv::Scalar (0, 0, 255), 1);
			}
			

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

