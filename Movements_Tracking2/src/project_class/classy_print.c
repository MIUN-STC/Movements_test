#include <stdio.h>
#include <locale.h>
#include <wchar.h>

#include "global.h"
#include "debug.h"
#include "classy.h"


int main (int argc, char *argv[])
{
	setlocale (LC_ALL, "");
	
	struct Classy P;
	classy_calloc (&P, 40);
	classy_rnd (&P);

	for (size_t i = 0; i < P.n; ++ i)
	{
		classy_apply_link (&P, i);
	}
	
	//classy_traverse (&P);
	traverse_fcf (P.n, P.next, P.next2, P.path);
	//memcpy (P.next2, P.next, sizeof (uint32_t) * P.n);
	//uniq (P.n, P.next2);
	classy_print (&P);
	
	return 0;
}











