#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "v3.h"
#include "v4.h"


void pp_pr (uint32_t x)
{
	if (x == UINT32_MAX)
	{
		printf ("%5s", "-");
	}
	else
	{
		printf ("%5lu", (long unsigned) x);
	}
}


struct pp_v4f32_txy1
{
	union
	{
		float v [4];
		struct
		{
			//Can be used as frame number or time of frame.
			float t;
			
			//2D coordinate of blob.
			float x;
			float y;
			
			//w dimension is not used currently.
			float w;
		};
	};
};


void pp_conesearch 
(
	//Number of points
	uint32_t n,
	
	//Point data
	struct pp_v4f32_txy1 p [], 
	
	float D [],
	
	//Minumum distance data
	float dmin [],
	
	//Linked list data
	uint32_t next [],
	
	//Linked list data
	//uint32_t qnext [],
	
	//Origin point index
	uint32_t o,
	
	//How far away in time to look
	float tplus,
	
	//Cone start radius
	float r0,
	
	//Cone radius growth rate
	float rd,
	
	float sx,
	float sy,
	float st,
	float sD
)
{
	//Break #1 when: No more points.
	//Break #2 when: Too far away in time.
	uint32_t i;
	for (i = o + 1; i < n; ++ i)
	{
		//Renaming to shorter variable names.
		//I hope the compiler knows that and optimizes.
		struct pp_v4f32_txy1 * a = p + o;
		struct pp_v4f32_txy1 * b = p + i;
		float const at = a->t;
		float const bt = b->t;
		float const * ap = a->v;
		float const * bp = b->v;
		
		//Only look forward in time.
		//Don't comare two points that are in the same frame.
		if (bt == at) {continue;}
		
		//Don't look too far away in time.
		//We can break here because we assume the points are ordered in time.
		if (bt > (at + tplus)) {break;}
		
		//Get delta vector between point a and b.
		// d := a - b
		struct pp_v4f32_txy1 d;
		v4f32_sub (d.v, bp, ap);
		float dD = D [i] - D [o];
		
		//Calculate square distance in xy (d2) and txy (d3)
		float d2 = (d.x * d.x) + (d.y * d.y);
		float d3 = (sx * (d.x * d.x)) + (sy * (d.y * d.y)) + (st * (d.t * d.t)) + (sD * (dD * dD));
		//float d3 = (sx * (d.x * d.x)) + (sy * (d.y * d.y)) + (st * (d.t * d.t));
		//float d3 = d.t;
		
		//Calculate the search radius.
		//Start with radius (r0).
		//Grow the radius along time, proportional to (rd).
		ASSERT (d.t >= 0.0f);
		float r = r0 + rd * d.t;

		//Check if the point (b) are inside the search radius.
		if (d2 > r*r) {continue;}
		
		//Check if the point (b) (which are inside the search radius)
		//are the closes point to (a).
		if (d3 > dmin [o]) {continue;}
		
		//Store new minumum distance.
		dmin [o] = d3;
		
		//Next point of (j) is set to point (i).
		next [o] = i;
	}
}


int pp_fread 
(
	FILE * f,
	
	//Point data
	struct pp_v4f32_txy1 * p,
	
	float * D,
	
	uint32_t * nbf
)
{
	ASSERT (f != NULL);
	ASSERT (p != NULL);
	return fscanf (f, "%f%f%f%f%u", &p->t, &p->x, &p->y, D, nbf);
}


void pp_fread_all 
(
	FILE * f,
	
	//Number of points
	uint32_t * n,
	
	//Point data
	struct pp_v4f32_txy1 p [],
	
	float D [],
	
	uint32_t nbf []
)
{
	ASSERT (f != NULL);
	ASSERT (p != NULL);
	ASSERT (n != NULL);
	
	uint32_t i;
	for (i = 0; i < (*n); ++ i)
	{
		int const r = pp_fread (f, p + i, D + i, nbf + i);
		//TRACE_F ("%i", r);
		if (r < 0) {break;}
		if (r == EOF) {break;}
		ASSERT (r == 5);
	}
	(*n) = i;
}


