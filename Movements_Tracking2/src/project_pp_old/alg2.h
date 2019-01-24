#pragma once

#include "v4.h"

struct v4f32xyzt
{
	union
	{
		float v [4];
		struct
		{
			//2D coordinate of blob.
			float x;
			float y;
			
			//z dimension is not used currently.
			float z;
			
			//Can be used as frame number or time of frame.
			float t;
		};
	};
};


void v4f32xyzt_ConeSearch 
(
	//Number of points
	size_t n,
	
	//Point data
	struct v4f32xyzt p [], 
	
	//Minumum distance data
	float dmin [],
	
	//Linked list data
	size_t next [],
	
	//Origin point index
	size_t j,
	
	//How far away in time to look
	float tplus,
	
	//Cone start radius
	float r0,
	
	//Cone radius growth rate
	float rd
)
{
	//Break #1 when: No more points.
	//Break #2 when: Too far away in time.
	for (size_t i = j + 1; i < n; ++ i)
	{
		//Renaming to shorter variable names.
		//I hope the compiler knows that and optimizes.
		struct v4f32xyzt * a = p + j;
		struct v4f32xyzt * b = p + i;
		float const at = a->t;
		float const bt = b->t;
		float const * ap = a->v + 1;
		float const * bp = b->v + 1;
		
		//Only look forward in time.
		//Don't comare two points that are in the same frame.
		if (bt == at) {continue;}
		
		//Don't look too far away in time.
		//We can break here because we assume the points are ordered in time.
		if (bt > (at + tplus)) {break;}
		
		//Get delta vector between point a and b.
		// d := a - b
		struct v4f32xyzt d;
		v4f32_sub (d.v, ap, bp);
		
		//Calculate square distance in xy (d2) and txy (d3)
		float d2 = (d.x * d.x) + (d.y * d.y);
		float d3 = d2 + (d.t * d.t);
		
		//Calculate the search radius.
		//Start with radius (r0).
		//Grow the radius futher away in time proportional (rd).
		float r = r0 + rd * d.t;

		//Check if the point (b) is inside the search radius.
		if (d2 > r*r) {continue;}
		
		//Check if the point (b) (which are inside the search radius)
		//are the closes point to (a).
		if (d3 > dmin [i]) {continue;}
		
		//Store new minumum distance.
		dmin [i] = d3;
		
		//Next point of (j) is set to point (i).
		next [j] = i;
	}
}


int v4f32xyzt_fread 
(
	FILE * f,
	
	//Point data
	struct v4f32xyzt * p
)
{
	ASSERT (f != NULL);
	ASSERT (p != NULL);
	return fscanf (f, "%f%f%f", &p->t, &p->x, &p->y);
}


void v4f32xyzt_fread_all 
(
	FILE * f,
	
	//Number of points
	size_t * n,
	
	//Point data
	struct v4f32xyzt p []
)
{
	ASSERT (f != NULL);
	ASSERT (p != NULL);
	size_t i;
	for (i = 0; i < (*n); ++ i)
	{
		int const r = v4f32xyzt_fread (f, p + i);
		//TRACE_F ("%i", r);
		if (r < 0) {break;}
		if (r == EOF) {break;}
		ASSERT (r == 3);
	}
	(*n) = i;
}





