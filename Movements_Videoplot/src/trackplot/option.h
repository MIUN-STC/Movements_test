#pragma once

#include "global.h"
#include "misc.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include <inttypes.h>



#define OPT_NONE       0x0000
#define OPT_HELP       0x0001
#define OPT_VERBOSITY  0x0002
#define OPT_ERROR      0x0004
#define OPT_SAVEVID    0x0008

struct Option
{
	int mode;
	char * src_vid;
	char * src_blob;
	char * src_rel;
	
	char * des; //Outout video file
	
	uint32_t nr; //Number of relational records
	uint32_t nb; //Number of blob records
	uint32_t nf; //Number of frames
	
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
	typedef long long unsigned llu;
	opt_print_mode (opt->mode);
	fprintf (stderr, "%20s %s\n", "src_vid", opt->src_vid);
	fprintf (stderr, "%20s %s\n", "src_blob", opt->src_blob);
	fprintf (stderr, "%20s %s\n", "src_rel", opt->src_rel);
	fprintf (stderr, "%20s %s\n", "des", opt->des);
	fprintf (stderr, "%20s %llu\n", "nr", (llu)opt->nr);
	fprintf (stderr, "%20s %llu\n", "nb", (llu)opt->nb);
	fprintf (stderr, "%20s %llu\n", "nf", (llu)opt->nf);
	fprintf (stderr, "\n");
}


void opt_help (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	fprintf (stderr, "%4s %s\n", "v:", "Input video file");
	fprintf (stderr, "%4s %s\n", "b:", "Input blob file");
	fprintf (stderr, "%4s %s\n", "r:", "Input relational index file");
	fprintf (stderr, "%4s %s\n", "o:", "Output video file");
	fprintf (stderr, "%4s %s\n", "nr:", "Number of relational records");
	fprintf (stderr, "%4s %s\n", "nb:", "Number of blob records");
	fprintf (stderr, "%4s %s\n", "nf:", "Number of frames");
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
		
		case 'o':
		opt->des = optarg;
		break;
		
		case 'n':
		{
			uint32_t v = str_u32 (optarg + 1);
			if (optarg [0] == 'b'){opt->nb = v;}
			if (optarg [0] == 'r'){opt->nr = v;}
			if (optarg [0] == 'f'){opt->nf = v;}
		}
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
	opt->des = NULL;
	opt->nf = 0;
	while (1)
	{
		int C;
		C = getopt (argc, argv, ":hv:b:r:n:o:");
		if (C < 0) {break;}
		opt_parse (opt, C);
	}
}
