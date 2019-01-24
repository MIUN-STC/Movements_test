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
	char * directory;
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
	printf ("%20s %s\n", "directory", opt->directory);
	printf ("\n");
}


void opt_help (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%4s %s\n", "d:", "Directory path");
}


void opt_parse (struct Option * opt, int C)
{
	switch (C)
	{
		case 'h':
		opt->mode |= OPT_HELP;
		break;
		
		case 'd':
		opt->directory = optarg;
		break;
	}
}


void opt_init (struct Option * opt, int argc, char * argv [])
{
	ASSERT_F (opt != NULL, "%s", "");
	ASSERT_F (argv != NULL, "%s", "");
	opt->mode = OPT_NONE;
	opt->directory = NULL;
	while (1)
	{
		int C;
		C = getopt (argc, argv, ":i:hd:");
		if (C < 0) {break;}
		opt_parse (opt, C);
	}
}