//Outputs pair of points by using indiced from (next)
void generate_nextchain_edge 
(
	uint32_t * ne,
	void * edges,
	uint32_t np, 
	uint32_t const p [], 
	void * vertices,
	uint32_t stride
)
{
	ASSERT (ne != NULL);
	ASSERT (edges != NULL);
	ASSERT (p != NULL);
	ASSERT (vertices != NULL);
	ASSERT (((*ne) % 2) == 0);
	
	//Rename to shorter and use 8 bit byte grid.
	uint8_t * e = (uint8_t *) edges;
	uint8_t * v = (uint8_t *) vertices;
	//Create iterator (ie) for edges.
	size_t ie = 0;
	//Traverse paths array (p) using path iterator (ip).
	//Every edge is defined as (ip, p [ip]).
	//Copy two vertices from (v) into edges (e) for each path element.
	for (uint32_t ip = 0; ip < np; ++ ip)
	{
		//TRACE_F ("%05i %05i", (int)i, (int) next [i]);
		
		//Edges guard.
		if (ie >= (*ne)) {break;}
		
		//Ignore path element that is end of path.
		if (p [ip] == UINT32_MAX) {continue;}
		
		memcpy (e + stride * (ie + 0), v + stride * ip    , stride);
		memcpy (e + stride * (ie + 1), v + stride * p [ip], stride);
		ie += 2;
	}
	//Output number of edges.
	(*ne) = ie;
}


//TODO: This does not seem to remove a fork at the end.
//Check if it is possible to fix that. It is not currently a big problem.
void pp_deforkocf
(
	uint32_t n, 
	
	//Input path linked list
	uint32_t const p [], 
	
	//Output track linked list
	uint32_t t []
)
{
	ASSERT (p != NULL);
	ASSERT (t != NULL);
	for (uint32_t i = 0; i < n; ++ i)
	{
		ASSERT ((p [i] == UINT32_MAX) || p [i] < n);
	}
	
	for (uint32_t i = 0; i < n; ++ i)
	{
		//Create iterator (j) for traversing the path (i).
		uint32_t j = i;
		//Traverse path:
		//Break #1 when at end of path.
		//Break #2 when a path is already occupied by a track.
		while (1)
		{
			if (p [j] == UINT32_MAX) {break;}
			if (t [j] != UINT32_MAX) {break;}
			if (t [p [j]] != UINT32_MAX) {break;}
			//pp_pr (p [j]);
			t [j] = p [j];
			j = p [j];
		}
		//printf ("\n");
	}
}



//Defork paths info tracks.
//Same as (pp_deforkocf) but with more outputs.
void pp_deforkocf_info
(
	//Input
	uint32_t n, 
	
	//Input path linked list
	uint32_t const p [], 
	
	//Output track linked list
	uint32_t t [],
	
	//Output/Input
	uint32_t * nh,
	
	//Output start indices of tracks
	uint32_t h [],
	
	//Output track lengths
	uint32_t q [],
	
	//Minumum track length
	uint32_t qmin
)
{
	ASSERT (p != NULL);
	ASSERT (t != NULL);
	ASSERT (nh != NULL);
	ASSERT (h != NULL);
	ASSERT (q != NULL);
	for (uint32_t i = 0; i < n; ++ i)
	{
		ASSERT ((p [i] == UINT32_MAX) || p [i] < n);
	}
	
	uint32_t ih = 0;
	for (uint32_t ip = 0; ip < n; ++ ip)
	{
		//Create a counter to calculate the length of path.
		uint32_t nq = 0;
		//Create iterator (j) for traversing the path (p).
		uint32_t j = ip;
		//Traverse path:
		//Break #1 when at end of path.
		//Break #2 when a path is already occupied by a track.
		while (1)
		{
			//Incrament the path length counter first because
			//one path element is allways length 1.
			nq ++;
			if (p [j] == UINT32_MAX) {break;}
			if (t [j] != UINT32_MAX) {break;}
			if (t [p [j]] != UINT32_MAX) {break;}
			//Track (t) occupies the path (p) at index (j)
			t [j] = p [j];
			//Goto next path element
			j = p [j];
		}
		//fprintf (stderr, "%i\n", ip);
		//Start capturing the interesting tracks by looking at the path length.
		if (nq >= qmin)
		{
			if (ih >= (*nh)) {break;}
			h [ih] = ip;
			q [ih] = nq;
			ih ++;
		}
		
	}
	//Output the array count.
	(*nh) = ih;
}




