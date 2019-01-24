#include "debug.h"
#include "cam.h"
#include "enc.h"
#include "option.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>


int main (int argc, char * argv [])
{
	struct Gev_Cam cam;
	struct Encoder enc;
	struct Option opt;
	
	enc_init (&enc);
	opt_init (&opt, &enc, argc, argv);
	
	opt_print (&opt, &enc);
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
		//Get cam attributes to open the encoder
		enc_import_cam (&enc, &cam);
		enc_open (&enc);
		cam_start (&cam);
		if (r) {goto main_error;}
	}
	
	for (size_t i = 0; i < opt.frame_count; i = i + 1)
	{
		struct Gev_Image * image;
		cam_read_block (&cam, &image, opt.timeout);
		if (image)
		{
			if (opt.verbosity == 200)
			{
				cam_print_image (&cam, image);
			}
			//if (image->status == GEV_FRAME_STATUS_RECVD)
			{
				enc_write (&enc, image);
			}
			cam_release (&cam, image);
		}
	}
	
	cam_close (&cam);
	enc_close (&enc);
	return 0;
	
main_error:

	cam_close (&cam);
	enc_close (&enc);
	return 1;
}


