#pragma once
#include "debug.h"
#include "common.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <inttypes.h>

struct Option
{
	int help_flag;
	uint32_t buffer_count;
	uint32_t verbosity;
	uint32_t loop_count;
	int32_t timeout;
	char * config_file;
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
}


void opt_print (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%20s\n", "Option");
	printf ("%20s %i\n", "buffer_count", (int)opt->buffer_count);
	printf ("%20s %i\n", "loop_count", (int)opt->loop_count);
	printf ("%20s %i\n", "verbosity", (int)opt->verbosity);
	printf ("%20s %llu\n", "timeout", (long long unsigned)opt->timeout);
	printf ("%20s %i\n", "help_flag", (int)opt->help_flag);
	printf ("%20s %s\n", "config_file", opt->config_file);
	printf ("\n");
}


void opt_parse (struct Option * opt, int C)
{
	switch (C)
	{
		case 'h':
		opt->help_flag = 1;
		break;
		
		case 'b':
		opt->buffer_count = str_u32 (optarg);
		break;
		
		case 'n':
		opt->loop_count = str_u32 (optarg);
		break;
		
		case 'v':
		opt->verbosity = str_u32 (optarg);
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


void opt_init (struct Option * opt, int argc, char * argv [])
{
	ASSERT_F (opt != NULL, "%s", "");
	ASSERT_F (argv != NULL, "%s", "");
	opt->timeout = 10000;
	opt->buffer_count = 10;
	opt->loop_count = 10;
	opt->help_flag = 0;
	opt->verbosity = 200;
	opt->config_file = "conf.txt";
	while (1)
	{
		int C;
		C = getopt (argc, argv, ":hb:v:T:n:c:");
		if (C < 0) {break;}
		opt_parse (opt, C);
	}
}
