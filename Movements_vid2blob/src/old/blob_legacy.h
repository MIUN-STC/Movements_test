#pragma once

#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>


void store_blobcoord (FILE * f, float t, std::vector <cv::KeyPoint> & p)
{
	for (size_t i = 0; i < p.size (); i = i + 1)
	{
		fprintf (f, "%e %e %e\n", (float)t, (float)p [i].pt.x, (float)p [i].pt.y);
	}
}


#define VPC_VIDEO_END 1 << 0

//Video to point of interset converter
struct VPC
{
	int flags;
	char const * filename;
	cv::VideoCapture cap;
	cv::Mat frame;
	cv::Mat fgmask;
	cv::Ptr<cv::BackgroundSubtractorKNN> bgfs;
	cv::SimpleBlobDetector::Params blobparams;
	cv::Ptr<cv::SimpleBlobDetector> blobber;
	std::vector <cv::KeyPoint> kp;
	float frameid;
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
	double w = vpc.cap.get (cv::CAP_PROP_FRAME_WIDTH);
	double h = vpc.cap.get (cv::CAP_PROP_FRAME_HEIGHT);
	//double f = cap.get (cv::CAP_PROP_FORMAT);
	vpc.frame = cv::Mat (h, w, CV_8UC3);
	vpc.fgmask = cv::Mat (h, w, CV_8UC1);
	
	vpc.bgfs = cv::createBackgroundSubtractorKNN ();
	if (!vpc.bgfs)
	{
		fprintf (stderr, "Can not create a backgrounder.\n");
		return -1;
	}
	vpc.bgfs->setHistory (300);
	vpc.bgfs->setDist2Threshold (2000.0);
	vpc.bgfs->setDetectShadows (false);
	
	for (size_t i = 0; i < 20; i = i + 1)
	{
		vpc.cap >> vpc.frame;
		vpc.bgfs->apply (vpc.frame, vpc.fgmask, 1.0);
	}
	
	
	vpc.blobparams.minDistBetweenBlobs = 40;
	vpc.blobparams.minRepeatability = 2;
	vpc.blobparams.minThreshold = 100;
	vpc.blobparams.maxThreshold = 255;
	vpc.blobparams.filterByArea = false;
	vpc.blobparams.minArea = 5;
	vpc.blobparams.maxArea = 1000;
	vpc.blobparams.filterByColor = false;
	vpc.blobparams.blobColor = 255;
	vpc.blobparams.filterByCircularity = false;
	vpc.blobparams.minCircularity = 0.1;
	vpc.blobparams.filterByConvexity = false;
	vpc.blobparams.minConvexity = 0.87;
	vpc.blobparams.filterByInertia = false;
	vpc.blobparams.minInertiaRatio = 0.01;
	
	vpc.blobber = cv::SimpleBlobDetector::create (vpc.blobparams);
	if (!vpc.blobber)
	{
		fprintf (stderr, "Can not create a blobber.\n");
		return -1;
	}
	
}


void vpc_update (struct VPC & vpc)
{
	vpc.cap >> vpc.frame;
	if (vpc.frame.empty ()) 
	{
		vpc.flags |= VPC_VIDEO_END;
		return;
	}
	vpc.frameid = vpc.cap.get (cv::CAP_PROP_POS_FRAMES);
	//float frameid = vpc.cap.get (cv::CAP_PROP_POS_FRAMES);
	//if (frameid > 400) {break;}
	
	vpc.bgfs->apply (vpc.frame, vpc.fgmask); //, 0.01
	vpc.blobber->detect (vpc.fgmask, vpc.kp);
	//masker (fgmask, auto_mask1);
	//auto_mask1.convertTo (auto_mask2, auto_mask2.type ());
	//copy_channel (fgmask, frame, 2);
	//for (size_t i = 0; i < kp.size (); i = i + 1)
	{
		//cv::circle (frame, kp [i].pt, 20.0f, cv::Scalar (0, 255, 0), 2);
		//fprintf (f, "%e %e %e\n", (float)frameid, (float)kp [i].pt.x, (float)kp [i].pt.y);
	}
	//show (frame);


	//int c = cv::waitKey(3);
	//if (c == 'q' || c == 'Q' || (c & 255) == 27)
	//break;
}
