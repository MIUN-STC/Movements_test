#pragma once

#include "global.h"
#include "misc.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include <inttypes.h>



#define OPT_NONE 0
#define OPT_HELP 1U << 1
#define OPT_VERBOSITY 1U << 2
#define OPT_ERROR 1U << 3

struct Option
{
	int mode;
	char * src_vid;
	char * src_blob;
	char * src_rel;
};


void opt_print_mode (int mode)
{
	if (mode & OPT_NONE) {fprintf (stderr, "OPT_NONE | ");}
	if (mode & OPT_HELP) {fprintf (stderr, "OPT_HELP | ");}
	if (mode & OPT_VERBOSITY) {fprintf (stderr, "OPT_VERBOSITY | ");}
	if (mode & OPT_ERROR) {fprintf (stderr, "OPT_ERROR | ");}
	fprintf (stderr, "\n");
}


void opt_print (struct Option * opt)
{
	opt_print_mode (opt->mode);
	fprintf (stderr, "%20s %s\n", "src", opt->src_vid);
	fprintf (stderr, "%20s %s\n", "src", opt->src_blob);
	fprintf (stderr, "%20s %s\n", "src", opt->src_rel);
	fprintf (stderr, "\n");
}


void opt_help (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	fprintf (stderr, "%4s %s\n", "v:", "Input video file");
	fprintf (stderr, "%4s %s\n", "b:", "Input blob file");
	fprintf (stderr, "%4s %s\n", "r:", "Input relationship file");
}


void opt_parse (struct Option * opt, int C)
{
	char * e = NULL;
	switch (C)
	{
		case ':':
		TRACE_F (": %c", optopt);
		break;
		
		case 'h':
		opt->mode |= OPT_HELP;
		break;
		
		case 'v':
		opt->src_vid = optarg;
		break;
		
		case 'b':
		opt->src_blob = optarg;
		break;
		
		case 'r':
		opt->src_rel = optarg;
		break;
	}
}


void opt_init (struct Option * opt, int argc, char * argv [])
{
	ASSERT_F (opt != NULL, "%s", "");
	ASSERT_F (argv != NULL, "%s", "");
	opt->mode = 0;
	opt->src_vid = NULL;
	opt->src_blob = NULL;
	opt->src_rel = NULL;
	while (1)
	{
		int C;
		C = getopt (argc, argv, ":hv:b:r:");
		if (C < 0) {break;}
		opt_parse (opt, C);
	}
}
