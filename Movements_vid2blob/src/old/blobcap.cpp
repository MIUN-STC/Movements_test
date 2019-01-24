#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>

#include "option.h"
#include "blob.h"

void show (char const * name, cv::Mat const & image)
{
	cv::Mat dst;
	cv::resize (image, dst, cv::Size (960, 540));
	cv::imshow (name, dst);
}


void copy_channel (cv::Mat const & x, cv::Mat& y, int i)
{
	int m [] = {0, i};
	cv::mixChannels (&x, 1, &y, 1, m, 1);
}

void paint_circles (cv::Mat const & frame, float t, std::vector <cv::KeyPoint> & kp)
{
	for (size_t i = 0; i < kp.size (); i = i + 1)
	{
		cv::circle (frame, kp [i].pt, 20.0f, cv::Scalar (0, 255, 0), 2);
	}
}

int main (int argc, char** argv)
{
	struct Option opt;
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	if (opt.mode & OPT_HELP) {opt_help (&opt);return 0;}
	if (opt.src == NULL) {return 0;}
	FILE * f = NULL;
	if (strcmp (opt.des, "stdout") == 0)
	{
		f = stdout;
	}
	else if (opt.des != NULL)
	{
		f = fopen (opt.des, "w+");
	}
	
	struct VPC vpc;
	vpc.filename = opt.src;
	vpc_init (vpc);
	
	cv::namedWindow ("frame viewer");
	
	for (;;)
	{
		int c = cv::waitKey(3);
		if (c == 'q' || c == 'Q' || (c & 255) == 27) {break;}
		
		vpc_update (vpc);
		fprintf (stderr, "\rProcessing frame: %f", vpc.frameid);
		if (vpc.flags & VPC_VIDEO_END) {break;}
		if ((opt.n != 0) && (vpc.frameid > opt.n)) {break;}
		
		
		//Put the background filter output frame on a single color channel.
		//This way we can both see the video source and the background filter output at the same time.
		int color_channel = 2;
		copy_channel (vpc.fgmask, vpc.frame, color_channel);
		
		paint_circles (vpc.frame, vpc.frameid, vpc.kp);
		if (f)
		{
			store_blobcoord (f, vpc.frameid, vpc.kp);
		}
		
		show ("frame viewer", vpc.frame);
		//show ("Gray viewer", vpc.Gray);
	}
	
	if (f != NULL) {fclose (f);}


	return 0;
}
