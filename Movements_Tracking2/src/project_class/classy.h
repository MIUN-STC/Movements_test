#pragma once

#include <stdint.h>
#include <float.h>
#include <wchar.h>
#include <xmmintrin.h>
#include <immintrin.h>

#include "v.h"
#include "v3.h"
#include "v4.h"


struct v4f32_txyz
{
	union
	{
		struct
		{
			float t;
			float x;
			float y;
			float z;
		};
		float v [4];
	};
};


struct Classy
{
	size_t n; //Count
	__attribute__ ((aligned (16))) struct v4f32_txyz * p;
	__attribute__ ((aligned (16))) uint32_t * next;
	__attribute__ ((aligned (16))) uint32_t * next2;
	__attribute__ ((aligned (16))) uint32_t * path;
	__attribute__ ((aligned (16))) uint32_t * prev;
};


void classy_calloc (struct Classy * P, size_t n)
{
	P->n = n;
	P->p = calloc (n, sizeof (struct v4f32_txyz));
	P->next = malloc (n * sizeof (uint32_t));
	P->next2 = malloc (n * sizeof (uint32_t));
	P->path = malloc (n * sizeof (uint32_t));
	P->prev = malloc (n * sizeof (uint32_t));
	for (size_t i = 0; i < P->n; ++ i) {P->path [i] = UINT32_MAX;}
	for (size_t i = 0; i < P->n; ++ i) {P->next [i] = UINT32_MAX;}
	for (size_t i = 0; i < P->n; ++ i) {P->next2 [i] = UINT32_MAX;}
	//TRACE ("");
}


void classy_rnd (struct Classy * P)
{
	for (size_t i = 0; i < P->n; ++ i)
	{
		P->p [i].x = (float)rand () / (float)RAND_MAX;
		P->p [i].y = (float)rand () / (float)RAND_MAX;
		P->p [i].t = i;
	}
}


void classy_print (struct Classy * P)
{
	printf ("%7s ", "T");
	printf ("%7s ", "X");
	printf ("%7s ", "Y");
	printf ("%5s ", "NEXT");
	printf ("%5s ", "NEXT2");
	printf ("%5s ", "PATH");
	printf ("\n");
	for (size_t i = 0; i < P->n; ++ i)
	{
		printf ("%7.3f ", P->p [i].t);
		printf ("%7.3f ", P->p [i].x);
		printf ("%7.3f ", P->p [i].y);
		if (P->next [i] != UINT32_MAX) {printf ("%5u ", P->next [i]);}
		if (P->next [i] == UINT32_MAX) {printf ("%5s ", "~");}
		if (P->next2 [i] != UINT32_MAX) {printf ("%5u ", P->next2 [i]);}
		if (P->next2 [i] == UINT32_MAX) {printf ("%5s ", "~");}
		if (P->path [i] != UINT32_MAX) {printf ("%5u ", P->path [i]);}
		if (P->path [i] == UINT32_MAX) {printf ("%5s ", "~");}
		printf ("\n");
	}
}


void classy_apply_link (struct Classy * P, size_t j)
{
	float lmin = FLT_MAX;
	for (size_t i = j + 1; i < P->n; ++ i)
	{
		float * ap = P->p [i].v;
		float * bp = P->p [j].v;
		float d [3];
		v3f32_sub (d, ap, bp);
		if (d [0] == 0) {continue;}
		if (d [0] > 4.0f) {break;}
		float r = 0.1f + 0.1f * d [0];
		float l2 = vf32_dot (d + 1, d + 1, 2);
		float l3 = vf32_dot (d + 0, d + 0, 3);
		if (l2 > (r*r)){continue;}
		if (l3 > lmin) {continue;}
		lmin = l3;
		P->next [j] = i;
	}
}


void classy_traverse (struct Classy * P)
{
	size_t n = P->n;
	uint32_t * next = P->next;
	uint32_t * next2 = P->next2;
	uint32_t * path = P->path;
	for (size_t i = 0; i < n; ++ i)
	{
		size_t j = i;
		printf ("%02i ", (int)i);
		while (1)
		{
			if (path [j] != UINT32_MAX) {break;}
			if (next [j] == UINT32_MAX) {break;}
			printf ("%02i ", (int)j);
			path [j] = i;
			next2 [j] = ((path [next [j]] == UINT32_MAX) ? next [j] : UINT32_MAX);
			j = next [j];
		}
		printf ("\n");
	}
}






void traverse_fcf (size_t n, uint32_t const x [], uint32_t s [], uint32_t p [])
{
	for (size_t i = 0; i < n; ++ i)
	{
		size_t j = i;
		while (1)
		{
			if (p [j] != UINT32_MAX) {break;}
			if (x [j] == UINT32_MAX) {break;}
			p [j] = i;
			s [j] = ((p [x [j]] == UINT32_MAX) ? x [j] : UINT32_MAX);
			j = x [j];
		}
	}
}






void uniq (size_t n, uint32_t x [])
{
	for (size_t i = 0; i < n; ++ i)
	{
		for (size_t j = i + 1; j < n; ++ j)
		{
			if (x [i] == x [j])
			{
				x [j] = UINT32_MAX;
			}
		}
	}
}



/*

__m128 _mm_hmin_ps(__m128 v)
{
    v = _mm_min_ps(v, _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 1, 0, 3)));
    v = _mm_min_ps(v, _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 0, 3, 2)));
    return v;
}


void classy_apply_link_simd (struct Classy * P, size_t j)
{
	__m128 jx = _mm_broadcast_ss ((float const *)P->x + j);
	__m128 jy = _mm_broadcast_ss ((float const *)P->y + j);
	__m128 jt = _mm_broadcast_ss ((float const *)P->t + j);
		
	for (size_t i = j/4; i < P->n; i += 4)
	{
		//TRACE_F ("i : %i", i);
		__m128 ix = _mm_load_ps (P->x + i);
		__m128 iy = _mm_load_ps (P->y + i);
		__m128 it = _mm_load_ps (P->t + i);
		__m128 dx = _mm_sub_ps (ix, jx);
		__m128 dy = _mm_sub_ps (iy, jy);
		__m128 dt = _mm_sub_ps (it, jt);
		__m128 dx2 = _mm_mul_ps (dx, dx);
		__m128 dy2 = _mm_mul_ps (dy, dy);
		__m128 dt2 = _mm_mul_ps (dt, dt);
		__m128 dr2 = _mm_mul_ps (dt, dt);
		__m128 dxy2 = _mm_add_ps (dx2, dy2);
		__m128 dxyt2 = _mm_add_ps (dxy2, dt2);
		_mm_store_ps (P->dr2 + i, dr2);
		_mm_store_ps (P->lxy + i, dxy2);
		_mm_store_ps (P->lxyt + i, dxyt2);
	}
	
	P->lxyt [j] = FLT_MAX;
	for (size_t i = j + 1; i < P->n; ++ i)
	{
		//if (P->lxy [j] > P->dr2 [i]) {continue;}
		//printf ("%li %li : %f < %f\n", j, i, P->lxyt [j], P->lxyt [i]);
		if (P->lxyt [j] < P->lxyt [i]) {continue;}
		P->lxyt [j] = P->lxyt [i];
		P->next [j] = i;
	}
}

*/

