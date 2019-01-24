#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "debug.h"
#include "poi.h"
#include "common.h"

void show (char const * name, cv::Mat const & image)
{
	cv::Mat dst;
	cv::resize (image, dst, cv::Size (1920, 1080));
	cv::imshow (name, dst);
}


int32_t dot (int32_t const * a, int32_t const * b, size_t n)
{
	int32_t r = 0;
	for (int i = 0; i < n; i = i + 1)
	{
		r += a [i] * b [i];
	}
	return r;
}


void sub (int32_t const * a, int32_t const * b, int32_t * r, size_t n)
{
	for (int i = 0; i < n; i = i + 1)
	{
		r [i] = a [i] - b [i];
	}
}


int32_t length2 (int32_t const * a, int32_t const * b, int32_t * d, size_t n)
{
	int32_t d2;
	sub (a, b, d, n);
	d2 = dot (d, d, n);
	return d2;
}

/*
void search1 (struct POI * p, size_t n)
{
	int32_t d2min = INT32_MAX;
	int32_t t;
	int imin = -1;
	int jmin = -1;
	for (int i = 0; i < n; i = i + 1)
	{
		//if (p [i].ci > 0) {continue;}
		for (int j = i + 1; j < n; j = j + 1)
		{
			if (p [j].ci > 0) {continue;}
			int32_t d [2];
			int32_t d2 = dist (p [i].p, p [j].p, d, 2);
			if (d2 < d2min) 
			{
				d2min = d2;
				imin = i; 
				jmin = j;
			}
		}
	}
	if (imin < 0) {return;}
	if (jmin < 0) {return;}
	if (p [imin].ci < 0) {p [imin].ci = jmin;}
	if (p [jmin].ci < 0) {p [jmin].ci = imin;}
}


void search2 (struct POI * p, size_t n)
{
	int32_t d2min = INT32_MAX;
	int jmin = -1;
	for (int i = 0; i < n; i = i + 1)
	{
		for (int j = i + 1; j < n; j = j + 1)
		{
			if (p [j].ti >= 0) {continue;}
			int32_t d [2];
			int32_t d2 = dist (p [i].p, p [j].p, d, 2);
			if (d2 < d2min) 
			{
				d2min = d2;
				jmin = j;
			}
		}
	}
	p [i].ti = jmin;
}
*/


/*
int32_t search3 (int32_t a, struct POI * p, size_t n)
{
	if (a < 0) {return a;}
	int32_t d2min = INT32_MAX;
	int jmin = -1;
	for (int j = 0; j < n; j = j + 1)
	{
		if (j == a) {continue;}
		if (p [j].ti > 0) {continue;}
		int32_t d [2];
		int32_t d2 = dist (p [a].p, p [j].p, d, 2);
		if (d2 < d2min) 
		{
			d2min = d2;
			jmin = j;
		}
	}
	if (jmin < 0) {return jmin;}
	p [jmin].ci = a;
	p [a].ti = jmin;
	return jmin;
}
*/

/*
void search4 (struct POI * p, size_t n)
{
	int32_t d2min = INT32_MAX;
	int32_t t;
	int imin = -1;
	int jmin = -1;
	for (int i = 0; i < n; i = i + 1)
	{
		//if (p [i].ci > 0) {continue;}
		for (int j = i + 1; j < n; j = j + 1)
		{
			if (p [j].ci > 0) {continue;}
			int32_t d [2];
			int32_t d2 = dist (p [i].p, p [j].p, d, 2);
			if (d2 < d2min) 
			{
				d2min = d2;
				imin = i; 
				jmin = j;
			}
		}
	}
	if (imin < 0) {return;}
	if (jmin < 0) {return;}
	if (p [imin].ci < 0) {p [imin].ci = jmin;}
	if (p [jmin].ci < 0) {p [jmin].ci = imin;}
}
*/


struct POIS
{
	int32_t g;
	int32_t ai;
};


