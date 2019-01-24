#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "tcol.h"
#include "debug.h"
#include "global.h"
#include "vertex.h"


struct BufK
{
	void * a;
	void * b;
};

void bufk_init (struct BufK * k, size_t esize8, size_t count)
{
	void * r = malloc (esize8 * count);
	k->a = r;
	k->b = (char *)r + esize8 * (count - 1);
}


int main (void)
{
	
	struct Vertex * v = malloc (sizeof (struct Vertex) * 100);
	return v == NULL;
	
	return 0;
}





