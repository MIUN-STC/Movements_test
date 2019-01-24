#define CAM_ASSERT_F ASSERT_F
#define CAM_TRACE_F TRACE_F

#include "option.h"
#include "debug.h"
#include "cam.h"
#include "cam_controller.h"
#include "common.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <assert.h>
#include <inttypes.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>


int main (int argc, char * argv [])
{
	struct Gev_Cam cam;
	struct Option opt;
	
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	
	if (opt.mode & OPT_HELP)
	{
		opt_help (&opt);
		return 0;
	}
	
	{
		int r;
		r = cam_open_serial (&cam, opt.sn);
		if (r) {goto main_error;}
		r = cam_load (&cam, opt.config_file);
		if (r) {goto main_error;}
		r = cam_allocate_frame_buffer (&cam, 4);
		if (r) {goto main_error;}
		r = cam_start (&cam);
		if (r) {goto main_error;}
		//goto main_error;
		//Starting 2 times in a row causes a segmentation fault in cam_close.
		//r = cam_start (&cam);
		//if (r) {goto main_error;}
	}
			
	for (size_t I = 0; I < opt.loop_count; I = I + 1)
	{
		int r;
		struct Gev_Image * image;
		r = cam_read_block (&cam, &image, opt.timeout);
		if (image == NULL) {continue;}
		if (r) {goto main_error;}
		if (opt.mode & OPT_VERBOSITY)
		{
			float exposure;
			float gain;
			r = cam_get_float (&cam, "ExposureTime", &exposure);
			if (r) {goto main_error;}
			r = cam_get_float (&cam, "Gain", &gain);
			if (r) {goto main_error;}
			print_time (stdout, time (NULL));
			cam_print_image (&cam, image);
			printf ("e: %08.2f ", exposure);
			printf ("g: %08.2f\n", gain);
		}
		r = cam_release (&cam, image);
		if (r) {goto main_error;}
	}

	cam_stop (&cam);
	cam_close (&cam);
	return 0;
	
main_error:
	
	cam_stop (&cam);
	cam_close (&cam);
	return 1;
	
}