//Defork paths info tracks.
//Same as (pp_deforkocf) but with more outputs.
void pp_deforkocf_info1
(
	//Input
	uint32_t n, 
	
	//Input path linked list
	uint32_t const p [], 
	
	//Output track linked list
	uint32_t t [],
	
	
	
	
	//Output/Input
	uint32_t * nh,
	
	//Output start indices of tracks
	uint32_t h [],
	
	//Output track lengths
	uint32_t b [],
	
	//Output track lengths
	uint32_t q [],
	
	//Minumum track length
	uint32_t qmin
)
{
	ASSERT (p != NULL);
	ASSERT (t != NULL);
	ASSERT (nh != NULL);
	ASSERT (h != NULL);
	ASSERT (q != NULL);
	for (uint32_t i = 0; i < n; ++ i)
	{
		ASSERT ((p [i] == UINT32_MAX) || p [i] < n);
	}
	
	uint32_t ih = 0;
	for (uint32_t ip = 0; ip < n; ++ ip)
	{
		//Create a counter to calculate the length of path.
		uint32_t nq = 0;
		//Create iterator (j) for traversing the path (p).
		uint32_t j = ip;
		//Traverse path:
		//Break #1 when at end of path.
		//Break #2 when a path is already occupied by a track.
		while (1)
		{
			//Incrament the path length counter first because
			//one path element is allways length 1.
			nq ++;
			if (p [j] == UINT32_MAX) {break;}
			if (t [j] != UINT32_MAX) {break;}
			if (t [p [j]] != UINT32_MAX) {break;}
			//Track (t) occupies the path (p) at index (j)
			t [j] = p [j];
			//Goto next path element
			j = p [j];
		}
		
		if (nq >= qmin)
		{
			j = ip;
			while (1)
			{
				if (j == UINT32_MAX) {break;}
				if (ih >= (*nh)) {break;}
				h [ih] = ip;
				q [ih] = nq;
				b [ih] = j;
				ih ++;
				j = t [j];
			}
		}
		
	}
	//Output the array count.
	(*nh) = ih;
}


//Defork paths info tracks.
//Same as (pp_deforkocf) but with more outputs.
void pp_deforkocf_info2
(
	//Input.
	uint32_t n, 
	
	//Input. Paths as next index notation.
	uint32_t const p [], 
	
	//Output. Tracks as next index notation.
	uint32_t t [],
	
	//Input. Blob diameter
	float d [],
	
	//Input. Blob diameter passthrough
	float dpass,
	
	
	
	
	//Input: Number of elements in the following blob arrays.
	//Ouput: Number of elements that is useable in the following blob arrays.
	uint32_t * nb,
	
	//Output: Blob start index
	uint32_t bstart [],
	
	//Output: Blob current index
	uint32_t bcurrent [],
	
	//Output: Blob next index
	uint32_t bnext [],
	
	//Output: Number of blobs the track.
	uint32_t bquantity [],
	
	//Input: Minumum track length
	uint32_t qmin
)
{
	ASSERT (p);
	ASSERT (t);
	ASSERT (nb);
	ASSERT (bstart);
	ASSERT (bquantity);
	ASSERT (bcurrent);
	ASSERT (bnext);
	for (uint32_t i = 0; i < n; ++ i)
	{
		ASSERT ((p [i] == UINT32_MAX) || p [i] < n);
	}
	
	uint32_t ib = 0;
	for (uint32_t ip = 0; ip < n; ++ ip)
	{
		//Create a counter to calculate the length of path.
		uint32_t nq = 0;
		//Create iterator (j) for traversing the path (p).
		uint32_t j = ip;
		//Traverse path:
		//Break #1 when at end of path.
		//Break #2 when a path is already occupied by a track.
		while (1)
		{
			//Incrament the path length counter first because
			//one path element is allways length 1.
			nq ++;
			if (p [j] == UINT32_MAX) {break;}
			if (t [j] != UINT32_MAX) {break;}
			if (t [p [j]] != UINT32_MAX) {break;}
			//Track (t) occupies the path (p) at index (j)
			t [j] = p [j];
			//Goto next path element
			j = p [j];
		}
		
		if ((nq >= qmin) || (d [ip] > dpass))
		{
			j = ip;
			while (1)
			{
				if (j == UINT32_MAX) {break;}
				if (ib >= (*nb)) {break;}
				bstart    [ib] = ip;
				bquantity [ib] = nq;
				bcurrent  [ib] = j;
				bnext     [ib] = t [j];
				ib ++;
				j = t [j];
			}
		}
		
	}
	//Output the array count.
	(*nb) = ib;
}
