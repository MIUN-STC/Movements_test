#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>

#include "debug.h"

#define SPACEBAR 32
#define ESCAPE 27

#define QUIT 1U << 1
#define PAUSE 1U << 2

void show (char const * name, cv::Mat const & image)
{
	cv::Mat dst;
	cv::resize (image, dst, cv::Size (1920, 1080));
	cv::imshow (name, dst);
}


void copy_channel (cv::Mat const & x, cv::Mat& y, int i)
{
	int m [] = {0, i};
	cv::mixChannels (&x, 1, &y, 1, m, 1);
}


void paint_circles (cv::Mat const & frame, std::vector <cv::KeyPoint> const & kp)
{
	for (size_t i = 0; i < kp.size (); i = i + 1)
	{
		//if (kp [i].pt.y > 2000.0f) {continue;}
		cv::circle (frame, kp [i].pt, 30.0f, cv::Scalar (0, 255, 0), 2);
	}
}


void paint_text (cv::Mat const & frame, std::vector <cv::KeyPoint> const & kp)
{
	for (size_t i = 0; i < kp.size (); i = i + 1)
	{
		//if (kp [i].pt.y > 2000.0f) {continue;}
		char buf [100];
		snprintf (buf, 100, "%05.02f", (double) kp [i].size);
		cv::Point2f offset (0.0f, 60.0f);
		int fontFace = CV_FONT_HERSHEY_SCRIPT_SIMPLEX;
		cv::putText (frame, buf, kp [i].pt + offset, fontFace, 1, cv::Scalar (0, 0, 255), 3, 8);
	}
}


void visual 
(
	cv::Mat & out,
	cv::Mat const & mask, 
	cv::Mat const & frame, 
	std::vector <cv::KeyPoint> const & kp
)
{
	cv::Mat m (mask.rows, mask.cols, CV_8UC1);
	//out.setTo (cv::Scalar (0, 0, 255));
	//cv::bitwise_not (mask, m);
	//frame.copyTo (out, m);
	frame.copyTo (out);
	paint_circles (out, kp);
	paint_text (out, kp);
}


void get_flags (uint32_t * flags)
{
	int c = cv::waitKey (3);
	if (c == 'q' || c == 'Q' || (c & 255) == ESCAPE) 
	{
		(*flags) |= QUIT;
	}
	if (c == SPACEBAR) 
	{
		(*flags) ^= PAUSE;
	}	
}


enum OutputFormat
{
	POI_TXY,
	POI_TXYDN
};


void fwrite_poi 
(
	FILE * f, 
	int t, 
	std::vector <cv::KeyPoint> const & kp, 
	enum OutputFormat format
)
{
	for (size_t i = 0; i < kp.size (); i = i + 1)
	{
		switch (format)
		{
			case POI_TXY:
			fprintf (f, "%e %e %e\n", (double)t, (double)kp [i].pt.x, (double)kp [i].pt.y);
			break;
			case POI_TXYDN:
			fprintf (f, "%6i %e %e %e %6i\n", t, kp [i].pt.x, kp [i].pt.y, kp [i].size, (int)kp.size ());
			break;	
		}
		
	}
}