void search5 (struct POIS * s, struct POI * x, size_t n)
{
	uint32_t gn = 0;
	for (size_t i = 0; i < n; i = i + 1)
	{
		int32_t r [2];
		int32_t r2 = length2 (x [i].p, x [s->ai].p, r, 2);
		int32_t t = abs (x [i].t - x [s->ai].t);
		if (t > x [s->ai].dt) {continue;}
		if (r2 < x [i].dr2)
		{
			x [i].g = x [s->ai].g;
			gn ++;
		}
	}
	x [s->ai].gn = gn;
}


void search6 (struct POIS * s, struct POI * x, size_t n)
{
	for (size_t i = 0; i < n; i = i + 1)
	{
		if (x [i].gn == 0) 
		{
			if (x [i].g == 0) 
			{
				s->g ++;
				x [i].g = s->g;
			}
			s->ai = i;
			break;
		}
	}
	//printf ("s.ai %i\n", s->ai);
}


void search7 (struct POIS * s, struct POI * x, size_t n)
{
	for (size_t i = 0; i < n; i = i + 1)
	{
		if ((x [i].gn >= 1) && (x [i].gn <= 2)) 
		{
			x [i].g = 0;
		}
	}
	//printf ("s.ai %i\n", s->ai);
}



void draw_poi (cv::Mat & image, struct POI * p)
{
	//printf ("g %i\n", p->g);
	cv::RNG rng (p->g);
	cv::Scalar c = cv::Scalar (rng.uniform (0, 255), rng.uniform (0, 255), rng.uniform (0, 255));
	cv::Point2f pc (p->p [0], p->p [1]);
	cv::circle (image, pc, 10.0f, c, -1, cv::LINE_4);	
	//cv::circle (image, pc, sqrt (p->dr2), c, 2, cv::LINE_4);	
	
	char buf [10];
	//snprintf (buf, sizeof (buf), "%i %i", p->i, p->g);
	snprintf (buf, sizeof (buf), "%i", p->t);
	
	
	cv::putText 
	(
		image, buf,
		pc + cv::Point2f (20.0f, -20.0f), // Coordinates
		cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
		1.0f, // Scale. 2.0 = 2x bigger
		cv::Scalar(255,255,255), // BGR Color
		1.2f // Line Thickness (Optional)
	);
	
}



int main( int /*argc*/, char** /*argv*/ )
{
	cv::VideoCapture cap;
	cap.open ("/home/sansa/cv_test1/c1_20180626_210007.mp4");
	ASSERT (cap.isOpened ());
	double w = cap.get (cv::CAP_PROP_FRAME_WIDTH);
	double h = cap.get (cv::CAP_PROP_FRAME_HEIGHT);
	cv::Mat frame1 (h, w, CV_8UC3, cv::Scalar (0, 0, 0));
	cv::Mat frame2 (h, w, CV_8UC3, cv::Scalar (0, 0, 0));
	cap >> frame2;
	frame2.copyTo (frame1);
	
	struct POI p [10000];
	FILE * f = fopen ("poi11.txt", "r");
	int n = poi_load (f, p, COUNTOF (p));
	printf ("n %i\n", n);
	fclose (f);
	
	struct POIS s;
	s.ai = 4;
	s.g = 1;
	
	for (size_t i = 0; i < n; i = i + 1)
	{
		p [i].i = i;
		p [i].g = 0;
		p [i].gn = 0;
		p [i].dr2 = 100 * 100;
		p [i].dt = 5;
	}
	
	for (size_t i = 0; i < n; i = i + 1)
	{
		draw_poi (frame1, p + i);
	}
			
	while (1)
	{
		show ("1", frame1);
		
		int key = cv::waitKey (1);
        if( key == 27 || key == 'q' || key == 'Q' ) {break;}
        //if (key == 'u')
        {
			search6 (&s, p, n);
			search5 (&s, p, n);
			//search7 (&s, p, n);
			frame2.copyTo (frame1);
			for (size_t i = 0; i < n; i = i + 1)
			{
				draw_poi (frame1, p + i);
			}
		}
	}
	
	
	
	
	
	
	
	
	

    
    return 0;
}
