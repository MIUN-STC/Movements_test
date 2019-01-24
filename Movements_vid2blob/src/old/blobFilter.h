#pragma once

#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>

void read_blobcoord (FILE * f, cv::Mat & image)
{
	int r = 1;
	int temp = 0;
	float  t;
	float  x;
	float  y;
	wchar_t * c;
		//fprintf(stderr, "\n Got here!");
	while (r > 0)
	{
		//fprintf(stderr, "\n Got here AGAIN!\n");
		r = fscanf (f, "%f%f%f%C", &t, &x, &y, c);
		int pX = (int) x; int pY = (int) y;
		fprintf(stderr, "\n At time: %f, X: %f-%i and Y: %f-%i data: %i ",t,x,pX,y,pY,image.at<int>(pY,pX));
		temp = image.at<int>(pY,pX) + 50;
		image.at<int>(pY, pX) = temp; 
		//fprintf(stderr, "\n At time: %f, X: %f and Y: %f ", t, x, y);
	}
	fprintf(stderr, "\n Done reading data file! Displaying ...");
}


#define VPC_VIDEO_END 1 << 0

//Video to point of interset converter
struct VPC
{
	int flags;
	char const * filename;
	cv::VideoCapture cap;
	cv::Mat image;
	double h;
	double w;
};

int vpc_init (struct VPC & vpc)
{
	vpc.flags = 0;
	vpc.cap.open (vpc.filename);
	if (!vpc.cap.isOpened ())
	{
		fprintf (stderr, "Cannot read video %s. ", vpc.filename);
		fprintf (stderr, "Try moving video file to sample directory.\n");
		return -1;
	}
	vpc.w = vpc.cap.get (cv::CAP_PROP_FRAME_WIDTH);
	vpc.h = vpc.cap.get (cv::CAP_PROP_FRAME_HEIGHT);

	vpc.image = cv::Mat::zeros (vpc.h, vpc.w, CV_8UC1);	
}

