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

struct Option
{
	int mode;
	char * vidl;
	char * vidr;
	char * vall;
	char * valr;
};


void opt_print_mode (int mode)
{
	if (mode & OPT_NONE) {printf ("OPT_NONE | ");}
	if (mode & OPT_HELP) {printf ("OPT_HELP | ");}
	if (mode & OPT_VERBOSITY) {printf ("OPT_VERBOSITY | ");}
	printf ("\n");
}


void opt_print (struct Option * opt)
{
	opt_print_mode (opt->mode);
	printf ("%20s %s\n", "vidl", opt->vidl);
	printf ("%20s %s\n", "vidr", opt->vidr);
	printf ("%20s %s\n", "vall", opt->vall);
	printf ("%20s %s\n", "valr", opt->valr);
	printf ("\n");
}


void opt_help (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%4s %s\n", "h", "Help");
	printf ("%4s %s\n", "v", "Verbosity");
	printf ("%4s %s\n", "L:", "Left camera");
	printf ("%4s %s\n", "R:", "Right camera");
	printf ("%4s %s\n", "l:", "Values left");
	printf ("%4s %s\n", "r:", "Values right");
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
		
		case 'L':
		opt->vidl = optarg;
		break;

		case 'R':
		opt->vidr = optarg;
		break;
		
		case 'l':
		opt->vall = optarg;
		break;

		case 'r':
		opt->valr = optarg;
		break;
	}
}


void opt_init (struct Option * opt, int argc, char * argv [])
{
	ASSERT_F (opt != NULL, "%s", "");
	ASSERT_F (argv != NULL, "%s", "");
	opt->mode = OPT_NONE;
	opt->vidl = NULL;
	opt->vidr = NULL;
	opt->vall = NULL;
	opt->valr = NULL;
	while (1)
	{
		int C;
		C = getopt (argc, argv, ":hvL:R:l:r:");
		if (C < 0) {break;}
		opt_parse (opt, C);
	}
}
