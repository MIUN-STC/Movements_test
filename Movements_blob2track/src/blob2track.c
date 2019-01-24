#include "pp.h"
#include "option.h"

#include "mem.h"



struct pather
{
	//Blob position original
	struct pp_v4f32_txy1 * pos0;
	
	//Number of blobs per frames
	uint32_t * nbf;
	
	//Paths and tracks as (Next Index Notation)
	uint32_t * path;
	uint32_t * track;
	
	//Minmum distance
	float * dmin;
	
	//Blob diameter
	float * D;
};



struct relationship
{
	uint32_t * r_start;
	uint32_t * r_current;
	uint32_t * r_next;
	uint32_t * r_quantity;
};



void pather_load (struct pather * ph, struct relationship * rs, struct Option * opt, FILE * f)
{
	ph->D = calloc (opt->nb, sizeof (float));
	ph->dmin = calloc (opt->nb, sizeof (float));
	ph->pos0 = calloc (opt->nb, sizeof (struct pp_v4f32_txy1));
	ph->nbf = calloc (opt->nb, sizeof (uint32_t));
	ph->path = calloc (opt->nb, sizeof (uint32_t));
	ph->track = calloc (opt->nb, sizeof (uint32_t));
	ASSERT (ph->D);
	ASSERT (ph->dmin);
	ASSERT (ph->pos0);
	ASSERT (ph->nbf);
	ASSERT (ph->path);
	ASSERT (ph->track);
	vu32_set1 (opt->nb, ph->path, UINT32_MAX);
	vu32_set1 (opt->nb, ph->track, UINT32_MAX);
	pp_fread_all (f, &opt->nb, ph->pos0, ph->D, ph->nbf);
	TRACE_F ("nb %i", opt->nb);
	

	
	for (uint32_t i = 0; i < opt->nb; ++ i)
	{
		//Ignore when too many blobs in one frame.
		if (ph->nbf [i] > opt->nbf_max) {continue;}
		if (ph->path [i] != UINT32_MAX) {continue;}
		ph->dmin [i] = FLT_MAX;
		pp_conesearch 
		(
			opt->nb, 
			ph->pos0, 
			ph->D, 
			ph->dmin, 
			ph->path, 
			i, 
			opt->tplus,
			opt->r0, 
			opt->rk,
			opt->sx,
			opt->sy,
			opt->st,
			opt->sD
		);
	}
	
	rs->r_start = calloc (opt->nr, sizeof (uint32_t));
	rs->r_next = calloc (opt->nr, sizeof (uint32_t));
	rs->r_current = calloc (opt->nr, sizeof (uint32_t));
	rs->r_quantity = calloc (opt->nr, sizeof (uint32_t));
	ASSERT (rs->r_start);
	ASSERT (rs->r_next);
	ASSERT (rs->r_current);
	ASSERT (rs->r_quantity);
	
	//pp_deforkocf_info (opt->nb, pp->path, pp->track, &opt->np, pp->h, pp->q, 2);
	//pp_deforkocf_info1 (opt->nb, pp->path, pp->track, &opt->np, pp->h, pp->b, pp->q, 2);
	pp_deforkocf_info2 
	(
		opt->nb, 
		ph->path, 
		ph->track, 
		ph->D, 
		opt->Dpass, 
		&opt->nr, 
		rs->r_start, 
		rs->r_current, 
		rs->r_next, 
		rs->r_quantity, 
		opt->qmin
	);
	
	TRACE_F ("nr %i", opt->nr);
}


int main (int argc, char *argv[])
{
	struct Option opt;
	opt.r0 = 100.0f;
	opt.rk = 100.0f;
	opt.tplus = 4.0f;
	opt.sx = 0.1f;
	opt.sy = 0.1f;
	opt.st = 1.0f;
	opt.sD = 1.0f;
	opt.nb = 100000;
	opt.nr = 100000;
	opt.nbf_max = 100;
	opt.qmin = 2;
	opt.Dpass = 20;
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	if (opt.mode & OPT_HELP) {opt_help (&opt);return 0;}
	
	
	struct pather pp = {0};
	struct relationship tr = {0};
	
	//Select input source. Use stdin as default.
	FILE * fin = stdin;
	if (opt.src) {fin = fopen (opt.src, "r");}
	ASSERT (fin);
	
	//Select outout destination. Use stdout as default.
	FILE * fout = stdout;
	if (opt.des) {fout = fopen (opt.des, "w+");}
	ASSERT (fout);
	
	//Load data from the input source.
	//pp.f = fopen ("data/c1_20180627_090007.txt", "r");
	pather_load (&pp, &tr, &opt, fin);


	for (uint32_t i = 0; i < opt.nr; ++ i)
	{
		ASSERT (fout != NULL);
		//fprintf (fout, "%10lu ", (long unsigned)pp.pos0 [tr.r_start [i]].t);
		fprintf (fout, "%10lu ", (long unsigned)tr.r_start [i]);
		fprintf (fout, "%10lu ", (long unsigned)tr.r_current [i]);
		fprintf (fout, "%10lu ", (long unsigned)tr.r_next [i]);
		fprintf (fout, "%10lu ", (long unsigned)tr.r_quantity [i]);
		//fprintf (out, "%e ", (double)pp.D [pp.start [i]]);
		fprintf (fout, "\n");
	}
	
	
	ASSERT (fout);
	ASSERT (fin);
	fclose (fout);
	fclose (fin);
	
	return 0;
}











