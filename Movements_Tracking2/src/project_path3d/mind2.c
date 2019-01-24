#include "debug.h"
#include "alg.h"

#define MAX_IDS 10000


size_t sum (size_t n, size_t x [])
{
	size_t s = 0;
	for (size_t i = 0; i < n; ++i)
	{
		s = s + x [i];
	}
	return s;
}

void test2 (char const * src, char const * out)
{
	struct PP pp;
	//load_pp (&pp, "data/c2_20180626_190007.txt");
	load_pp (&pp, src);
	pp_process_bcount (&pp);
	pp_process_filter (&pp);
	for (size_t i = 0; i < pp.poi.n; ++ i)
	{
		pp_process_distance (&pp, i);
	}
	pp_process_backtrack (&pp);
	pp_process_grouper (&pp);
	pp_process_dotavg (&pp);
	printf ("Vertices : %li\n", pp.poi.n);
	printf ("Edges    : %li\n", pp.edge_n);
	printf ("Groups   : %li\n", pp.g);
	
	size_t bird [MAX_IDS] = {0};
	//FILE * f = fopen ("hello2.txt", "w");
	FILE * f = fopen (out, "w");
	size_t i = 0;
	BUFV_LOOP (pp.poi, struct POI *, e)
	{
		poi_write (f, e);
		if (e->q > 12 && e->q < 14)
		{
			bird [i] ++;
		}
		i ++;
	}
	fclose (f);
	printf ("Birds    : %li\n", sum (MAX_IDS, bird));
}



int main (int argc, char *argv[])
{
	ASSERT_F (argc == 3, "provide input and output%s", "");
	test2 (argv [1], argv [2]);
	return 0;
}










