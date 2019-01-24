#pragma once

#include "global.h"
#include "misc.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include <inttypes.h>



#define OPT_NONE 0x0
#define OPT_HELP 0x1
#define OPT_VERBOSITY 0x2

struct Option
{
	int mode;
	char * src;
	char * des;
	uint32_t n; //number of frames to process
	uint32_t v; //number of frames to process
	//TODO:
	//Add background subtraction properties.
	//Add blob detection properties.
};


void opt_print_mode (int mode)
{
	if (mode & OPT_NONE) {fprintf (stderr, "OPT_NONE | ");}
	if (mode & OPT_HELP) {fprintf (stderr, "OPT_HELP | ");}
	if (mode & OPT_VERBOSITY) {fprintf (stderr, "OPT_VERBOSITY | ");}
	fprintf (stderr, "\n");
}


void opt_print (struct Option * opt)
{
	opt_print_mode (opt->mode);
	fprintf (stderr, "%20s %s\n", "src", opt->src);
	fprintf (stderr, "%20s %s\n", "des", opt->des);
	fprintf (stderr, "%20s %lu\n", "n", (unsigned long)opt->n);
	fprintf (stderr, "%20s %lu\n", "v", (unsigned long)opt->v);
	fprintf (stderr, "\n");
}


void opt_help (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	fprintf (stderr, "%4s %s\n", "i:", "Input source files");
	fprintf (stderr, "%4s %s\n", "o:", "Output destination file");
	fprintf (stderr, "%4s %s\n", "n:", "Number of frames to process");
	fprintf (stderr, "%4s %s\n", "v:", "Verbosity (1)");
}


void opt_parse (struct Option * opt, int C)
{
	switch (C)
	{
		case 'h':
		opt->mode |= OPT_HELP;
		break;
		
		case 'i':
		opt->src = optarg;
		break;

		case 'o':
		opt->des = optarg;
		break;
		
		case 'n':
		opt->n = str_u32 (optarg);
		break;
		
		case 'v':
		opt->v = str_u32 (optarg);
		break;
	}
}


void opt_init (struct Option * opt, int argc, char * argv [])
{
	ASSERT_F (opt != NULL, "%s", "");
	ASSERT_F (argv != NULL, "%s", "");
	opt->mode = OPT_NONE;
	opt->des = NULL;
	opt->src = NULL;
	opt->v = 0;
	opt->n = 0;
	while (1)
	{
		int C;
		C = getopt (argc, argv, ":i:o:hn:v:");
		if (C < 0) {break;}
		opt_parse (opt, C);
	}
}
