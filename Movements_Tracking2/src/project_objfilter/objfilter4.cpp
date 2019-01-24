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






void draw_poi_next (cv::Mat & image, struct POI_Context * ctx, struct POI * pv, size_t pn)
{
	//while (1)
	for (size_t i = 0; i < pn; i = i + 1)
	{
		struct POI * x = pv + i;
		//if (x->prev != NULL) {continue;}
		while (1)
		{
			if (x->next == NULL) {break;}
			if (x->next == x) {break;}
			cv::Point2f pa (x->p [POI_X], x->p [POI_Y]);
			cv::Point2f pb (x->next->p [POI_X], x->next->p [POI_Y]);
			cv::arrowedLine (image, pa, pb, cv::Scalar (180, 180, 255), 1, cv::LINE_4, 0, 0.2);
			x = x->next;
		}
	}
}




void draw_poi (cv::Mat & image, struct POI_Context * ctx, struct POI * pv, size_t pn)
{	
	char buf [30];
	snprintf (buf, sizeof (buf), "%f", ctx->s [POI_T]);
	cv::putText 
	(
		image, buf,
		cv::Point2f (20.0f, 50.0f), // Coordinates
		cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
		2.0f, // Scale. 2.0 = 2x bigger
		cv::Scalar(255,255,255), // BGR Color
		1.2f // Line Thickness (Optional)
	);
		
		
	for (size_t i = 0; i < pn; i = i + 1)
	{
		//printf ("g %i\n", pv [i].g);
		cv::Point2f pc (pv [i].p [POI_X], pv [i].p [POI_Y]);
		cv::Scalar c = cv::Scalar (0, 0, 0);

		snprintf (buf, sizeof (buf), "%i", (int) pv [i].p [POI_T]);
		//snprintf (buf, sizeof (buf), "%i", (int) pv [i].l);
		//snprintf (buf, sizeof (buf), "%i", (int) pv [i].g);
		//memset (buf, 0, sizeof (buf));
		//snprintf (buf, sizeof (buf), "%x %x", pv + i, pv [i].prev);
		//printf ("%p\n", pv [i].next);
		cv::putText 
		(
			image, buf,
			pc + cv::Point2f (20.0f, -20.0f), // Coordinates
			cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
			1.5f, // Scale. 2.0 = 2x bigger
			cv::Scalar (255,255,255), // BGR Color
			1.2f // Line Thickness (Optional)
		);
		


		//if (pv [i].prev == NULL) 
		{
			//c = cv::Scalar (100, 100, 255);	
		}
		//else
		{
			cv::RNG rng (pv [i].g);
			c = cv::Scalar (rng.uniform (0, 255), rng.uniform (0, 255), rng.uniform (0, 255));
		}
		
		//if (pv [i].prev == NULL)
		{
			cv::circle (image, pc, 10.0f, c, -1, cv::LINE_4);
			//cv::circle (image, pc, sqrt (pv [i].r2_xy), c, 2, cv::LINE_4);
		}
	}
}


int poi_find_t (struct POI * x, size_t n, float t)
{
	int i = 0;
	for (i = 0; i < n; i = i + 1)
	{
		//TRACE_F ("%f %f", x [i].p [POI_T], t);
		if (x [i].p [POI_T] >= t) {break;}
	}
	return i;
}



int main( int /*argc*/, char** /*argv*/ )
{
	cv::VideoCapture cap;
	cap.open ("c2_20180626_190007.mp4");
	ASSERT (cap.isOpened ());
	double w = cap.get (cv::CAP_PROP_FRAME_WIDTH);
	double h = cap.get (cv::CAP_PROP_FRAME_HEIGHT);
	cv::Mat frame1 (h, w, CV_8UC3, cv::Scalar (0, 0, 0));
	cv::Mat frame2 (h, w, CV_8UC3, cv::Scalar (0, 0, 0));
	cap >> frame2;
	frame2.copyTo (frame1);
	
	struct POI pv_data [10000];
	FILE * pf = fopen ("poi2.txt", "r");
	int pn = poi_load (pf, pv_data, COUNTOF (pv_data));
	printf ("pn %i\n", pn);
	fclose (pf);
	int pv_i = poi_find_t (pv_data, pn, 200.0f);
	struct POI * pv = pv_data + pv_i;
	pn = pn - pv_i;
	printf ("pn %i\n", pn);
	
	

	
	struct POI_Context ctx;
	ctx.s [POI_T] = 1;
	ctx.s [POI_X] = 1;
	ctx.s [POI_Y] = 1;
	ctx.g = 1;
	
	bool update = true;

	while (1)
	{
		show ("1", frame1);
		
		int key = cv::waitKey (100);
        if( key == 27 || key == 'q' || key == 'Q' ) {break;}	
        if (key == 'u')
        {
			update = true;
			ctx.s [POI_T] += 1;
		}
		
		
		if (update)
        {
			ctx.g = 1;
			poi_reset (pv, pn);
			dev_search1_distance (&ctx, pv, pn);
			dev_search3_backtrack (&ctx, pv, pn);
			dev_search4_relink (&ctx, pv, pn);	
			dev_search2_length (&ctx, pv, pn);
			dev_search5_group (&ctx, pv, pn);
			
			
			frame2.copyTo (frame1);
			draw_poi (frame1, &ctx, pv, pn);
			draw_poi_next (frame1, &ctx, pv, pn);
			//draw_poi_prev (frame1, &ctx, pv, pn);
			update = false;
		}
	}
	
	
	
	
	
	
	
	
	

    
    return 0;
}
