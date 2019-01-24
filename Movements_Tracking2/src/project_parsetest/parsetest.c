#include "common.h"
#include "debug.h"
#include "poi.h"


int main(int argc, char** argv)
{
	struct POI x [10] = {0};
	x [0].m = 4;
	x [1].m = 3;
	x [2].m = 2;
	x [3].m = 1; x [3].p [0] = 123.123f;
	
	
	FILE * f = fopen ("hello.txt", "w+");
	
	printf ("Save file\n");
	poi_save (stdout, x, COUNTOF (x));
	poi_save (f, x, COUNTOF (x));
	
	//rewind (f);
	fseek (f, 0, SEEK_SET);
	printf ("\n");
	memset (x, 0, sizeof (x));
	printf ("Load file\n");
	int n = poi_load (f, x, COUNTOF (x));
	printf ("n %i:\n", n);
	poi_save (stdout, x, n);
	
	fclose (f);
	
	return 0;
}
