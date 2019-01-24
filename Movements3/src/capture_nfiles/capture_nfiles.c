#include "debug.h"
#include "cam.h"
#include "enc.h"
#include "option.h"
#include "common.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>
#include <math.h>
#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>


void ctrl_auto_exposure2 (float * exposure, struct Gev_Image * image, uint8_t target)
{
	float x = (*exposure);
	float d = 0;
	float n = MAX (1, (float)image->recv_size) * 256;
	for (size_t I = 0; I < image->recv_size; I = I + 1)
	{
		d += (float)(target - image->address [I]);
	}
	d = d / n;
	if (fabs (d) < 0.05f) {d = 0.0f;}
	x += (d * x);
	x = MAX (26, x);
	x = MIN (1000000, x);
	(*exposure) = x;
}


void timer_read (int timer)
{
	uint64_t missed;
	int R;
	R = read (timer, &missed, sizeof (missed));
	ASSERT_F (R == sizeof (missed), "read%s", "");
}


int main (int argc, char * argv [])
{
	struct Gev_Cam cam;
	struct Encoder enc;
	struct Option opt;
	
	enc_init (&enc);
	opt_init (&opt, &enc, argc, argv);

	opt_print (&opt, &enc);
	if (opt.mode & OPT_HELP)
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
		r = cam_allocate_frame_buffer (&cam, opt.buffer_count);
		if (r) {goto main_error;}
		//Get cam attributes to open the encoder
		enc_import_cam (&enc, &cam);
		r = cam_start (&cam);
		if (r) {goto main_error;}
	}
	
	uint32_t file_id = 0;
	uint32_t frame_id = 0;
	float exposure = 30.0f;
	
	while (1)
	{
		struct Gev_Image * image;
		cam_read_block (&cam, &image, opt.timeout);
		if (image)
		{
			if (image->id >= frame_id)
			{
				enc_close (&enc);
				file_id ++;
				frame_id += opt.period_frames;
				if (file_id > opt.file_count) {break;}
				enc_open (&enc);
			}
			if (opt.mode & OPT_VERBOSITY)
			{
				cam_print_image (&cam, image);
			}
			if (opt.mode & OPT_AUTO_EXPOSURE)
			{
				ctrl_auto_exposure2 (&exposure, image, 100);
				cam_set_float (&cam, "ExposureTime", exposure);
			}
			enc_write (&enc, image);
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


