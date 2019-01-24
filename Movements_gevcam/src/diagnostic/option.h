#pragma once
#include "debug.h"
#include "common.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
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
	uint32_t loop_count;
	int32_t timeout;
	char * config_file;
	char * sn;
};


void opt_help (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%4s %s\n", "h", "Shows help");
	printf ("%4s %s\n", "n:", "Number of frames");
	printf ("%4s %s\n", "T:", "timeout");
	printf ("%4s %s\n", "v:", "verbosity");
	printf ("%4s %s\n", "b:", "Number of frame buffers");
	printf ("%4s %s\n", "c:", "Config file");
	printf ("%4s %s\n", "e:", "Auto exposure");
	printf ("%4s %s\n", "s:", "Serial number");
}


void opt_print (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%20s\n", "Option");
	printf ("%20s ", "localtime");
	print_time (stdout, time (NULL));
	printf ("\n");
	printf ("%20s %x ", "mode", opt->mode);
	opt_print_mode (opt->mode);
	printf ("%20s %i\n", "buffer_count", (int)opt->buffer_count);
	printf ("%20s %i\n", "loop_count", (int)opt->loop_count);
	printf ("%20s %llu\n", "timeout", (long long unsigned)opt->timeout);
	printf ("%20s %s\n", "config_file", opt->config_file);
	printf ("%20s %s\n", "sn", opt->sn);
	printf ("\n");
}


void opt_parse (struct Option * opt, int C)
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
		opt->loop_count = str_u32 (optarg);
		break;
		
		case 'c':
		opt->config_file = optarg;
		break;
		
		case 's':
		opt->sn = optarg;
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


void opt_init (struct Option * opt, int argc, char * argv [])
{
	ASSERT_F (opt != NULL, "%s", "");
	ASSERT_F (argv != NULL, "%s", "");
	opt->timeout = UINT32_MAX;
	opt->buffer_count = 10;
	opt->loop_count = 10;
	opt->mode = OPT_NONE;
	opt->config_file = "conf.txt";
	opt->sn = NULL;
	while (1)
	{
		int C;
		C = getopt (argc, argv, ":hb:vT:n:c:s:");
		if (C < 0) {break;}
		opt_parse (opt, C);
	}
}
