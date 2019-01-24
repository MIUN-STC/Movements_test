#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>

#include "option.h"
#include "app.h"

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

	cv::Mat currentI;
	cv::Mat currentIbl;
	cv::Mat previousI;
	cv::Mat backgroundI;
	cv::Mat currentPI;
	cv::Mat currentPBG;
	cv::Mat processedI;
	cv::Mat Gray;
	cv::Mat thresholdedI;
	cv::Mat openI;
	cv::Mat closeI;
	const int ks = 3;
	const int th = 45; // Subtract: 40 - 50, Multiply: 20 
	const int ms = 5;
	cv::Mat element;

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

	vpc.currentI = cv::Mat (h, w, CV_8UC3);
	vpc.currentIbl = cv::Mat (h, w, CV_8UC3);
	vpc.previousI = cv::Mat (h, w, CV_8UC3);
	vpc.backgroundI = cv::Mat (h, w, CV_8UC3);
	vpc.currentPI = cv::Mat (h, w, CV_8UC3);
	vpc.currentPBG = cv::Mat (h, w, CV_8UC3);
	vpc.processedI = cv::Mat (h, w, CV_8UC3);
	vpc.Gray = cv::Mat (h, w, CV_8UC1);
	vpc.thresholdedI = cv::Mat (h, w, CV_8UC1);
	vpc.openI = cv::Mat (h, w, CV_8UC1);
	vpc.closeI = cv::Mat (h, w, CV_8UC1);
	vpc.element = cv::getStructuringElement(0, cv::Size(2*vpc.ms + 1, 2*vpc.ms+1), cv::Point(vpc.ms, vpc.ms) );
	
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
		if (vpc.frame.empty ()) 
		{
			vpc.flags |= VPC_VIDEO_END;
			return -1;
		}
		vpc.frame.copyTo(vpc.currentI);
		cv::medianBlur(vpc.currentI, vpc.currentIbl, vpc.ks);
		cv::absdiff(vpc.previousI, vpc.currentIbl, vpc.currentPI);
		cv::absdiff(vpc.previousI, vpc.backgroundI, vpc.currentPBG);

		cv::subtract(vpc.currentPI, vpc.currentPBG, vpc.processedI);
		//cv::multiply(vpc.currentPI, vpc.currentPBG, vpc.processedI, 0.005);
		cv::cvtColor(vpc.processedI, vpc.Gray, cv::COLOR_BGR2GRAY);
		cv::threshold(vpc.Gray, vpc.thresholdedI, vpc.th,255,0);
		cv::morphologyEx( vpc.thresholdedI, vpc.closeI, 0, vpc.element );
		cv::morphologyEx( vpc.closeI, vpc.openI, 1, vpc.element );

		vpc.bgfs->apply (vpc.closeI, vpc.fgmask, -1); //, 0.01  // Johan: 

		cv::addWeighted(vpc.previousI, 0.1, vpc.backgroundI, 0.9, 0.0, vpc.backgroundI);
		//vpc.previousI.copyTo(vpc.backgroundI);
		vpc.currentIbl.copyTo(vpc.previousI);
	}
	
	
	vpc.blobparams.minDistBetweenBlobs = 40;
	vpc.blobparams.minRepeatability = 2;
	vpc.blobparams.minThreshold = 60;  // Johan: 100
	vpc.blobparams.maxThreshold = 255;
	vpc.blobparams.filterByArea = false;  // Johan: false
	vpc.blobparams.minArea = 50;
	vpc.blobparams.maxArea = 20000;
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
	
	vpc.frame.copyTo(vpc.currentI);
	cv::medianBlur(vpc.currentI, vpc.currentIbl, vpc.ks);
	cv::absdiff(vpc.previousI, vpc.currentIbl, vpc.currentPI);
	cv::absdiff(vpc.previousI, vpc.backgroundI, vpc.currentPBG);

	cv::subtract(vpc.currentPI, vpc.currentPBG, vpc.processedI); 
	//cv::multiply(vpc.currentPI, vpc.currentPBG, vpc.processedI, 0.005);
	cv::cvtColor(vpc.processedI, vpc.Gray, cv::COLOR_BGR2GRAY);
	cv::threshold(vpc.Gray, vpc.thresholdedI, vpc.th,255,0);
	cv::morphologyEx( vpc.thresholdedI, vpc.closeI, 0, vpc.element );
	cv::morphologyEx( vpc.closeI, vpc.openI, 1, vpc.element );

	vpc.bgfs->apply (vpc.thresholdedI, vpc.fgmask, -1); //, 0.01  // Johan: 
	vpc.blobber->detect (vpc.fgmask, vpc.kp);

	cv::addWeighted(vpc.previousI, 0.1, vpc.backgroundI, 0.9, 0.0, vpc.backgroundI);
	//vpc.previousI.copyTo(vpc.backgroundI);
	vpc.currentIbl.copyTo(vpc.previousI);
}


int main (int argc, char** argv)
{
	struct Option opt;
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	if (opt.mode & OPT_HELP) {opt_help (&opt);return 0;}
	if (opt.src == NULL) {return 0;}
	
	FILE * f = NULL;
	if (opt.des && strcmp (opt.des, "stdout") == 0)
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
	
	uint32_t flags = 0;
	
	for (;;)
	{
		get_flags (&flags);
		if (flags & PAUSE) {continue;}
		if (flags & QUIT) {break;}
		

		vpc_update (vpc);
		fprintf (stderr, "\rProcessing frame: %f", vpc.frameid);
		if (vpc.flags & VPC_VIDEO_END) {break;}
		if ((opt.n != 0) && (vpc.frameid > opt.n)) {break;}
		
		if (opt.des && f)
		{
			store_blobcoord (f, vpc.frameid, vpc.kp);
		}

		if (opt.v == 1)
		{
			//Put the background filter output frame on a single color channel.
			//This way we can both see the video source and the background filter output at the same time.
			int color_channel = 2;
			//copy_channel (vpc.fgmask, vpc.frame, color_channel);
			//paint_circles (vpc.frame, vpc.kp);
			visual (vpc.frame, vpc.fgmask, vpc.frame, vpc.kp);
			show ("frame viewer", vpc.frame);
			//show ("Gray viewer", vpc.Gray);
		}
		
	}
	
	if (f != NULL) {fclose (f);}


	return 0;
}
