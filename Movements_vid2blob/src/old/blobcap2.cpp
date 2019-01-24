#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>

#include "option.h"
#include "blob.h"



int main(int argc, char * argv [])
{
	struct Option opt;
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	if (opt.mode & OPT_HELP) {opt_help (&opt);return 0;}
	if (opt.src == NULL){return 0;}
	if (opt.des == NULL){return 0;}
	
	struct VPC vpc;
	vpc.filename = opt.src;
	vpc_init (vpc);
	
	size_t nblobs = 0;
	size_t const nframes = vpc.cap.get (cv::CAP_PROP_FRAME_COUNT);
	
	FILE * f = fopen (opt.des, "w+");
	ASSERT (f != NULL);
	
	for (;;)
	{
		vpc_update (vpc);
		if (vpc.flags & VPC_VIDEO_END) {break;}
		if ((opt.n != 0) && (vpc.frameid > opt.n)) {break;}
		store_blobcoord (f, vpc.frameid, vpc.kp);
		nblobs += vpc.kp.size ();
		printf ("\rCompletion: %f%% %i %i", (vpc.frameid / nframes) * 100.0f, (int)vpc.frameid, (int)nframes);
		fflush (stdout);
	}
	fclose (f);

	printf ("\nNumber of blobs found: %lu\n", (long unsigned)nblobs);


	return 0;
}
