#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <unistd.h>


//Common
#include "global.h"
#include "debug.h"

//App
#include "app.h"






int main (int argc, char** argv)
{
	cv::VideoCapture capr ("data/c1_20180626_190007.mp4");
	ASSERT (capr.isOpened ());
	

	cv::namedWindow ("test", CV_WINDOW_NORMAL);
	cv::resizeWindow ("test", 1920, 1080);
	
	cv::Mat img (capr.get (CV_CAP_PROP_FRAME_WIDTH), capr.get (CV_CAP_PROP_FRAME_HEIGHT), CV_8UC3);
	cv::imshow ("test", img);
	
	int32_t fi;
	uint32_t flags = PAUSE;
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
		cv::imshow ("test", img);
	}
	
	capr.release ();
	
	return 0;
}

