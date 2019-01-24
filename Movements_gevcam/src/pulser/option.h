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
	uint32_t duration_period_usec;
	uint32_t duration_on_usec;
	uint32_t gpio;
};


void opt_print (struct Option * opt)
{
	typedef long unsigned lu;
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%20s\n", "Option");
	printf ("%20s ", "localtime");
	print_time (stdout, time (NULL));
	printf ("\n");
	printf ("%20s %x ", "mode", opt->mode);
	opt_print_mode (opt->mode);
	printf ("%20s %lu\n", "duration_period_usec", (lu)opt->duration_period_usec);
	printf ("%20s %lu\n", "duration_on_usec", (lu)opt->duration_on_usec);
	printf ("%20s %lu\n", "gpio number", (lu)opt->gpio);
	printf ("\n");
}


void opt_help (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%4s %s\n", "h", "Shows help");
	printf ("%4s %s\n", "v", "Verbosity");
	printf ("%4s %s\n", "p", "duration_period_usec");
	printf ("%4s %s\n", "i", "duration_on_usec");
	printf ("%4s %s\n", "g", "gpio number");
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
		
		case 'p':
		opt->duration_period_usec = str_u32 (optarg);
		break;
		
		case 'i':
		opt->duration_on_usec = str_u32 (optarg);
		break;

		case 'g':
		opt->gpio = str_u32 (optarg);
		break;
	}
}


void opt_init (struct Option * opt, int argc, char * argv [])
{
	ASSERT_F (opt != NULL, "%s", "");
	ASSERT_F (argv != NULL, "%s", "");
	opt->mode = OPT_NONE;
	opt->duration_period_usec = 1000 * 200;
	opt->duration_on_usec = 1000 * 10;
	opt->gpio = 255;
	while (1)
	{
		int C;
		C = getopt (argc, argv, ":hvp:i:g:");
		if (C < 0) {break;}
		opt_parse (opt, C);
	}
}
