#pragma once

#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>

#include "debug.h"


#define QUIT  0x0001
#define PAUSE 0x0002
#define ERROR 0x0004



void draw_circle (cv::Mat const & frame, std::vector <cv::KeyPoint> const & kp)
{
	for (size_t i = 0; i < kp.size (); i = i + 1)
	{
		//if (kp [i].pt.y > 2000.0f) {continue;}
		cv::circle (frame, kp [i].pt, 30.0f, cv::Scalar (0, 255, 0), 2);
	}
}


void draw_text (cv::Mat const & frame, std::vector <cv::KeyPoint> const & kp)
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


/*
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
*/
