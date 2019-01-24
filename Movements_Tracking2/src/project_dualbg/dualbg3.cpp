#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>
#include <iostream>




void show (cv::Mat const & image)
{
	cv::Mat dst;
	cv::resize (image, dst, cv::Size (1920, 1080));
	cv::imshow ("FG Segmentation", dst);
}


void copy_channel (cv::Mat const & x, cv::Mat& y, int i)
{
	int m [] = {0, i};
	cv::mixChannels (&x, 1, &y, 1, m, 1);
}

/*
struct Mat_Mix
{
	cv::Mat vid;
	cv::Mat fg;
	cv::Mat 
};
*/


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
	cv::Mat frame3 (h, w, CV_8UC3);
	cv::Mat fgmask1 (h, w, CV_8UC1);
	cv::Mat fgmask2 (h, w, CV_8UC1);
	cv::Mat fgmask3 (h, w, CV_8UC3);
	cv::Mat fgmask4 (h, w, CV_8UC3, cv::Scalar (0, 0, 0));

	

	
	cv::namedWindow ("FG Segmentation", cv::WINDOW_NORMAL);
	//resizeWindow ("FG Segmentation", 800, 600);

/*
	for (size_t i = 0; i < 100; i = i + 1)
	{
		cap1 >> frame1;
		cap2 >> frame2;
		bgfs1->apply (frame1, fgmask1, 1.0);
		bgfs2->apply (frame2, fgmask2, 1.0);
	}
	
	cap1.set (cv::CAP_PROP_POS_FRAMES, 0);
	
	show (fgmask1);
	cv::waitKey (-1);
	show (fgmask2);
	cv::waitKey (-1);
*/
	
	uint32_t frameid0 = 0;
	uint32_t frameid1;
	uint32_t frameid2;
	
	int morph_elem = 1;
	int morph_size = 4;
	int morph_operator = 2;
	int const max_operator = 4;
	int const max_elem = 2;
	int const max_kernel_size = 24;
	cv::Mat element = cv::getStructuringElement (morph_elem, cv::Size( 2*morph_size + 1, 2*morph_size+1 ), cv::Point( morph_size, morph_size ) );

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
		frameid0 ++;
		frameid1 = cap1.get (cv::CAP_PROP_POS_FRAMES);
		frameid2 = cap2.get (cv::CAP_PROP_POS_FRAMES);


		

		bgfs1->apply (frame1, fgmask1, 0.025);
		bgfs2->apply (frame2, fgmask2, 0.025);
		
		//cv::morphologyEx (fgmask1, fgmask1, morph_operator, element);
		//cv::morphologyEx (fgmask2, fgmask2, morph_operator, element);
		copy_channel (fgmask1, fgmask3, 1);
		copy_channel (fgmask2, fgmask3, 2);
		
		cv::subtract (frame2, cv::Scalar (100, 100, 100), frame2);
		cv::add (fgmask3, fgmask4, fgmask4);
		cv::add (frame2, fgmask4, frame3);
		cv::subtract (fgmask4, cv::Scalar (4, 4, 4), fgmask4);
		
		{
			char buf [10];
			snprintf (buf, sizeof (buf), "%i %i", (int)frameid1, (int)frameid2);
			cv::putText (frame3, buf, cv::Point (30,30), cv::FONT_HERSHEY_COMPLEX_SMALL, 2.0, cv::Scalar (200,200,250), 1, CV_AA); 
		}
		
		show (frame3);



	}


	return 0;
}

