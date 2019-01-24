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
	char * src;
	char * des;

	float r0;
	float rk;
	float tplus;
	
	float sx;
	float sy;
	float st;
	float sD;
	
	float Dpass;
	
	//Maximum number of blob per frame
	uint32_t nbf_max;
	
	uint32_t nb;
	uint32_t nr;
	
	uint32_t qmin;
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
	fprintf (stderr, "%20s %s\n", "src", opt->src);
	fprintf (stderr, "%20s %s\n", "des", opt->des);
	fprintf (stderr, "%20s %f\n", "r0", (double)opt->r0);
	fprintf (stderr, "%20s %f\n", "rk", (double)opt->rk);
	fprintf (stderr, "%20s %f\n", "tplus", (double)opt->tplus);
	fprintf (stderr, "%20s %f\n", "sx", (double)opt->sx);
	fprintf (stderr, "%20s %f\n", "sy", (double)opt->sy);
	fprintf (stderr, "%20s %f\n", "st", (double)opt->st);
	fprintf (stderr, "%20s %f\n", "sD", (double)opt->sD);
	fprintf (stderr, "%20s %lu\n", "nb", (unsigned long)opt->nb);
	fprintf (stderr, "%20s %lu\n", "nr", (unsigned long)opt->nr);
	fprintf (stderr, "%20s %f\n", "Dpass", (double)opt->Dpass);
	fprintf (stderr, "%20s %f\n", "qmin", (double)opt->qmin);
	fprintf (stderr, "%20s %lu\n", "nbf_max", (unsigned long)opt->nbf_max);
	fprintf (stderr, "\n");
}


void opt_help (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	fprintf (stderr, "Input format is (t x y) for each text line, where each line is a blob.\n");
	fprintf (stderr, "Output format is (t b q) for each text line, where each line is track.\n");
	fprintf (stderr, "Where t is frame number, x, y is blob coordinate, ");
	fprintf (stderr, "b is blob index, ");
	fprintf (stderr, "q is number of blob in the track.\n\n");
	fprintf (stderr, "%4s %s\n", "i:", "Input source files. (stdin) default");
	fprintf (stderr, "%4s %s\n", "o:", "Output destination file. (stdout) default");
	fprintf (stderr, "%4s %s\n", "n:", "Number of frames to process");
	fprintf (stderr, "%4s %s\n", "v", "Verbosity on/off");
	fprintf (stderr, "%4s %s\n", "r0:", "Cone start radius");
	fprintf (stderr, "%4s %s\n", "rk:", "Cone growth radius rate");
	fprintf (stderr, "%4s %s\n", "t:", "How far in future to search");
	fprintf (stderr, "%4s %s\n", "sx:", "Space X scalar. Distance function.");
	fprintf (stderr, "%4s %s\n", "sy:", "Space Y scalar. Distance function.");
	fprintf (stderr, "%4s %s\n", "st:", "Time T scalar. Distance function.");
	fprintf (stderr, "%4s %s\n", "sD:", "Diameter D scalar. Distance function.");
	fprintf (stderr, "%4s %s\n", "nb:", "Number of blobs to allocate");
	fprintf (stderr, "%4s %s\n", "nr:", "Number of relationship to allocate");
	fprintf (stderr, "%4s %s\n", "m:", "Max number of blobs per frame");
	fprintf (stderr, "%4s %s\n", "D:", "Blob diameter passthrough");
	fprintf (stderr, "%4s %s\n", "q:", "Minmum numbers of blob in the track");
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
		
		case 'i':
		opt->src = optarg;
		break;

		case 'o':
		opt->des = optarg;
		break;
		
		case 'v':
		opt->mode |= OPT_VERBOSITY;
		break;
		
		case 'm':
		opt->nbf_max = str_u32 (optarg);
		break;
		
		case 'r':
		{
			//TRACE_F ("%s", optarg);
			if (optarg [0] == '\0') {break;}
			float v = strtof (optarg + 1, &e);
			if (optarg [0] == '0'){opt->r0 = v;}
			if (optarg [0] == 'k'){opt->rk = v;}
			if (e [0] != '\0') {opt->mode |= OPT_ERROR;}
		}
		break;
		
		case 't':
		opt->tplus = strtof (optarg, &e);
		if (e [0] != '\0') {opt->mode |= OPT_ERROR;}
		break;
		
		case 'q':
		opt->qmin = strtof (optarg, &e);
		if (e [0] != '\0') {opt->mode |= OPT_ERROR;}
		break;
		
		case 'D':
		opt->Dpass = strtof (optarg, &e);
		if (e [0] != '\0') {opt->mode |= OPT_ERROR;}
		break;
		
		case 'n':
		{
			if (optarg [0] == '\0') {break;}
			uint32_t v = str_u32 (optarg + 1);
			if (optarg [0] == 'b'){opt->nb = v;}
			if (optarg [0] == 'r'){opt->nr = v;}
		}
		break;
		
		case 's':
		{
			if (optarg [0] == '\0') {break;}
			float v = strtof (optarg + 1, &e);
			if (optarg [0] == 'x'){opt->sx = v;}
			if (optarg [0] == 'y'){opt->sy = v;}
			if (optarg [0] == 't'){opt->st = v;}
			if (optarg [0] == 'D'){opt->sD = v;}
			if (e [0] != '\0') {opt->mode |= OPT_ERROR;}
		}
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
	while (1)
	{
		int C;
		C = getopt (argc, argv, ":i:vo:hn:r:t:s:R:D:q:");
		if (C < 0) {break;}
		opt_parse (opt, C);
	}
}
