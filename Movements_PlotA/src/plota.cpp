#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/xfeatures2d.hpp>

//Common
#include "global.h"
#include "debug.h"

#include "app.h"

#include <unistd.h>
void read_blob (uint32_t * n, float t [], float x [], float y [], FILE * f)
{
	uint32_t i;
	for (i = 0; i < (*n); ++ i)
	{
		int r = fscanf (f, "%f%f%f", t + i, x + i, y + i);
		if (r == EOF) {break;}
	}
	(*n) = i;
}


void read_track (uint32_t * n, uint32_t t [], uint32_t o [], uint32_t l [], uint32_t b [], FILE * f)
{
	uint32_t i;
	for (i = 0; i < (*n); ++ i)
	{
		int r = fscanf (f, "%u%u%u%u", t + i, o + i, l + i, b + i);
		if (r == EOF) {break;}
	}
	(*n) = i;
}


void draw_track 
(
	cv::Mat & img, 
	uint32_t nb, 
	float x [], 
	float y [], 
	uint32_t nt, 
	uint32_t f [], 
	uint32_t o [], 
	uint32_t b [], 
	uint32_t l [], 
	uint32_t fi,
	uint32_t * from
)
{
	uint32_t i;
	//for (i = 0; i < nt; ++ i)
	for (i = *from; i < nt; ++ i)
	{
		if (f [i] == fi)
		{
			cv::Point p1 (x [b [i]], y [b [i]]);
			cv::Point p2 (x [o [i]], y [o [i]]);
			cv::circle (img, p1, 10, cv::Scalar (255, 0, 255), 2);
			char buf [10];
			snprintf (buf, 10, "%lu", (long unsigned)l [i]);
			cv::putText (img, buf, p2, CV_FONT_HERSHEY_TRIPLEX, 1.2, cv::Scalar (0,255,255));
			cv::arrowedLine (img, p2, p1, cv::Scalar (255,255,0), 2);
		}
		if (f [i] > fi) {break;}
		//if (o [i] == b [i] && fi > f [i]) {break;}
	}
	*from = i;
}


int main (int argc, char** argv)
{
	cv::VideoCapture capr ("data/c1_20180626_190007.mp4");
	cv::VideoWriter capw ("test.avi", cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), 10, cv::Size (1920,1080), true);
	//cv::VideoWriter capw ("test.avi", -1, 10, cv::Size (1920,1080), true);
	FILE * fb = fopen ("data/c1_20180626_190007_b1.txt", "r");
	//FILE * ft = fopen ("data/c1_20180626_190007_folb1.txt", "r");
	FILE * ft = popen ("sort -k 1,1 -n data/c1_20180626_190007_folb1.txt", "r");
	ASSERT (fb != NULL);
	ASSERT (ft != NULL);
	
	uint32_t nb = 10000;
	float * t = (float *)malloc (sizeof (float) * nb);
	float * x = (float *)malloc (sizeof (float) * nb);
	float * y = (float *)malloc (sizeof (float) * nb);
	read_blob (&nb, t, x, y, fb);
	
	uint32_t nt = 10000;
	uint32_t * f = (uint32_t *)malloc (sizeof (float) * nt);
	uint32_t * o = (uint32_t *)malloc (sizeof (float) * nt);
	uint32_t * l = (uint32_t *)malloc (sizeof (float) * nt);
	uint32_t * b = (uint32_t *)malloc (sizeof (float) * nt);
	read_track (&nt, f, o, l, b, ft);
	
	cv::namedWindow ("test", CV_WINDOW_NORMAL);
	cv::resizeWindow ("test", 1920, 1080);
	
	cv::Mat img (capr.get (CV_CAP_PROP_FRAME_WIDTH), capr.get (CV_CAP_PROP_FRAME_HEIGHT), CV_8UC3);
	cv::imshow ("test", img);
	int32_t fi;
	uint32_t flags = PAUSE;
	uint32_t from = 0;
	for (;;)
	{
		get_flags (&flags);
		if (flags & QUIT) {break;}
		if (flags & PAUSE) {continue;}
		if (flags & SLOW) {sleep (1);}
		if (flags & BACK) {capr.set (cv::CAP_PROP_POS_FRAMES, MAX (fi-10, 1));}
		flags |= capr.read (img) ? 0 : QUIT;
		fi = capr.get (cv::CAP_PROP_POS_FRAMES);
		fprintf (stderr, "\rProcessing frame: %lu", (long unsigned)fi);
		draw_track (img, nb, x, y, nt, f, o, b, l, fi, &from);
		cv::imshow ("test", img);
		capw.write (img);
	}
	
	capr.release ();
	//capw.release ();
	
	return 0;
}

