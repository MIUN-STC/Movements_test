#pragma once

#include "gbuf.h"
#include "vertex.h"
#include "map.h"
#include "v.h"
#include "v4.h"
#include "c.h"


struct Grid
{
	//Position corner of the grid
	union
	{
		float p [4];
		struct v4f32_XYZW p_xyzw;
	};
	
	//Direction 1. Grid gap.
	union
	{
		float d1 [4];
		struct v4f32_XYZW d1_xyzw;
	};
	
	//Direction 2. Grid gap.
	union
	{
		float d2 [4];
		struct v4f32_XYZW d2_xyzw;
	};
	
	//Color.
	union
	{
		float c [4];
		struct v4f32_RGBA c_rgba;
	};
};


void gen_grid 
(
	size_t n,
	struct Vertex * v,
	struct Grid const * grid
)	
{
	if (n == 0) {return;}
	float const * d1 = grid->d1;
	float const * d2 = grid->d2;
	float const * p = grid->p;
	for (size_t i = 0; i < n; ++ i)
	{
		v4f32_cpy (v [i].c, grid->c);
	}
	float d [4];
	float * p0;
	float * p1;
	//p0 := p + d1*i
	//p1 := p0 + d2*n
	//p0 := p + d2*i
	//p1 := p0 + d1*n
	size_t i = 0;
	while (1)
	{
		if (i+0 >= n) {break;}
		p0 = v [i+0].p;
		v4f32_mus (d, d1, i/4);  //  d := d1 * i
		v4f32_add (p0, p, d);  // p0 :=  p + d
		if (i+1 >= n) {break;}
		p1 = v [i+1].p;
		v4f32_mus (d, d2, n/4 - 1);  //  d :=  d * n
		v4f32_add (p1, p0, d); // p1 := p0 + d
		if (i+2 >= n) {break;}
		p0 = v [i+2].p;
		v4f32_mus (d, d2, i/4);  //  d := d2 * i
		v4f32_add (p0, p, d);  // p0 :=  p + d
		if (i+3 >= n) {break;}
		p1 = v [i+3].p;
		v4f32_mus (d, d1, n/4 - 1);  //  d := d1 * n
		v4f32_add (p1, p0, d); // p0 := p0 + d
		i += 4;
	}
}
