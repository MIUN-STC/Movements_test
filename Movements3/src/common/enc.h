#pragma once

#include "debug.h"
#include "cam.h"
#include "common.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include <inttypes.h>
#include <time.h>


struct Encoder
{
	FILE * file;
	char * name;
	float fps;
	uint32_t width;
	uint32_t height;
	uint32_t bitrate;
};


void enc_init (struct Encoder * enc)
{
	ASSERT_F (enc != NULL, "%s", "");
	enc->file = NULL;
	enc->name = NULL;
	enc->bitrate = 0;
	enc->fps = 1;
	enc->width = 0;
	enc->height = 0;
}


void enc_import_cam (struct Encoder * enc, struct Gev_Cam * cam)
{
	ASSERT_F (enc != NULL, "%s", "");
	ASSERT_F (cam != NULL, "%s", "");
	ASSERT_F (cam->handle != NULL, "%s", "");
	cam_get_u32 (cam, "Width", &enc->width);
	cam_get_u32 (cam, "Height", &enc->height);
	cam_get_float (cam, "AcquisitionFrameRate", &enc->fps);
}


void set_file_name (char * buffer, size_t length, char const * name)
{
	ASSERT_F (buffer != NULL, "%s", "");
	ASSERT_F (name != NULL, "%s", "");
	
	time_t raw;
	struct tm * T;
	time (&raw);
	T = localtime (&raw);
	snprintf 
	(
		buffer, 
		length, 
		"%s_%04i%02i%02i_%02i%02i%02i.mp4", 
		name,
		T->tm_year + 1900,
		T->tm_mon + 1,
		T->tm_mday,
		T->tm_hour,
		T->tm_min,
		T->tm_sec
	);
}


void enc_open (struct Encoder * enc)
{
	ASSERT_F (enc != NULL, "%s", "");
	ASSERT_F (enc->name != NULL, "%s", "");
	
	char filename [100];
	set_file_name (filename, sizeof (filename), enc->name);
	
	//TODO: TriggerMode On needs a fps value.
	enc->fps = 5;
	
	char buffer [1000] = {'\0'};
	snprintf 
	(
		buffer, 
		sizeof (buffer), 
		"gst-launch-1.0 fdsrc fd=0 ! "
		"videoparse width=%i height=%i format=gray8 framerate=%i/1 ! "
		"videoconvert ! "
		"'video/x-raw, format=(string)I420, width=(int)%i, height=(int)%i, framerate=%i/1' ! "
		"omxh265enc bitrate=%i ! "
		"'video/x-h265, streamformat=(string)byte-stream' ! "
		"h265parse ! "
		//"mpegtsmux ! "
		"matroskamux ! "
		//"qtmux ! "
		"filesink location=%s sync=false async=false -e",
		(int)enc->width, 
		(int)enc->height,
		(int)enc->fps,
		(int)enc->width, 
		(int)enc->height,
		(int)enc->fps,
		(int)enc->bitrate,
		filename
	);
	fprintf (stderr, "%s\n", buffer);
	enc->file = popen (buffer, "w");
	ASSERT_F (enc->file != NULL, "%s", "popen");
}


void enc_close (struct Encoder * enc)
{
	ASSERT_F (enc != NULL, "%s", "");
	if (enc->file == NULL) {return;}
	fflush (enc->file);
	pclose (enc->file);
	enc->file = NULL;
}


void enc_write (struct Encoder * enc, struct Gev_Image * img)
{
	ASSERT_F (enc != NULL, "%s", "");
	ASSERT_F (img != NULL, "%s", "");
	ASSERT_F (enc->file != NULL, "%s", "");
	
	ASSERT_F (
	img->w * img->h == img->recv_size, 
	"Recevied image size is not correct%s", "");
	
	int R;
	R = fwrite (img->address, img->recv_size, 1, enc->file);
	ASSERT_F (R == 1, "%s %i", "write error", R);
}


