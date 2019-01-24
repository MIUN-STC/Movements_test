#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "tcol.h"
#include "bufq.h"
#include "debug.h"
#include "global.h"
#include "vertex.h"




int main (void)
{
	struct BufQ q;
	q.capacity = 100;
	q.esize8 = sizeof (struct Vertex);
	q.flags = BUFQ_MALLOC;
	bufq_init (&q);
	
	struct Vertex * v;
	v = bufq_next (&q);
	v->pos [0] = 1.0f;
	v->pos [1] = 1.0f;
	v->pos [2] = 1.0f;
	if (v == NULL) {return -1;}
	v = bufq_next (&q);
	v->pos [0] = 1.0f;
	v->pos [1] = 1.0f;
	v->pos [2] = 1.0f;
	if (v == NULL) {return -1;}
	
	//for (struct Vertex * e = q.base; e <= (struct Vertex *)bufq_last (&q); ++ e)
	BUFQ_LOOP2 (q, struct Vertex, e)
	{
		vertex_print (stdout, e);
	}
	
	return 0;
}





