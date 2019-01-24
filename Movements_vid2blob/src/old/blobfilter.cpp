#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>

#include "option.h"
#include "blobFilter.h"

void show (char const * name, cv::Mat const & image)
{
	cv::Mat dst;
	cv::resize (image, dst, cv::Size (960, 540));
	cv::imshow (name, dst);
}

int main (int argc, char** argv)
{
	struct Option opt;
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	if (opt.mode & OPT_HELP) {opt_help (&opt);return 0;}
	if (opt.src == NULL) {return 0;}
	FILE * f = NULL;
	if (strcmp (opt.des, "stdout") == 0)
	{
		f = stdout;
	}
	else if (opt.des != NULL)
	{
		f = fopen (opt.des, "r");
	}
	
	struct VPC vpc;
	vpc.filename = opt.src;
	vpc_init (vpc);
	
	//for (;;)
	{
		//int c = cv::waitKey(3);
		//if (c == 'q' || c == 'Q' || (c & 255) == 27) {break;}
		
		fprintf (stderr, "\n Video frame has X: %f and Y: %f pixels\n", vpc.w, vpc.h);
		//if (vpc.flags & VPC_VIDEO_END) {break;}

		if (f)
		{
			read_blobcoord (f, vpc.image);
		}
		cv::imshow ("blob viewer 0", vpc.image);
		//int c = cv::waitKey();
		show ("blob viewer", vpc.image);
		int c1 = cv::waitKey();
	}
	
	if (f != NULL) {fclose (f);}
	return 0;
}

