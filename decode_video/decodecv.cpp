#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/opencv.hpp>


int main()
{
	cv::VideoCapture cap("c1_20180627_200007.mp4", cv::CAP_FFMPEG);
	CvCapture* capture = cvCreateFileCapture("c1_20180627_200007.mp4");
	if (!cap.isOpened())
	{
		printf ("Error opening video stream\n");
		return -1;
	}
	while (1)
	{
		cv::Mat Frame;
		if (!cap.read(Frame)){
			printf ("No Frame available\n");
			cv::waitKey();
		}
		cv::imshow("output", Frame);
		if (cv::waitKey(33) == 27) break;
	}
}
