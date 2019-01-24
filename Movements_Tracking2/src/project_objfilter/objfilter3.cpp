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




void draw_poi (cv::Mat & image, struct POI * p)
{
	//printf ("g %i\n", p->g);
	cv::RNG rng (p->g);
	cv::Scalar c = cv::Scalar (rng.uniform (0, 255), rng.uniform (0, 255), rng.uniform (0, 255));
	cv::Point2f pc (p->p [0], p->p [1]);
	cv::circle (image, pc, 10.0f, c, -1, cv::LINE_4);	
	cv::circle (image, pc, sqrt (p->dr2), c, 2, cv::LINE_4);	
	
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




void draw_pair (cv::Mat & image, struct POI_Pair * x)
{
	cv::Point2f pa (x->a->p [0], x->a->p [1]);
	cv::Point2f pb (x->b->p [0], x->b->p [1]);
	cv::line (image, pa, pb, cv::Scalar (255, 255, 255), 1, cv::LINE_4);
}


struct POI_Context
{
	uint32_t g;
};


int search1 (struct POI_Context * ctx, struct POI * p, size_t pn, struct POI_Pair * q, size_t qn)
{
	size_t k = 0;
	for (size_t i = 0;     i < pn; i = i + 1)
	for (size_t j = i + 1; j < pn; j = j + 1)
	{
		struct POI * a = p + i;
		struct POI * b = p + j;
		
		if (a->m == 1) {continue;}
		if (b->m == 1) {continue;}
		if ((a->t + a->dt) < b->t) {break;}
		if (a->t == b->t) {continue;}
		int32_t r [2];
		int32_t r2 = length2 (a->p, b->p, r, 2);
		if (r2 > a->dr2) {continue;}
		if (k >= qn) {break;}
		q [k].a = a;
		q [k].b = b;
		k ++;
	}
	return k;
}

void search2 (struct POI_Context * ctx, struct POI * p, size_t pn, struct POI_Pair * q, size_t qn, uint32_t dr2)
{
	ctx->g ++;
	for (size_t i = 0; i < qn; i = i + 1)
	{
		q [i].a->m = 1;
		q [i].b->m = 1;
		if (q [i].a->g == 0) {q [i].a->g = ctx->g;}
		if (q [i].b->g == 0) {q [i].b->g = ctx->g;}
	}
	
	for (size_t i = 0; i < pn; i = i + 1)
	{
		if (p [i].m == 1) {continue;}
		p [i].dr2 = dr2;
	}
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
	struct POI_Pair q [10000];
	FILE * f = fopen ("poi11.txt", "r");
	int n = poi_load (f, p, COUNTOF (p));
	printf ("n %i\n", n);
	fclose (f);
	
	
	for (size_t i = 0; i < n; i = i + 1)
	{
		p [i].i = i;
		p [i].g = 0;
		p [i].gn = 0;
		p [i].m = 0;
		//p [i].dr2 = 500 * 500;
		p [i].dr2 = 10 * 10;
		p [i].dt = 3;
	}
	
	struct POI_Context ctx;
	ctx.g = 0;
	
	int m = 0;
	uint32_t dr = 100;
	
	for (size_t i = 0; i < n; i = i + 1)
	{
		draw_poi (frame1, p + i);
	}
	for (size_t i = 0; i < m; i = i + 1)
	{
		draw_pair (frame1, q + i);
	}
		
	while (1)
	{
		show ("1", frame1);
		
		int key = cv::waitKey (1);
        if( key == 27 || key == 'q' || key == 'Q' ) {break;}
        if (key == 'u')
        {
			frame2.copyTo (frame1);
			for (size_t i = 0; i < n; i = i + 1)
			{
				draw_poi (frame1, p + i);
			}
			
			m += search1 (&ctx, p, n, q + m, COUNTOF (q));
			printf ("m %i\n", (int) m);
			
			search2 (&ctx, p, n, q, m, dr * dr);
			dr += 100;
			
			

			for (size_t i = 0; i < m; i = i + 1)
			{
				draw_pair (frame1, q + i);
			}
		}
	}
	
	
	
	
	
	
	
	
	

    
    return 0;
}
