#include "pp.h"
#include "option.h"

#include "mem.h"



void app_stretch 
(
	uint32_t n,
	struct pp_v4f32_txy1 des [],
	struct pp_v4f32_txy1 src [],
	struct Option * opt
)
{
	for (uint32_t i = 0; i < n; ++ i)
	{
		des [i].t = src [i].t * opt->dt;
		des [i].x = src [i].x * opt->dx;
		des [i].y = src [i].y * opt->dy;
		des [i].w = 1.0f;
	}
}	



FILE * app_get_src_output (struct Option * opt)
{
	FILE * out = NULL;
	if (strcmp (opt->des, "stdout") == 0)
	{
		out = stdout;
	}
	else if (opt->des != NULL)
	{
		out = fopen (opt->des, "w+");
	}
	return out;
}


FILE * app_get_src_input (struct Option * opt)
{
	FILE * f = NULL;
	if (strcmp (opt->src, "stdin") == 0)
	{
		f = stdin;
	}
	else if (opt->src != NULL)
	{
		f = fopen (opt->src, "r");
		ASSERT (f != NULL);
	}
	return f;
}


struct PP
{
	//Input blob file
	FILE * f;
	
	//Blob position original
	struct pp_v4f32_txy1 * pos0;
	
	//Blob position stretched
	struct pp_v4f32_txy1 * pos1;
	
	//Paths and tracks as (Next Index Notation)
	uint32_t * path;
	uint32_t * track;
	float * dmin;
	
	//Each element is a uniq track.
	//Element value is the start of the path as blob index.
	uint32_t * h;
	
	//Each element is a uniq track.
	//Element value is the start of the path as blob index.
	uint32_t * b;
	
	//Each element is a uniq track.
	//Element value is the number of blobs in the track.
	uint32_t * q;
	
};


void pp_load (struct PP * pp, struct Option * opt)
{
	pp->dmin = calloc (opt->nb, sizeof (float));
	pp->pos0 = calloc (opt->nb, sizeof (struct pp_v4f32_txy1));
	pp->pos1 = calloc (opt->nb, sizeof (struct pp_v4f32_txy1));
	pp->path = calloc (opt->nb, sizeof (uint32_t));
	pp->track = calloc (opt->nb, sizeof (uint32_t));
	vu32_set1 (opt->nb, pp->path, UINT32_MAX);
	vu32_set1 (opt->nb, pp->track, UINT32_MAX);
	pp_fread_all (pp->f, &opt->nb, pp->pos0);
	app_stretch (opt->nb, pp->pos1, pp->pos0, opt);
	TRACE_F ("%i", opt->nb);
	
	pp->h = calloc (opt->np, sizeof (uint32_t));
	pp->q = calloc (opt->np, sizeof (uint32_t));
	pp->b = calloc (opt->np, sizeof (uint32_t));
	
	for (uint32_t i = 0; i < opt->nb; ++ i)
	{
		pp->dmin [i] = FLT_MAX;
		if (pp->path [i] != UINT32_MAX) {continue;}
		pp_conesearch (opt->nb, pp->pos1, pp->dmin, pp->path, i, 0.3f, 0.3f, 0.3f);
	}
	//pp_deforkocf_info (opt->nb, pp->path, pp->track, &opt->np, pp->h, pp->q, 2);
	pp_deforkocf_info1 (opt->nb, pp->path, pp->track, &opt->np, pp->h, pp->b, pp->q, 2);
}


int main (int argc, char *argv[])
{
	struct Option opt;
	opt.dx = 1.0f/1000.0f;
	opt.dy = 1.0f/1000.0f;
	opt.dt = 1.0f/10.0f;
	opt.nb = 100000;
	opt.np = 10000;
	opt_init (&opt, argc, argv);
	//opt_print (&opt);
	if (opt.mode & OPT_HELP) {opt_help (&opt);return 0;}
	if (opt.src == NULL) {opt_help (&opt);return 0;}
	if (opt.des == NULL) {opt_help (&opt);return 0;}
	
	
	struct PP pp = {0};
	FILE * out = app_get_src_output (&opt);
	pp.f = app_get_src_input (&opt);
	//pp.f = fopen ("data/c1_20180627_090007.txt", "r");
	pp_load (&pp, &opt);
	
	
	/*
	for (uint32_t i = 0; i < pp.n; ++ i)
	{
		ASSERT (out != NULL);
		fprintf (out, "%10lu ", (long unsigned)pp.next2 [i]);
		fprintf (out, "%10lu ", (long unsigned)pp.pos [i].t);
		fprintf (out, "%8.2f ", (double)pp.pos [i].x);
		fprintf (out, "%8.2f ", (double)pp.pos [i].y);
		fprintf (out, "\n");
	}
	* */
	
	for (uint32_t i = 0; i < opt.nb; ++ i)
	{
		//ASSERT (out != NULL);
		//fprintf (out, "%10lu ", (long unsigned)pp.next2 [i]);
		//fprintf (out, "\n");
	}
	
	//fprintf (out, "\n");
	//fprintf (out, "\n");
	
	for (uint32_t i = 0; i < opt.np; ++ i)
	{
		ASSERT (out != NULL);
		fprintf (out, "%10lu ", (long unsigned)pp.pos0 [pp.b [i]].t);
		fprintf (out, "%10lu ", (long unsigned)pp.h [i]);
		fprintf (out, "%10lu ", (long unsigned)pp.q [i]);
		fprintf (out, "%10lu ", (long unsigned)pp.b [i]);
		fprintf (out, "\n");
	}
	
	
	ASSERT (out != NULL);
	ASSERT (pp.f != NULL);
	fclose (out);
	fclose (pp.f);
	
	return 0;
}











