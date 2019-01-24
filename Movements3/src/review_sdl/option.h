#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include <inttypes.h>

struct Option
{
	int help_flag;
	char * config_file;
	char * image_filename;
};


void opt_print (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%20s\n", "Option");
	printf ("%20s %i\n", "help_flag", (int)opt->help_flag);
	printf ("%20s %s\n", "config_file", opt->config_file);
	printf ("%20s %s\n", "image_filename", opt->image_filename);
	printf ("\n");
}


void opt_help (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%20s %s\n", "h", "Shows help");
	printf ("%20s %s\n", "c:", "Config file");
	printf ("%20s %s\n", "o:", "Outout image file");
	printf ("%20s %s\n", "[P]", "Save image file from texture");
	printf ("%20s %s\n", "[H]", "Show histogram");
	printf ("%20s %s\n", "[C]", "Show crosshair");
	printf ("%20s %s\n", "[S]", "Enable save on trigger");
	printf ("%20s %s\n", "[SPACE]", "Start/Stop");
	printf ("%20s %s\n", "[5]", "Enable auto exposure");
	printf ("%20s %s\n", "[6]", "Enable auto gain");
	printf ("%20s %s\n", "[E]", "Enable manual exposure control");
	printf ("%20s %s\n", "[G]", "Enable manual gain control");
	printf ("%20s %s\n", "[LEFT]", "Select left digit of current control number");
	printf ("%20s %s\n", "[RIGHT]", "Select right digit of current control number");
	printf ("%20s %s\n", "[UP]", "Increase digit");
	printf ("%20s %s\n", "[DOWN]", "Decrease digit");
}


void opt_parse (struct Option * opt, int C)
{
	switch (C)
	{
		case 'h':
		opt->help_flag = 1;
		break;
		
		case 'c':
		opt->config_file = optarg;
		break;
		
		case 'o':
		opt->image_filename = optarg;
		break;
	}
}


void opt_init (struct Option * opt, int argc, char * argv [])
{
	ASSERT_F (opt != NULL, "%s", "");
	ASSERT_F (argv != NULL, "%s", "");
	opt->help_flag = 0;
	opt->config_file = "conf.txt";
	opt->image_filename = "default";
	while (1)
	{
		int C;
		C = getopt (argc, argv, ":hc:o:");
		if (C < 0) {break;}
		opt_parse (opt, C);
	}
}
