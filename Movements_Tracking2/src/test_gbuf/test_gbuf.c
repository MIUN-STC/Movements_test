#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "tcol.h"
#include "gbuf.h"
#include "debug.h"
#include "global.h"

struct Vec2
{
	float x;
	float y;
};


void vec2_print (struct Vec2 * v, size_t n)
{
	for (size_t i = 0; i < n; i ++)
	{
		printf ("%f %f\n", v [i].x, v [i].y);
	}
}

void vec2_add_values (struct Vec2 * v, size_t n)
{
	for (size_t i = 0; i < n; i ++)
	{
		v [i].x = 1 * i;
		v [i].y = 2 * i;
	}
}


void vec2_print1 (struct Vec2 * v)
{
	GBUF_LOOP_CAPACITY (size_t, i, v)
	{
		printf ("%f %f\n", v [i].x, v [i].y);
	}
}

void vec2_add_values1 (struct Vec2 * v)
{
	GBUF_LOOP_CAPACITY (size_t, i, v)
	{
		v [i].x = 1 * i;
		v [i].y = 2 * i;
	}
}



int main (void)
{
	int count = 4;
	struct Vec2 * v0 = malloc (count * sizeof (struct Vec2));
	struct Vec2 * v1 = gbuf_init (NULL, count, sizeof (struct Vec2), GBUF_MALLOC);
	

	vec2_add_values (v0, count);
	vec2_print (v0, count);
	
	printf ("\n\n");
	
	vec2_add_values1 (v1);
	vec2_print1 (v1);
	
	return 0;
}





