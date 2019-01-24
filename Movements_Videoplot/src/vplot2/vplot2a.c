#define _XOPEN_SOURCE 600 /* for usleep */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>

#include "debug.h"

void my_log_callback(void *ptr, int level, const char *fmt, va_list vargs)
{
    vprintf(fmt, vargs);
}

int main (int argc, char * argv [])
{
	//av_log_set_level (AV_LOG_ERROR);
	//av_log_set_callback (my_log_callback);
	
	av_register_all ();
	AVFormatContext * fctx = NULL;
	AVCodec * dec = NULL;
	AVCodecContext * cctx = NULL;
	enum AVMediaType type = AVMEDIA_TYPE_VIDEO;
	int r;
	int ivid;
	r = avformat_open_input (&fctx, argv [1], NULL, NULL);
	if (r < 0)
	{
		fprintf (stderr, "Could not open source file %s.\n", argv [1]);
		fprintf (stderr, "error: %i (%s)\n", r, av_err2str (r));
		exit (1);
	}
	r = avformat_find_stream_info (fctx, NULL);
	if (r < 0)
	{
		fprintf (stderr, "Could not find stream information.\n");
		fprintf (stderr, "error: %i (%s)\n", r, av_err2str (r));
		exit (1);
	}
	r = av_find_best_stream (fctx, type, -1, -1, &dec, 0);
	if (r < 0)
	{
		fprintf(stderr, "Could not find %s ", av_get_media_type_string (type));
		fprintf(stderr, "stream in input file '%s'\n", argv [1]);
		fprintf (stderr, "error: %i (%s)\n", r, av_err2str (r));
		exit (1);
	}
	else
	{
		ivid = r;
	}
	
	cctx = avcodec_alloc_context3 (dec);
	ASSERT (cctx);
	fctx->streams [ivid]->codecpar = avcodec_parameters_alloc ();
	avcodec_parameters_to_context (cctx, fctx->streams [ivid]->codecpar);
	TRACE_F ("width  %i", fctx->streams [ivid]->codecpar->width);
	TRACE_F ("height %i", fctx->streams [ivid]->codecpar->height);
	TRACE_F ("height %i", fctx->streams [ivid]->codecpar->codec_id);
	
	AVStream * stream = fctx->streams [ivid];
	
	
	//ASSERT (stream->codec);
	//ASSERT (stream->codecpar);
	//AVCodec * dec = avcodec_find_decoder (stream->codecpar->codec_id);
	
	
}
