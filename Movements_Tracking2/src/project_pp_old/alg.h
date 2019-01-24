#pragma once

#include <stdint.h>
#include <stdio.h>

#include "v.h"
#include "v2.h"
#include "v3.h"
#include "c.h"
#include "debug.h"
#include "bufv.h"





#define POI_IGNORE 1U << 0

//Point of interests.
//Note 1: This is used as Array of Structure (AoS).
//Note 2: AoS migth have a bad perfomence compared to Structure of Array (SoA).
//https://deplinenoise.files.wordpress.com/2015/03/gdc2015_afredriksson_simd.pdf
struct POI;
struct POI
{
	//Input source 3d points (t,x,y):
	float p [3];
	
	//Output. Linked list:
	struct POI * next;
	struct POI * prev;
	
	//Output. Delta vector between (THIS) and (next):
	float d [3];
	
	//Output. Length of (d)
	float l;
	
	
	float dotavg;

	int q;
	float g;
	size_t nb;
	
	uint32_t flags;
};


void poi_init (size_t n, struct POI p [])
{
	for (size_t i = 0; i < n; ++ i)
	{
		//The length (l) must bet set maximum float number (FLT_MAX) at the initialization stage:
		p [i].l = FLT_MAX;
		
		//We dont know the (next) point at the initialization stage:
		p [i].next = NULL;
		p [i].prev = NULL;
		
		p [i].g = 0;
		p [i].dotavg = 0;
		p [i].q = 0;
		p [i].flags = 0;
		
		//Delta vector:
		p [i].d [0] = 0;
		p [i].d [1] = 0;
		p [i].d [2] = 0;
	}
}


int poi_read (FILE * f, struct POI * p)
{
	int r;
	//Rename to t, x, y:
	float * t = &p->p [0];
	float * x = &p->p [1];
	float * y = &p->p [2];
	char c;
	r = fscanf (f, "%f%f%f%c", t, x, y, &c);
	return r;
}


int poi_write (FILE * f, struct POI * p)
{
	int r;
	//Path id.
	int g = p->g;
	//Path length
	int q = p->q;
	//Frame blob count
	int nb = p->nb;
	r = fprintf (f, "%05d %05d %05d\n", g, q, nb);
	return r;
}


//Path predictor
struct PP
{
	//Points of interests.
	//This is the source of data to process.
	struct BUFV (struct POI, poi);
	
	//Number of edges.
	size_t edge_n;
	
	//Maximum length between two points.
	float lmax;
	
	//How far in the future to search for new points.
	float tplus;
	
	size_t g;
	
	size_t nb;
};


void pp_read (struct PP * ctx, char const * filename)
{
	FILE * f = fopen (filename, "r");
	ASSERT (f != NULL);
	
	//Loop forever but there will be two break scenarios.
	//break #1 when the point buffer is depleted.
	//break #2 when reading a point failed.
	while (1)
	{
		struct POI * p;
		
		//Request a buffer element to be put in (p).
		BUFV_PUSH (ctx->poi, 1, p);
		
		//The buffer might not be large enough.
		if (p == NULL) {break;}
		
		int r = poi_read (f, p);
		
		//When reading a point failed:
		//Fail 1: End of file have reached.
		//Fail 2: Error reading the file.
		if (r < 0) 
		{
			//The buffer was giving us a element before but now we dont need it.
			//So decrement the element count.
			ctx->poi.n--;
			break;
		}
		
		//vf32_print (stdout, p->p, 3, "%f ");
	}
	
	
	fclose (f);
	
	//Renaming for clarity.
	//At this point we have a buffer (p) with (n) elements filled with points.
	size_t const n = ctx->poi.n;
	struct POI * p = ctx->poi.p;
	poi_init (n, p);
	
	TRACE_F ("n : %i", n);
}


//This has large effect on point to point distance calculation.
//It is also used for adapting it for 3D rendering of (time,x,y).
void pp_transform (struct PP * ctx, float time_scale, float space_scale)
{
	size_t const n = ctx->poi.n;
	struct POI * p = ctx->poi.p;
	for (size_t i = 0; i < n; ++ i)
	{
		float s [3];
		s [0] = time_scale;
		s [1] = space_scale;
		s [2] = space_scale;
		float * v = p [i].p;
		v3f32_mul (v, v, s);
	}
}


