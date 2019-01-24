#pragma once

#include <float.h>
#include <xmmintrin.h>
#include <immintrin.h>

#include "v.h"



//https://software.intel.com/en-us/articles/how-to-manipulate-data-structure-to-optimize-memory-use-on-32-bit-intel-architecture
//https://bwidawsk.net/blog/index.php/2015/09/gen-graphics-and-the-urb/
struct P4
{
	float x [4];
	float y [4];
	float t [4];
};




struct Classy
{
	size_t n; //Count
	size_t n4; //Count
	
	struct P4 * p4;
	
	//Align 128b = 16B for SSE instructions.
	__attribute__ ((aligned (16))) float * x; //Y Pos
	__attribute__ ((aligned (16))) float * y; //X Pos
	__attribute__ ((aligned (16))) float * t; //Time frame
	//__attribute__ ((aligned (16))) float * dxyt2; //Length
	__attribute__ ((aligned (16))) size_t * next;
	__attribute__ ((aligned (16))) size_t * next2;
	__attribute__ ((aligned (16))) size_t * prev;
};


void classy_calloc (struct Classy * P, size_t n)
{
	P->n = n;
	P->n4 = n/4;
	P->x = calloc (n, sizeof (float));
	P->y = calloc (n, sizeof (float));
	P->t = calloc (n, sizeof (float));
	P->next = calloc (n, sizeof (size_t));
	P->next2 = calloc (n, sizeof (size_t));
	P->prev = calloc (n, sizeof (size_t));
	P->p4 = calloc (n/4, sizeof (struct P4));
}


void classy_rnd (struct Classy * P)
{
	for (size_t i = 0; i < P->n; ++ i)
	{
		float * x = P->x;
		float * y = P->y;
		float * t = P->t;
		x [i] = (float)rand () / (float)RAND_MAX;
		y [i] = (float)rand () / (float)RAND_MAX;
		t [i] = i / 5;
	}
	
	for (size_t i = 0; i < P->n4; ++ i)
	{
		float * x = P->p4 [i].x;
		float * y = P->p4 [i].y;
		float * t = P->p4 [i].t;
		
		x [0] = (float)rand () / (float)RAND_MAX;
		x [1] = (float)rand () / (float)RAND_MAX;
		x [2] = (float)rand () / (float)RAND_MAX;
		x [3] = (float)rand () / (float)RAND_MAX;
		
		y [0] = (float)rand () / (float)RAND_MAX;
		y [1] = (float)rand () / (float)RAND_MAX;
		y [2] = (float)rand () / (float)RAND_MAX;
		y [3] = (float)rand () / (float)RAND_MAX;
		
		t [0] = i;
		t [1] = i;
		t [2] = i;
		t [3] = i;
	}
}


void classy_print (struct Classy * P)
{
	float * x = P->x;
	float * y = P->y;
	float * t = P->t;
	//float * dxyt2 = P->dxyt2;
	size_t * next = P->next;
	size_t * next2 = P->next2;
	
	printf ("%7s ", "X");
	printf ("%7s ", "Y");
	printf ("%7s ", "T");
	//printf ("%7s ", "DXYT2");
	printf ("%4s ", "NEXT");
	printf ("%4s ", "NEXT2");
	printf ("\n");
	
	for (size_t i = 0; i < P->n; ++ i)
	{
		printf ("%7.3f ", x [i]);
		printf ("%7.3f ", y [i]);
		printf ("%7.3f ", t [i]);
		//if (dxyt2 [i] != FLT_MAX) {printf ("%7.3f ", dxyt2 [i]);}
		//if (dxyt2 [i] == FLT_MAX) {printf ("%7s ", "inf");}
		printf ("%4i ", (int)next [i]);
		printf ("%4i ", (int)next2 [i]);
		printf ("\n");
	}
}


void classy_print2 (struct Classy * P)
{
	printf ("%3s ", "I");
	printf ("%7s ", "X");
	printf ("%7s ", "Y");
	printf ("%7s ", "T");
	printf ("\n");
	
	for (size_t i = 0; i < P->n4; ++ i)
	{

		float * x = P->p4 [i].x;
		float * y = P->p4 [i].y;
		float * t = P->p4 [i].t;
		
		printf ("%3ld ", i*4 + 0);
		printf ("%7.3f ", x [0]);
		printf ("%7.3f ", y [0]);
		printf ("%7.3f ", t [0]);
		printf ("\n");
		
		printf ("%3ld ", i*4 + 1);
		printf ("%7.3f ", x [1]);
		printf ("%7.3f ", y [1]);
		printf ("%7.3f ", t [1]);
		printf ("\n");
		
		printf ("%3ld ", i*4 + 2);
		printf ("%7.3f ", x [2]);
		printf ("%7.3f ", y [2]);
		printf ("%7.3f ", t [2]);
		printf ("\n");
		
		printf ("%3ld ", i*4 + 3);
		printf ("%7.3f ", x [3]);
		printf ("%7.3f ", y [3]);
		printf ("%7.3f ", t [3]);
		printf ("\n");
	}
}



void classy_apply_link (struct Classy * P, size_t j)
{
	float * x = P->x;
	float * y = P->y;
	float * t = P->t;
	
	float dxyt2_min = FLT_MAX;
	
	for (size_t i = j + 1; i < P->n; ++ i)
	{
		float dx = x [j] - x [i];
		float dy = y [j] - y [i];
		float dt = t [j] - t [i];
		
		if (dt == 0) {continue;}
		if (dt > 4.0f) {break;}
		
		float dr    = 6.0f + dt;
		float dxy2  = (dx * dx) + (dy * dy);
		float dxyt2 = (dx * dx) + (dy * dy) + (dt * dt);
		
		if (dxy2 > (dr * dr))  {continue;}
		if (dxyt2 > dxyt2_min) {continue;}
		dxyt2_min = dxyt2;
		P->next [j] = i;
	}
}


void classy_traverse (size_t x [], size_t y [], size_t n)
{
	for (size_t i = 0; i < n; ++ i)
	{
		size_t j = i;
		while (1)
		{
			j = x [j];
			if (y [j] != 0) {break;}
			y [j] = x [j];
			
			if (j == 0) {break;}
			printf ("%02li ", j);
		}
		printf ("\n");
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

