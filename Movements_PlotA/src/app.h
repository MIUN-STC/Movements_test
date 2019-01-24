#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>

#include "debug.h"

#define SPACEBAR 32
#define ESCAPE 27

#define QUIT 1U << 1
#define PAUSE 1U << 2
#define BACK 1U << 3
#define SLOW 1U << 4



void get_flags (uint32_t * flags)
{
	(*flags) &= ~BACK;
	int c = cv::waitKey (100);
	if (c == 'q' || c == 'Q' || (c & 255) == ESCAPE) 
	{
		(*flags) |= QUIT;
	}
	if (c == SPACEBAR) 
	{
		(*flags) ^= PAUSE;
	}
	if (c == 'b' && (*flags) & SLOW)
	{
		(*flags) &= ~SLOW;
	}
	else if (c == 'b') 
	{
		(*flags) |= BACK;
		(*flags) |= SLOW;
	}
}

