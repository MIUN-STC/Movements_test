#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>

#include <opencv2/xfeatures2d.hpp>

#include "option.h"
#include "debug.h"
#include "app.h"


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


void blurrington (cv::Mat & mask)
{
	cv::Mat m (mask.rows, mask.cols, mask.type ());
	cv::blur (mask, m, cv::Size (10, 10));
	cv::add (mask, m, mask);
}


int main (int argc, char** argv)
{
	struct Option opt;
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	if (opt.mode & OPT_HELP) {opt_help (&opt);return 0;}
	if (opt.src == NULL) {return 0;}
	
	cv::VideoCapture cap (opt.src);
	double w = cap.get (cv::CAP_PROP_FRAME_WIDTH);
	double h = cap.get (cv::CAP_PROP_FRAME_HEIGHT);
	
	//cv::namedWindow ("f0");
	cv::namedWindow ("d");
	
	cv::Mat f0 (h, w, CV_8UC3);
	cv::Mat f1 (h, w, CV_8UC3);
	cv::Mat v (h, w, CV_8UC3);
	cv::Mat mask (h, w, CV_8UC1);
	
	cv::Ptr<cv::BackgroundSubtractorKNN> bgfs;
	cv::Ptr<cv::BackgroundSubtractorKNN> bgfs2;
	//cv::Ptr<cv::BackgroundSubtractorMOG2> bgfs;
	bgfs = cv::createBackgroundSubtractorKNN ();
	bgfs2 = cv::createBackgroundSubtractorKNN ();
	//bgfs = cv::createBackgroundSubtractorMOG2 ();
	bgfs->setHistory (100);
	bgfs->setDist2Threshold (500.0);
	//bgfs->setVarThreshold (80.0);
	bgfs->setDetectShadows (false);
	
	
	cv::SimpleBlobDetector::Params blobparams;
	
	blobparams.minDistBetweenBlobs = 20;
	blobparams.minRepeatability = 2;
	
	blobparams.minThreshold = 60;
	blobparams.maxThreshold = 255;
	
	blobparams.filterByArea = false;
	blobparams.minArea = 20;
	blobparams.maxArea = 10000;
	
	blobparams.filterByColor = false;
	blobparams.blobColor = 255;
	
	blobparams.filterByCircularity = false;
	blobparams.minCircularity = 0.7;
	blobparams.maxCircularity = 1.0;
	
	blobparams.filterByConvexity = false;
	blobparams.minConvexity = 0.4;
	
	blobparams.filterByInertia = false;
	blobparams.minInertiaRatio = 0.3;
	
	cv::Ptr<cv::SimpleBlobDetector> blobber = cv::SimpleBlobDetector::create (blobparams);
	std::vector <cv::KeyPoint> kp;
	
	//cv::Ptr<cv::xfeatures2d::SIFT> b = cv::xfeatures2d::SIFT::create (100);
	
	int morph_elem = cv::MorphShapes::MORPH_ELLIPSE;
	//int morph_elem = cv::MorphShapes::MORPH_RECT;
	//int morph_elem = cv::MorphShapes::MORPH_CROSS;
	int morph_size = 4;
	cv::Mat element = cv::getStructuringElement (morph_elem, cv::Size (2*morph_size + 1, 2*morph_size+1), cv::Point (morph_size, morph_size) );
	
	uint32_t flags = 0;
	
	//cap.set (cv::CAP_PROP_POS_FRAMES, 350);
	
	FILE * fout = fopen ("c2_20180627_090006.txt", "w");
	ASSERT (fout != NULL);
	
	for (;;)
	{
		get_flags (&flags);
		//TRACE_F ("%x", flags);
		if (flags & PAUSE) {continue;}
		flags |= cap.read (f0) ? 0 : QUIT;
		if (flags & QUIT) {break;}
		
		//cv::morphologyEx (f0, f1, cv::MORPH_OPEN, element);
		cv::morphologyEx (f0, f1, cv::MORPH_GRADIENT, element);
		
		bgfs->apply (f1, mask, 0.1);
		//blurrington (mask);
		//bgfs2->apply (mask, mask, 0.1);
		//cv::blur (mask, mask, cv::Size (100, 100));
		//cv::morphologyEx (mask, mask, cv::MORPH_CLOSE, element);
		//cv::blur (mask, mask, cv::Size (10, 10));
		//bgfs->apply (mask, mask, 0.1);
		//cv::morphologyEx (mask, mask, cv::MORPH_OPEN, element);
		blobber->detect (mask, kp);
		//cv::Mat desc;
		//b->detectAndCompute (mask, cv::Mat(), kp, desc);
		visual (v, mask, f0, kp);

		show ("d", v);
		
		printf ("%i\n", (int) cap.get (cv::CAP_PROP_POS_FRAMES));
		
		put_poi (fout, (int) cap.get (cv::CAP_PROP_POS_FRAMES), kp);
	}
	quit:
	
	fclose (fout);
	
	return 0;
}

