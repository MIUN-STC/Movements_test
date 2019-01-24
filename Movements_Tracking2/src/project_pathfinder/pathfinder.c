#include <stdio.h>
#include <unistd.h>

#include "debug.h"
#include "mat.h"
#include "global.h"
#include "poi.h"


#define TEMP_RELINK (1 << 0)

void temp_update1 
(
	struct POI_Context * ctx, 
	struct POI * x0,
	size_t n,
	uint32_t flags
)
{
	ctx->g = 1;
	poi_reset (x0, n);
	dev_search1_distance (ctx, x0, n);
	dev_search3_backtrack (ctx, x0, n);
	if (flags & TEMP_RELINK)
	{
		dev_search4_relink (ctx, x0, n);	
	}
	dev_search2_length (ctx, x0, n);
	dev_search5_group (ctx, x0, n);
}


void filter_bird (struct POI * x0, size_t n)
{
	for (size_t i = 0; < n; i = i + 1)
	{
		if (x0->)
	}
}


int main (int argc, char * argv [])
{
	ASSERT_F (argc == 1, "No arg allowed");
	ASSERT_F (argv != NULL, "NULL error");
	
	struct POI_Context ctx;
	ctx.g = 1;
	ctx.s [POI_T] = 1.0f;
	ctx.s [POI_X] = 1.0f;
	ctx.s [POI_Y] = 1.0f;
	struct POI p1v [10000];
	struct POI p2v [10000];
	FILE * p1f = fopen ("poi1.txt", "r");
	FILE * p2f = fopen ("poi2.txt", "r");
	int p1n = poi_load (p1f, p1v, COUNTOF (p1v));
	int p2n = poi_load (p2f, p2v, COUNTOF (p2v));
	fclose (p1f);
	fclose (p2f);
	
	temp_update1 (&ctx, p1v, p1n, TEMP_RELINK);
	temp_update1 (&ctx, p2v, p2n, TEMP_RELINK);


	return 0;
}
