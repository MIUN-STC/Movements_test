#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>
#include <iostream>




void show (char const * name, cv::Mat const & image)
{
	cv::Mat dst;
	cv::resize (image, dst, cv::Size (400, 300));
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
	bgfs1->setDist2Threshold (400.0);
	bgfs2->setDist2Threshold (400.0);
	bgfs1->setDetectShadows (true);
	bgfs2->setDetectShadows (true);
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
	
	cv::namedWindow ("FG1", cv::WINDOW_NORMAL);
	cv::namedWindow ("FG2", cv::WINDOW_NORMAL);
	

	bool play = false;
	
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

		bgfs1->apply (frame1, fgmask1, 0.025);
		bgfs2->apply (frame2, fgmask2, 0.025);
		
		show ("FG1", fgmask1);
		show ("FG2", fgmask2);
		
	}


	return 0;
}

