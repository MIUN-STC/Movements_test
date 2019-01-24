#include "common.h"
#include "debug.h"
#include "poi.h"


int main(int argc, char** argv)
{
	FILE * f = fopen ("poi1.txt", "r");
	struct POI x [10000];
	int n = poi_load (f, x, COUNTOF (x));
	printf ("n %i:\n", n);
	poi_save (stdout, x, n);
	fclose (f);
	return 0;
}
