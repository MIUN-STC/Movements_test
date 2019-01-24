#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>

#include <opencv2/xfeatures2d.hpp>

#include "option.h"
#include "debug.h"
#include "app.h"





int main (int argc, char** argv)
{
	struct Option opt;
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	if (opt.mode & OPT_HELP) {opt_help (&opt);return 0;}
	if (opt.src == NULL) {return 0;}


	FILE * fout = NULL;
	if (opt.des) {fout = fopen (opt.des, "w+");}
	else {fout = stdout;}
	ASSERT (fout);

	cv::VideoCapture cap (opt.src);
	double w = cap.get (cv::CAP_PROP_FRAME_WIDTH);
	double h = cap.get (cv::CAP_PROP_FRAME_HEIGHT);
	
	cv::namedWindow ("d");
	
	cv::Mat f0 (h, w, CV_8UC3);
	cv::Mat f1 (h, w, CV_8UC3);
	cv::Mat v (h, w, CV_8UC3);
	cv::Mat mask (h, w, CV_8UC1);
	
	cv::Ptr<cv::BackgroundSubtractorKNN> bgfs;
	bgfs = cv::createBackgroundSubtractorKNN ();
	bgfs->setHistory (100);
	bgfs->setDist2Threshold (2000.0);
	bgfs->setDetectShadows (false);
	
	
	cv::SimpleBlobDetector::Params blobparams;
	blobparams.minDistBetweenBlobs = 20;
	blobparams.minRepeatability = 2;
	blobparams.minThreshold = 200;
	blobparams.maxThreshold = 255;
	blobparams.filterByArea = false;
	blobparams.minArea = 40;
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
	
	int morph_elem = cv::MorphShapes::MORPH_ELLIPSE;
	int morph_size = 4;
	cv::Mat element = cv::getStructuringElement (morph_elem, cv::Size (2*morph_size + 1, 2*morph_size+1), cv::Point (morph_size, morph_size) );
	
	uint32_t flags = 0;
	
	for (;;)
	{
		uint32_t i = cap.get (cv::CAP_PROP_POS_FRAMES);
		if (opt.mode & OPT_VERBOSITY) {fprintf (stderr, "\rProcessing frame: %lu", (long unsigned)i);}
		if ((opt.n != 0) && (i >= opt.n)) {break;}
		
		get_flags (&flags);
		if (flags & PAUSE) {continue;}
		flags |= cap.read (f0) ? 0 : QUIT;
		if (flags & QUIT) {break;}

		
		
		cv::morphologyEx (f0, f1, cv::MORPH_GRADIENT, element);
		bgfs->apply (f1, mask);
		blobber->detect (mask, kp);
		visual (v, mask, f1, kp);
		show ("d", v);
		//printf ("%i\n", (int) cap.get (cv::CAP_PROP_POS_FRAMES));
		if (fout)
		{
			fwrite_poi (fout, (int) cap.get (cv::CAP_PROP_POS_FRAMES), kp, POI_TXYDN);
		}
	}
	
	fclose (fout);
	
	return 0;
}

