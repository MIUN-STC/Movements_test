#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>

struct Option
{
	int help_flag;
	uint32_t buffer_count;
	uint32_t frame_count;
	int32_t timeout;
	uint32_t verbosity;
	char * name;
	char * config_file;
};


void opt_print (struct Option * opt, struct Encoder * enc)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%20s\n", "Option");
	printf ("%20s %lu\n", "buffer_count", (long unsigned)opt->buffer_count);
	printf ("%20s %lu\n", "frame count", (long unsigned)opt->frame_count);
	printf ("%20s %lu\n", "verbosity", (long unsigned)opt->verbosity);
	printf ("%20s %i\n", "help_flag", (int)opt->help_flag);
	printf ("%20s %lu\n", "bitrate", (long unsigned)enc->bitrate);
	printf ("%20s %llu\n", "timeout", (long long unsigned)opt->timeout);
	printf ("%20s %s\n", "name", enc->name);
	printf ("%20s %s\n", "config_file", opt->config_file);
	printf ("\n");
}


void opt_help (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%4s %s\n", "h", "Shows help");
	printf ("%4s %s\n", "n:", "Number of frames");
	printf ("%4s %s\n", "b:", "Number of frame buffers");
	printf ("%4s %s\n", "B:", "Bitrate");
	printf ("%4s %s\n", "c:", "Config file");
	printf ("%4s %s\n", "v:", "verbosity");
	printf ("%4s %s\n", "o:", "Name");
	printf ("%4s %s\n", "T:", "timeout");
}


void opt_parse (struct Option * opt, struct Encoder * enc, int C)
{
	switch (C)
	{
		case 'h':
		opt->help_flag = 1;
		break;
		
		case 'v':
		opt->verbosity = str_u32 (optarg);
		break;
		
		case 'b':
		opt->buffer_count = str_u32 (optarg);
		break;
		
		case 'n':
		opt->frame_count = str_u32 (optarg);
		break;
		
		case 'B':
		enc->bitrate = str_u32 (optarg);
		break;
		
		case 'o':
		enc->name = optarg;
		break;
		
		case 'c':
		opt->config_file = optarg;
		break;
		
		case 'T':
		{
		intmax_t v = str_imax (optarg);
		if (v == -1) {opt->timeout = UINT32_MAX;}
		else {opt->timeout = v;}
		}
		break;
	}
}


void opt_init (struct Option * opt, struct Encoder * enc, int argc, char * argv [])
{
	ASSERT_F (opt != NULL, "%s", "");
	ASSERT_F (argv != NULL, "%s", "");
	opt->frame_count = 10;
	opt->buffer_count = 10;
	opt->help_flag = 0;
	opt->verbosity = 0;
	opt->config_file = "conf.txt";
	opt->timeout = UINT32_MAX;
	enc->name = "default";
	enc->bitrate = 50000;
	while (1)
	{
		int C;
		C = getopt (argc, argv, ":hb:n:v:B:T:o:");
		if (C < 0) {break;}
		opt_parse (opt, enc, C);
	}
}