void pp_process_distance (struct PP * ctx, size_t j)
{
	//Renaming
	struct POI * p = ctx->poi.p;
	size_t const n = ctx->poi.n;
	
	//Renaming.
	//Permutate points (a) and (b).
	struct POI * a = p + j;
	if (a->flags & POI_IGNORE) {return;}
			
	for (size_t i = j + 1; i < n; ++ i)
	{
		//Renaming.
		//Permutate points (a) and (b).
		struct POI * b = p + i;
		if (b->flags & POI_IGNORE) {continue;}
		
		float const at = a->p [0];
		float const bt = b->p [0];
		float const * ap = a->p;
		float const * bp = b->p;
		
		//Crop search in ahead of time.
		if (bt == at) {continue;}
		if (bt > (at + ctx->tplus)) {break;}

		//Experimental for point to point speed.
		//float * bd = b->d;
		//float ap0 [3];
		//v3f32_mus (ap0, bd, 0.0f);
		//v3f32_add (ap0, ap0, ap);
		
		float d [3];
		v3f32_sub (d, ap, bp);
		
		//Length functions, Calculate the length between (a) and (b):
		//Tip 1: Length can be calculated from only space or space and time.
		//Tip 2: The search radius can depend on time.
		//Note 1: This is highly unknown research area.
		//Note 2: Euclidian length should only be applied to x and y.
		//Note 3: For perfomence reasons sqrt is not needed to compare different length.
		//Note 4: The most realastic model should be cone based search.
		//Note 5: 3D vector is ordered as (t x y) <=> (v[0] v[1] v[2])
		
		float l = (d [1] * d [1]) + (d [2] * d [2]) + (d [0] * d [0]);
		float l_xy = (d [1] * d [1]) + (d [2] * d [2]);
		float r = 0.4f + 0.6f * d [0];
		
		//Ignore points outside length limit and 
		//points that are further away than already found closest point.
		if (l_xy > r*r) {continue;}
		if (l > a->l) {continue;}
		
		//v3f32_cpy (bd, d);
		
		//Store the length to be compared again with succeding (b):
		a->l = l;
		
		//Link the closest (b) to (a):
		a->next = b;
		
		//Stats:
		ctx->edge_n ++;
	}
}


//This automaticly builds a doubly linked list from a linked list.
//Doubly linked list is good for traversing two directions 
//and can be used to check initiated paths and terminated paths.
//Scenario 1: If (next) is NULL            : Terminated path
//Scenario 2: If (prev) is NULL            : Initiated path
//Scenario 3: If (prev) and (next) is NULL : No path
void pp_process_backtrack (struct PP * ctx)
{
	BUFV_LOOP (ctx->poi, struct POI *, e)
	{
		if (e->next == NULL) {continue;}
		//if (e->prev != NULL) {continue;}
		e->next->prev = e;
	}
}


void pp_process_grouper (struct PP * pp)
{
	BUFV_LOOP (pp->poi, struct POI *, a)
	{
		if (a->prev != NULL) {continue;}
		pp->g ++;
		struct POI * b = a;
		while (1)
		{
			if (b == NULL) {break;}
			if (b->next == b) {break;}
			b->g = pp->g;
			if (b->next && b->next->g > 0) 
			{
				b->next = NULL;
				break;
			}
			b = b->next;
		}
	}
}



float poi_linkdot (struct POI * p)
{
	if (p == NULL) {return 0;}
	if (p->next == NULL) {return 0;}
	if (p->next->next == NULL) {return 0;}
	float * a = p->p + 1;
	float * b = p->next->p + 1;
	float * c = p->next->next->p + 1;
	float u [2];
	float v [2];
	v2f32_sub (u, a, b);
	v2f32_sub (v, b, c);
	return fabs (v2f32_dot (u, v));
}


void pp_process_dotavg (struct PP * pp)
{
	BUFV_LOOP (pp->poi, struct POI *, a)
	{
		if (a->prev != NULL) {continue;}
		struct POI * b = a;
		int q = 0;
		float dotavg = 0;
		while (1)
		{
			if (b == NULL) {break;}
			if (b->next == NULL) {break;}
			if (b->next == b) {break;}
			q++;
			dotavg += poi_linkdot (b);
			b = b->next;
		}
		dotavg = dotavg / q;
		b = a;
		while (1)
		{
			if (b == NULL) {break;}
			if (b->next == NULL) {break;}
			if (b->next == b) {break;}
			b->q = q;
			b->dotavg = dotavg;
			b = b->next;
		}
	}
}


void pp_process_bcount (struct PP * pp)
{
	for (struct POI * a = pp->poi.p; a < (pp->poi.p + pp->poi.n); ++ a)
	{
		size_t n = 0;
		struct POI * b;
		for (b = a; b < (pp->poi.p + pp->poi.n); ++ b)
		{
			float at = a->p [0];
			float bt = b->p [0];
			if (at != bt) {break;}
			n ++;
		}
		//TRACE_F ("%i", n);
		for (b = a; b < (pp->poi.p + pp->poi.n); ++ b)
		{
			float at = a->p [0];
			float bt = b->p [0];
			if (at != bt) {break;}
			b->nb = n;
		}
		a = b-1;
	}
}



void pp_process_filter (struct PP * pp)
{
	for (struct POI * a = pp->poi.p; a < (pp->poi.p + pp->poi.n); ++ a)
	{
		if (a->nb > 30)
		{
			a->flags |= POI_IGNORE;
		}
	}
}



//Temporary
void load_pp (struct PP * pp, char const * filename)
{
	pp->edge_n = 0;
	BUFV_INIT (struct POI, pp->poi, 1000000);
	pp_read (pp, filename);
	pp_transform (pp, 1.0f/40.0f, 1.0f/1000.0f);
	pp->tplus = 0.9f;
	pp->lmax = 0.1f;
	pp->g = 0;
}
