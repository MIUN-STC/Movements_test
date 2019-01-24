#include <stdio.h>
#include <opencv2/videoio/videoio_c.h>

int main(int argc, char** argv)
{
	CvCapture* capture = cvCreateFileCapture("c1_20180627_200007.mp4");
	IplImage* frame;
	while(1)
	{
		//frame = cvQueryFrame (capture);
		//printf ("%i\n", frame->width);
	}
}
