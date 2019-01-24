#include "option.h"
#include "debug.h"
#include "cam.h"
#include "common.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <assert.h>
#include <inttypes.h>


int main (int argc, char * argv [])
{
	struct Gev_Cam cam;
	struct Option opt;
	
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	
	if (opt.help_flag)
	{
		opt_help (&opt);
		return 0;
	}
	
	{
		int r;
		r = cam_open (&cam);
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
		struct Gev_Image * image;
		int r;
		r = cam_read_block (&cam, &image, opt.timeout);
		if (r) {goto main_error;}
		if (opt.verbosity == 200)
		{
			cam_print_image (&cam, image);
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


