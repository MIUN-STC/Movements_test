#pragma once

#include "common.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include <inttypes.h>


#define OPT_NONE 0x0
#define OPT_HELP 0x1
#define OPT_VERBOSITY 0x2


void opt_print_mode (int mode)
{
	if (mode & OPT_NONE) {printf ("OPT_NONE | ");}
	if (mode & OPT_HELP) {printf ("OPT_HELP | ");}
	if (mode & OPT_VERBOSITY) {printf ("OPT_VERBOSITY | ");}
	printf ("\n");
}


struct Option
{
	int mode;
	uint32_t buffer_count;
	int32_t timeout;
	uint32_t frame_count;
	char * name;
	char * config_file;
};


void opt_print (struct Option * opt, struct Encoder * enc)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%20s\n", "Option");
	printf ("%20s ", "localtime");
	print_time (stdout, time (NULL));
	printf ("\n");
	printf ("%20s %x\n", "mode", opt->mode);
	opt_print_mode (opt->mode);
	printf ("%20s %i\n", "buffer_count", (int)opt->buffer_count);
	printf ("%20s %i\n", "frame_count", (int)opt->frame_count);
	printf ("%20s %i\n", "bitrate", (int)enc->bitrate);
	printf ("%20s %llu\n", "timeout", (long long unsigned)opt->timeout);
	printf ("%20s %s\n", "name", enc->name);
	printf ("%20s %s\n", "config_file", opt->config_file);
	printf ("%20s %s\n", "machine", enc->machine);
	printf ("\n");
}


void opt_help (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%4s %s\n", "h", "Shows help");
	printf ("%4s %s\n", "c:", "Config file");
	printf ("%4s %s\n", "n:", "frame count");
	printf ("%4s %s\n", "T:", "timeout");
	printf ("%4s %s\n", "b:", "Number of frame buffers");
	printf ("%4s %s\n", "B:", "Bitrate");
	printf ("%4s %s\n", "v", "verbosity");
	printf ("%4s %s\n", "o:", "Name");
	printf ("%4s %s\n", "m:", "Machine");
}


void opt_parse (struct Option * opt, struct Encoder * enc, int C)
{
	switch (C)
	{
		case 'h':
		opt->mode |= OPT_HELP;
		break;
		
		case 'v':
		opt->mode |= OPT_VERBOSITY;
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
		enc->name = optarg;
		break;
		
		case 'm':
		enc->machine = optarg;
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
	opt->frame_count = 50;
	opt->buffer_count = 50;
	opt->mode = OPT_NONE;
	opt->config_file = "conf.txt";
	opt->timeout = UINT32_MAX;
	enc->name = "default";
	enc->machine = "tx2";
	enc->bitrate = 500000;
	while (1)
	{
		int C;
		C = getopt (argc, argv, ":hb:n:vB:T:c:o:m:");
		if (C < 0) {break;}
		opt_parse (opt, enc, C);
	}
}
