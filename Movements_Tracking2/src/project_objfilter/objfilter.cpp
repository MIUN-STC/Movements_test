#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>
#include <iostream>

#include "poi.h"
#include "common.h"

void show (cv::Mat const & image)
{
	cv::Mat dst;
	cv::resize (image, dst, cv::Size (1920, 1080));
	cv::imshow ("POI show", dst);
}


void draw1 (cv::Mat & image, struct POI const * x, struct POI const * e)
{
	while (1)
	{
		if (x >= e) {break;}
		cv::Point2f p;
		p.x = x->p [0];
		p.y = x->p [1];
		cv::circle (image, p, 10.0f, cv::Scalar (0, 0, 255), 1);
		x++;
	}
}




int main(int argc, char** argv)
{
	cv::namedWindow ("POI show", cv::WINDOW_NORMAL);

	struct POI poi1_data [10000];
	struct POI poi2_data [10000];
	FILE * poi1_file = fopen ("poi1.txt", "r");
	FILE * poi2_file = fopen ("poi2.txt", "r");
	ASSERT (poi1_file != NULL);
	ASSERT (poi2_file != NULL);
	int poi1_n = poi_load (poi1_file, poi1_data, COUNTOF (poi1_data));
	int poi2_n = poi_load (poi2_file, poi2_data, COUNTOF (poi2_data));
	struct POI const * poi1_end = poi1_data + poi1_n;
	struct POI const * poi2_end = poi2_data + poi2_n;
	printf ("poi1_n: %i\n", poi1_n);
	printf ("poi2_n: %i\n", poi2_n);
	fclose (poi1_file);
	fclose (poi2_file);
	
	cv::VideoCapture cap;
	cap.open ("/home/sansa/cv_test1/c1_20180626_210007.mp4");
	ASSERT (cap.isOpened ());
	double w = cap.get (cv::CAP_PROP_FRAME_WIDTH);
	double h = cap.get (cv::CAP_PROP_FRAME_HEIGHT);
	cv::Mat frame (h, w, CV_8UC3);
	
	bool play = false;
	
	for (;;)
	{
		int c = cv::waitKey (200);
		if (c == 'q' || c == 'Q' || (c & 255) == 27) {break;}
		if (c == 32) {play = !play;}
		if (!play) {continue;};
		
		cap >> frame;
		int32_t t = cap.get (cv::CAP_PROP_POS_FRAMES);
		draw1 (frame, poi1_data + 4000, poi1_end);
		show (frame);
	}

quit:
	

	return 0;
}

