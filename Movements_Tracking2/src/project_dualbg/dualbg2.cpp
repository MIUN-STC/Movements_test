#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>
#include <iostream>


void show (char const * name, cv::Mat const & image)
{
	cv::Mat dst;
	cv::resize (image, dst, cv::Size (1800, 1000));
	cv::imshow (name, dst);
}


void copy_channel (cv::Mat const & x, cv::Mat& y, int i)
{
	int m [] = {0, i};
	cv::mixChannels (&x, 1, &y, 1, m, 1);
}



int main(int argc, char** argv)
{
	cv::Ptr<cv::BackgroundSubtractorKNN> bgfs1;
	cv::Ptr<cv::BackgroundSubtractorKNN> bgfs2;
	bgfs1 = cv::createBackgroundSubtractorKNN ();
	bgfs2 = cv::createBackgroundSubtractorKNN ();
	bgfs1->setHistory (100);
	bgfs2->setHistory (100);
	bgfs1->setDist2Threshold (800.0);
	bgfs2->setDist2Threshold (800.0);
	bgfs1->setDetectShadows (false);
	bgfs2->setDetectShadows (false);
	if (!bgfs1)
	{
		return -1;
	}
	if (!bgfs2)
	{
		return -1;
	}
	
	cv::VideoCapture cap1;
	cv::VideoCapture cap2;
	cap1.open ("/home/sansa/cv_test1/c1_20180626_210007.mp4");
	cap2.open ("/home/sansa/cv_test1/c2_20180626_210007.mp4");


	if (!cap1.isOpened())
	{
		std::cerr << "Cannot read video. Try moving video file to sample directory." << std::endl;
		return -1;
	}
	if (!cap2.isOpened())
	{
		std::cerr << "Cannot read video. Try moving video file to sample directory." << std::endl;
		return -1;
	}
	
	double w = cap1.get (cv::CAP_PROP_FRAME_WIDTH);
	double h = cap1.get (cv::CAP_PROP_FRAME_HEIGHT);
	//double f = cap.get (cv::CAP_PROP_FORMAT);
	cv::Mat frame1 (h, w, CV_8UC3);
	cv::Mat frame2 (h, w, CV_8UC3);
	cv::Mat fgmask1 (h, w, CV_8UC1);
	cv::Mat fgmask2 (h, w, CV_8UC1);
	cv::Mat image_dual (h, w, CV_8UC3, cv::Scalar (0, 0, 0));
	
	cv::namedWindow ("1", cv::WINDOW_NORMAL);

	
	uint32_t frameid1;
	uint32_t frameid2;

	bool play = true;
	
	
	int morph_elem = cv::MORPH_ELLIPSE;
	int morph_size = 2;
	cv::Mat element = cv::getStructuringElement (morph_elem, cv::Size( 2*morph_size + 1, 2*morph_size+1 ), cv::Point( morph_size, morph_size ) );
	
	
	for (;;)
	{
		int c = cv::waitKey (200);
		if (c == 'q' || c == 'Q' || (c & 255) == 27) {break;}
		if (c == 32) {play = !play;}
		if (!play) {continue;};
		
		cap1 >> frame1;
		cap2 >> frame2;

		if (frame1.empty ()) {break;}
		if (frame2.empty ()) {break;}
		frameid1 = cap1.get (cv::CAP_PROP_POS_FRAMES);
		frameid2 = cap2.get (cv::CAP_PROP_POS_FRAMES);
		
		
		
		bgfs1->apply (frame1, fgmask1, -1);
		bgfs2->apply (frame2, fgmask2, -1);
		
		cv::dilate (fgmask1, fgmask1, element);
		cv::dilate (fgmask2, fgmask2, element);
		
		copy_channel (fgmask1, image_dual, 1);
		copy_channel (fgmask2, image_dual, 2);
		cv::add (frame2, image_dual, image_dual);

		show ("1", image_dual);
		image_dual = cv::Scalar (0, 0, 0);

	}


	return 0;
}

