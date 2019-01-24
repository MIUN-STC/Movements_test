#pragma once

#include "v.h"
#include "c.h"
#include "poi.h"
#include "gen2.h"
#include "vertex.h"


struct v3f32_SIMD4_TXY
{
	float t [4]; //Time  T axis (SIMD)
	float x [4]; //Space X axis (SIMD)
	float y [4]; //Space Y axis (SIMD)
};


void poi_distance (size_t n, struct v3f32_SIMD4_TXY p [])
{
	for (size_t i = 0; i < n; ++ i)
	{

	}
}


//Read 4 points
int poi_read4 (FILE * f, struct v3f32_SIMD4_TXY * p)
{
	int r;
	for (size_t i = 0; i < 4; ++ i)
	{
		float * t = p->t + i;
		float * x = p->x + i;
		float * y = p->y + i;
		char c;
		r = fscanf (f, "%f%f%f%c", t, x, y, &c);
		if (r < 0) {break;}
	}
	return r;
}


//This has large effect on point to point distance calculation.
//It is also used for adapting it for 3D rendering of (time,x,y).
void poi_transform (size_t n, struct v3f32_SIMD4_TXY p [])
{
	for (size_t i = 0; i < n; ++ i)
	{
		v4f32_mus (p [i].t, p [i].t, 1.0/10.0f);
		v4f32_mus (p [i].x, p [i].x, 1.0/1000.0f);
		v4f32_mus (p [i].y, p [i].y, 1.0/1000.0f);
	}
}


//Read 4 points then continue read 4 until no points are available in the file.
void poi_readfile4 (int * n, struct v3f32_SIMD4_TXY p [], char const * filename)
{
	FILE * f = fopen (filename, "r");
	ASSERT (f != NULL);
	size_t i;
	for (i = 0; i < n [0]; ++ i)
	{
		int r = poi_read4 (f, p + i);
		if (r < 0) {break;}
	}
	fclose (f);
	TRACE_F ("n : %i", i);
	poi_transform (i, p);
	n [0] = i;
}

















void vertex_poi_copy4 (struct Vertex4 * v, struct v3f32_SIMD4_TXY const * p)
{
	for (size_t i = 0; i < 4; ++ i)
	{
		v->v [i].p [2] = p->t [i];
		v->v [i].p [0] = p->x [i];
		v->v [i].p [1] = -p->y [i];
		v->v [i].p [3] = 1.0f;
	}
}


void vertex_from_poi 
(
	size_t n, 
	struct Vertex4 v [], 
	struct v3f32_SIMD4_TXY const p [], 
	struct v4f32_RGBA color
)
{
	for (size_t i = 0; i < n; ++ i)
	{
		v [i].v [0].c_rgba = color;
		v [i].v [1].c_rgba = color;
		v [i].v [2].c_rgba = color;
		v [i].v [3].c_rgba = color;
	}
	for (size_t i = 0; i < n; ++ i)
	{
		vertex_poi_copy4 (v + i, p + i);
	}
}
