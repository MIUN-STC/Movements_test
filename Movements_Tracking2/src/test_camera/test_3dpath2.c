#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glcorearb.h>
#include <SDL2/SDL.h>

#include <stdio.h>
#include <unistd.h>

#include "simplewin.h"
#include "debug.h"
#include "debug_gl.h"
#include "mat.h"
#include "camera.h"
#include "shader.h"
#include "global.h"
#include "mat.h"
#include "map.h"
#include "gbuf.h"
#include "gmesh.h"
#include "gen.h"
#include "poi.h"

 
#define APP_MESH_GRID 0
#define APP_MESH_C1_POINTS 1
#define APP_MESH_C2_POINTS 2


void convert_poi_to_vertex (struct POI * p, struct Vertex * v0, float r, float g, float b, float a)
{
	GBUF_LOOP (size_t, i, p)
	{
		//TRACE_F ("%i", i);
		struct Vertex * v = gbuf_add (v0);
		if (v == NULL) {break;}
		v->pos [0] = p [i].p [POI_T];
		v->pos [1] = p [i].p [POI_X];
		v->pos [2] = p [i].p [POI_Y];
		v->pos [3] = 1.0f;
		V4_SET (v->col, r, g, b, a);
	}
	
}


void app_read_poi (struct POI * poi, char const * filename)
{
	FILE * f = fopen (filename, "r");
	ASSERT (f != NULL);
	while (1)
	{
		struct POI * p = gbuf_add (poi);
		if (p == NULL) {break;}
		int r = poi_read (f, p);
		if (r < 0) {break;}
	}
	fclose (f);
}


void app_create_mesh (struct SWin * win, struct GMesh * mesh, struct POI * poi1, struct POI * poi2)
{
	{
		struct GMesh * m = gbuf_add (mesh);
		ASSERT (m != NULL);
		struct Vertex * grid = gbuf_init (NULL, 20 * 20 * 2, sizeof (struct Vertex), GBUF_MALLOC);
		gen1_grid (grid, -1.0f, -10.0f, 10.0f, -10.0f, 10.0f, 0.1f);
		m->flags = GMESH_DRAW;
		m->mode = GL_LINES;
		m->data = grid;
		m->cam = &(win->cam);
		M4_IDENTITY (m->mm);
		//m->mm [M4_V0 + 0] =  4.0f;
		//m->mm [M4_V1 + 1] =  4.0f;
	}
	
	{
		struct GMesh * m = gbuf_add (mesh);
		ASSERT (m != NULL);
		size_t n = gbuf_count (poi1);
		struct Vertex * v = gbuf_init (NULL, n, sizeof (struct Vertex), GBUF_MALLOC);
		convert_poi_to_vertex (poi1, v, 1.0f, 0.0f, 0.0f, 1.0f);
		m->flags = GMESH_DRAW;
		m->mode = GL_POINTS;
		m->data = v;
		m->cam = &(win->cam);
		//M4_IDENTITY (m->mm);
		M4_CLR (m->mm);
		m->mm [M4_V0 + 0] =  1.0f / 100.0f;
		m->mm [M4_V1 + 2] =  1.0f / 3000.0f;
		m->mm [M4_V2 + 1] = -1.0f / 3000.0f;
		m->mm [15] = 1.0f;
	}
	
	
	{
		struct GMesh * m = gbuf_add (mesh);
		ASSERT (m != NULL);
		size_t n = gbuf_count (poi2);
		struct Vertex * v = gbuf_init (NULL, n, sizeof (struct Vertex), GBUF_MALLOC);
		convert_poi_to_vertex (poi2, v,0.0f, 1.0f, 0.0f, 1.0f);
		m->flags = GMESH_DRAW;
		m->mode = GL_POINTS;
		m->data = v;
		m->cam = &(win->cam);
		//M4_IDENTITY (m->mm);
		M4_CLR (m->mm);
		m->mm [M4_V0 + 0] =  1.0f / 100.0f;
		m->mm [M4_V1 + 2] =  1.0f / 3000.0f;
		m->mm [M4_V2 + 1] = -1.0f / 3000.0f;
		m->mm [15] = 1.0f;
	}
}

int main (int argc, char * argv [])
{
	ASSERT_F (argc == 1, "No arg allowed");
	ASSERT_F (argv != NULL, "NULL error");

	struct SWin win;
	swin_init (&win);
	
	struct Shader shaders [2] = 
	{
		{0, GL_VERTEX_SHADER, "shader.glvs"},
		{0, GL_FRAGMENT_SHADER, "shader.glfs"}
	};
	
	GLuint program = program_create (shaders, COUNTE (shaders));
	GLuint uniform_mvp = glGetUniformLocation (program, "mvp");
	ASSERT_F (uniform_mvp >= 0, "glGetUniformLocation no uniform found.");
	
	struct POI * poi1 = gbuf_init (NULL, 1000 * 1000, sizeof (struct POI), GBUF_MALLOC);
	struct POI * poi2 = gbuf_init (NULL, 1000 * 1000, sizeof (struct POI), GBUF_MALLOC);
	app_read_poi (poi1, "c1_20180626_190007.txt");
	app_read_poi (poi2, "c2_20180626_190007.txt");
	
	struct GMesh * mesh = gbuf_init (NULL, 3, sizeof (struct GMesh), GBUF_MALLOC);
	app_create_mesh (&win, mesh, poi1, poi2);
	gmesh_init (mesh, program);
	
	while (win.flags & SWIN_SHOULD_RUN)
	{
		while (SDL_PollEvent (&(win.event)))
		{
			swin_event (&win);
		}
		glUseProgram (program);
		gmesh_draw (mesh, uniform_mvp);
		swin_update (&win);
	}
	
	swin_quit (&win);

	return 0;
}
