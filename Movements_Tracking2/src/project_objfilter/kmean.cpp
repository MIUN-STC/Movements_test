#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "debug.h"

using namespace cv;
using namespace std;




int poi_load1 (FILE * f, std::vector <cv::Point3f> & coords, size_t max)
{
	//TRACE_F ("%i\n", n);
	typedef unsigned long lu;
	int i;
	for (i = 0; i < max; i = i + 1)
	{
		char c;
		lu m;
		lu g;
		lu t;
		cv::Point3f p;
		int r = fscanf (f, "%ld %ld %ld %f %f %c", &m, &g, &t, &(p.x), &(p.y), &c);
		p.z = t * 0.05f;
		if (r == EOF) {return i;}
		coords.push_back (p);
	}
	return i;
}



void show (cv::Mat const & image)
{
	cv::Mat dst;
	cv::resize (image, dst, cv::Size (1920, 1080));
	cv::imshow ("show", dst);
}





int main( int /*argc*/, char** /*argv*/ )
{
	cv::VideoCapture cap;
	cap.open ("/home/sansa/cv_test1/c1_20180626_210007.mp4");
	ASSERT (cap.isOpened ());
	double w = cap.get (cv::CAP_PROP_FRAME_WIDTH);
	double h = cap.get (cv::CAP_PROP_FRAME_HEIGHT);
	cv::Mat frame (h, w, CV_8UC3);
	cap >> frame;
	
	std::vector <cv::Point3f> p;
	FILE * f = fopen ("poi1.txt", "r");
	poi_load1 (f, p, 10000);
	fclose (f);

	
	for (size_t i = 0; i < p.size (); i = i + 1)
	{
		//printf ("(%f %f %f)\n", p [i].x, p [i].y, p [i].z);
	}
	cv::Mat labels;
	cv::Mat centers;
	int K = 18;
	TermCriteria termcrit (TermCriteria::EPS + TermCriteria::COUNT, 200, 0.001);
	cv::Scalar colorTab [] =
	{
		cv::Scalar (0x00, 0x00, 0x00),
		cv::Scalar (0x00, 0x00, 0xFF),
		cv::Scalar (0x00, 0xFF, 0x00),
		cv::Scalar (0x00, 0xFF, 0xFF),
		cv::Scalar (0xFF, 0x00, 0x00),
		cv::Scalar (0xFF, 0xFF, 0xFF),
		
		cv::Scalar (0xA0, 0xA0, 0xA0),
		cv::Scalar (0xA0, 0xA0, 0xFF),
		cv::Scalar (0xA0, 0xFF, 0xA0),
		cv::Scalar (0xA0, 0xFF, 0xFF),
		cv::Scalar (0xFF, 0xA0, 0xA0),
		cv::Scalar (0xFF, 0xFF, 0xFF),

		cv::Scalar (0x00, 0x00, 0x00),
		cv::Scalar (0x00, 0x00, 0xA0),
		cv::Scalar (0x00, 0xA0, 0x00),
		cv::Scalar (0x00, 0xA0, 0xA0),
		cv::Scalar (0xA0, 0x00, 0x00),
		cv::Scalar (0xA0, 0xA0, 0xA0)
	};
    
	cv::kmeans (p, K, labels, termcrit, 40, cv::KMEANS_PP_CENTERS, centers);
	//printf ("%i %i\n", centers.rows, centers.cols);
	
	for (int i = 0; i < p.size (); i = i + 1)
	{
		int clusterIdx = labels.at<int>(i);
		//printf ("clusterIdx %i\n", clusterIdx);
		cv::Point2f ipt (p [i].x, p [i].y);
		cv::circle (frame, ipt, 10, colorTab [clusterIdx]);
	}
	
	show (frame);
	int key = cv::waitKey ();
	
	
	
	
	
	
	
	
	//if (key == 27 || key == 'q' || key == 'Q'  {return;}
	
	/*
    const int MAX_CLUSTERS = 5;
    Scalar colorTab[] =
    {
        Scalar(0, 0, 255),
        Scalar(0,255,0),
        Scalar(255,100,100),
        Scalar(255,0,255),
        Scalar(0,255,255)
    };
    Mat img(500, 500, CV_8UC3);
    RNG rng(12345);
    for(;;)
    {
        int k, clusterCount = rng.uniform(2, MAX_CLUSTERS+1);
        int i, sampleCount = rng.uniform(1, 1001);
        //Mat points(sampleCount, 1, CV_32FC2), labels;
        clusterCount = MIN(clusterCount, sampleCount);
        Mat centers;
        for( k = 0; k < clusterCount; k++ )
        {
            Point center;
            center.x = rng.uniform(0, img.cols);
            center.y = rng.uniform(0, img.rows);
            Mat pointChunk = points.rowRange(k*sampleCount/clusterCount,
                                             k == clusterCount - 1 ? sampleCount :
                                             (k+1)*sampleCount/clusterCount);
            rng.fill(pointChunk, RNG::NORMAL, Scalar(center.x, center.y), Scalar(img.cols*0.05, img.rows*0.05));
        }
        randShuffle(points, 1, &rng);
        kmeans(points, clusterCount, labels,
            TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),
               3, KMEANS_PP_CENTERS, centers);
        img = Scalar::all(0);
        for( i = 0; i < sampleCount; i++ )
        {
            int clusterIdx = labels.at<int>(i);
            Point ipt = points.at<Point2f>(i);
            circle( img, ipt, 2, colorTab[clusterIdx], FILLED, LINE_AA );
        }
        imshow("clusters", img);
        char key = (char)waitKey();
        if( key == 27 || key == 'q' || key == 'Q' ) // 'ESC'
            break;
    }
    */
    
    return 0;
}
