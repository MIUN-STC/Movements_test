#include "debug.h"
#include "alg2.h"


#define MAX_POINTS 10000

void test2 (char const * src, char const * out)
{
	struct v4f32xyzt * p = malloc (MAX_POINTS * sizeof (struct v4f32xyzt));
	size_t * next = malloc (MAX_POINTS * sizeof (size_t));
	float * dmin = malloc (MAX_POINTS * sizeof (float));
	ASSERT (p != NULL);
	ASSERT (next != NULL);
	ASSERT (dmin != NULL);
	
	FILE * f = fopen (src, "r");
	ASSERT (f != NULL);
	size_t n = MAX_POINTS;
	v4f32xyzt_fread_all (f, &n, p);
	fclose (f);
	
	for (size_t i = 0; i < n; ++ i)
	{
		v4f32xyzt_ConeSearch (n, p, dmin, next, i, 4.0f, 10.0f, 10.0f);
	}
	
	for (size_t i = 0; i < n; ++ i)
	{
		if (next [i] == 0) {continue;}
		printf ("%05i %05i\n", (int)i, (int) next [i]);
	}
	printf ("\n");
	
	free (p);
}


int main (int argc, char *argv[])
{
	ASSERT_F (argc == 3, "provide input and output%s", "");
	test2 (argv [1], argv [2]);
	return 0;
}










