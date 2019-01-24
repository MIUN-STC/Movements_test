//#define CAM_ASSERT_F ASSERT_F
//#define CAM_TRACE_F TRACE_F

#include "debug.h"
#include "cam.h"
#include "enc.h"
#include "option.h"
#include "common.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>


struct Global
{
	uint32_t loop_index;
	volatile sig_atomic_t signal_cancel;
};

struct Global global = {0};


static void signal_cancel_handler (int sig, siginfo_t * info, void * context)
{
	//Remove unused warning
	if (sig) {}
	if (info) {}
	if (context) {}
	//Should exit gracefully
	global.signal_cancel = 1;
}


void handle_frame 
(
	struct Option * opt, 
	struct Gev_Cam * cam, 
	struct Encoder * enc, 
	struct Gev_Image * image
)
{
	ASSERT_F (opt != NULL, "%s", "");
	ASSERT_F (cam != NULL, "%s", "");
	ASSERT_F (enc != NULL, "%s", "");
	if (image == NULL) {return;}
	if (image->status != GEV_FRAME_STATUS_RECVD) {return;}
	enc_write (enc, image);
}


void handle_frameinfo
(
	struct Option * opt, 
	struct Gev_Cam * cam, 
	struct Gev_Image * image
)
{
	ASSERT_F (opt != NULL, "%s", "");
	ASSERT_F (cam != NULL, "%s", "");
	if (image == NULL) 
	{
		print_time (stdout, time (NULL));
		printf ("NULL Frame\n");
		return;
	}
	if (image->status != GEV_FRAME_STATUS_RECVD) 
	{
		print_time (stdout, time (NULL));
		cam_print_transfer_status (cam);
		cam_print_image (cam, image);
		printf ("\n");
		return;
	}
	if ((global.loop_index % 10) == 0)
	{
		float exposure;
		cam_get_float (cam, "ExposureTime", &exposure);
		typedef long unsigned lu;
		print_time (stdout, time (NULL));
		cam_print_transfer_status (cam);
		printf ("i: %06lu, ", (lu)global.loop_index);
		printf ("id: %06lu, ", (lu)image->id);
		printf ("e: %06lu", (lu)exposure);
		printf ("\n");
	}
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
		r = cam_open_serial (&cam, opt.sn);
		if (r) {goto main_error;}
		r = cam_load (&cam, opt.config_file);
		if (r) {goto main_error;}
		r = cam_allocate_frame_buffer (&cam, opt.buffer_count);
		if (r) {goto main_error;}
		//Get cam attributes to open the encoder
		enc_import_cam (&enc, &cam);
		enc_open (&enc);
		cam_start (&cam);
		if (r) {goto main_error;}
	}
	
	
	{
		struct sigaction a;
		a.sa_sigaction = &signal_cancel_handler;
		a.sa_flags = SA_SIGINFO;
		int r;
		r = sigemptyset (&a.sa_mask);
		ASSERT_F (r != -1, "%s", "");
		//TODO: find out why SIGINT exits program before cam_close.
		//SIGUSR1 exits program after cam_close.
		r = sigaction (SIGUSR1, &a, NULL);
		ASSERT_F (r != -1, "%s", "");
	}
	
	global.loop_index = 0;
	global.signal_cancel = 0;
	
	while (1)
	{
		global.loop_index ++;
		
		if (global.signal_cancel)
		{
			printf ("loop break by signal!\n");
			break;
		}
		
		{
			time_t t = time (NULL);
			struct tm * m = localtime (&t);
			if ((m->tm_hour == (int)opt.tohour) && (m->tm_min == (int)opt.tomin))
			{
				printf ("loop break by timer!\n");
				break;
			}
		}
		
		struct Gev_Image * image;
		cam_read_block (&cam, &image, opt.timeout);
		handle_frame (&opt, &cam, &enc, image);
		handle_frameinfo (&opt, &cam, image);
		if (image) {cam_release (&cam, image);}
	}
	
	printf ("cam_close!\n");
	
	cam_close (&cam);
	enc_close (&enc);
	return 0;
	
main_error:

	cam_close (&cam);
	enc_close (&enc);
	return 1;
}


