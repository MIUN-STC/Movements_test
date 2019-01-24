#pragma once

#include "v.h"
#include "v2.h"


struct Grid
{
	//Position corner of the grid
	float p [4];
	
	//Direction 1. Grid gap.
	//Direction 2. Grid gap.
	float d1 [4];
	float d2 [4];
};



void gen_grid 
(
	//Number of vertices
	size_t n,
	
	//Data of vertices
	float v [],
	
	//Layout information on how to generate the grid.
	struct Grid const * grid
)
{
	size_t const dim = 4;
	if (n == 0) {return;}
	float const * d1 = grid->d1;
	float const * d2 = grid->d2;
	float const * p = grid->p;
	float d [4];
	float * p0;
	float * p1;
	size_t i = 0;
	while (1)
	{
		//Generate the grid in direction 1 (d1)
		if (i+0 >= n) {break;}
		p0 = v + dim * (i+0);
		vf32_mus (d, d1, i/4, dim);  //  d := d1 * i
		vf32_add (p0, p, d, dim);  // p0 :=  p + d
		if (i+1 >= n) {break;}
		p1 = v + dim * (i+1);
		vf32_mus (d, d2, n/4 - 1, dim);  //  d :=  d * n
		vf32_add (p1, p0, d, dim); // p1 := p0 + d
		
		//Generate the grid in direction 2 (d2)
		if (i+2 >= n) {break;}
		p0 = v + dim * (i+2);
		vf32_mus (d, d2, i/4, dim);  //  d := d2 * i
		vf32_add (p0, p, d, dim);  // p0 :=  p + d
		if (i+3 >= n) {break;}
		p1 = v + dim * (i+3);
		vf32_mus (d, d1, n/4 - 1, dim);  //  d := d1 * n
		vf32_add (p1, p0, d, dim); // p0 := p0 + d
		
		//Every iteration generates two new lines which is 4 points.
		i += 4;
	}
}
