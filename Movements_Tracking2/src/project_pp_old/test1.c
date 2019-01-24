#include "bufv.h"


struct GG
{
	struct BUFV (int, v);
};

int main (int argc, char *argv[])
{
	struct GG gg;
	
	BUFV_INIT (int, gg.v, 10);
	
	int i = 0;
	while (1)
	{
		i++;
		int * x;
		BUFV_PUSH (gg.v, 1, x);
		if (x == NULL) {break;}
		(*x) = i;
	}
	
	
	for (size_t j = 0; j < gg.v.n; ++ j)
	{
		printf ("%i %i\n", gg.v.p [j], (int)gg.v.n);
	}
	

	return 0;
}










