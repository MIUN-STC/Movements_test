#include <stdint.h>
#include "global.h"
#include "debug.h"

//Path predictor
#include "pp.h"

int main (int argc, char *argv[])
{

	//uint32_t x [] = {4, 3, 4, 6, 7, 6, 8, 8, UINT32_MAX};
	uint32_t x [] = {2,  3,  4,  5,  6,  7,  7,  8,  UINT32_MAX};
	uint32_t p [COUNTOF (x)];
	for (uint32_t i = 0; i < COUNTOF (x); ++ i) {p [i] = UINT32_MAX;}
	pp_deforkocf (COUNTOF (x), x, p);
	printf ("\n");
	printf ("\n");
	
	for (uint32_t i = 0; i < COUNTOF (x); ++ i){pp_pr (i);}
	printf ("\n");
	
	for (uint32_t i = 0; i < COUNTOF (x); ++ i){pp_pr (x [i]);}
	printf ("\n");
	
	for (uint32_t i = 0; i < COUNTOF (x); ++ i){pp_pr (p [i]);}
	printf ("\n");
	
	return 0;
}











