#include "app.h"
#include "app1.h"
#include "grid.h"
 
#define FMT_INT TCOL (TCOL_NORMAL, TCOL_YELLOW, TCOL_DEFAULT) "%02i " TCOL_RESET
#define FMT_FLT TCOL (TCOL_NORMAL, TCOL_CYAN, TCOL_DEFAULT) "%04.1f " TCOL_RESET
#define APP_WINDOW_WIDTH 1024
#define APP_WINDOW_HEIGHT 768
#define APP_TITLE "My OpenGL test window"



void add_triangle (struct VA * va)
{
	TRACE ("Adding a triangle");
	struct Mesh * m = va->meshes + 0;
	m->flags = MESH_UPDATE_ONCE | MESH_DRAW_ALLWAYS;
	m->mode = GL_TRIANGLES;
	m->first = 0;
	m->count = 3;
	m->data = malloc (sizeof (struct Vertex) * m->count);
	vertex_sample1 (&m->count, m->data);
}


void add_grid (struct VA * va)
{
	TRACE ("Adding a grid and a line");
	struct Mesh * m = va->meshes + 1;
	m->flags = MESH_UPDATE_ONCE | MESH_DRAW_ALLWAYS;
	m->mode = GL_LINES;
	m->first = m [-1].first + m [-1].count;
	m->count = 4*20;
	m->data = malloc (sizeof (struct Vertex) * (m->count + 2));
	struct Grid grid;
	grid.p_xyzw = v4f32_XYZW (-1.0f, 0.0f, -1.0f, 1.0f);
	grid.d1_xyzw = v4f32_XYZW (0.1f, 0.0f, 0.0f, 0.0f);
	grid.d2_xyzw = v4f32_XYZW (0.0f, 0.0f, 0.1f, 0.0f);
	grid.c_rgba = v4f32_RGBA (0.3f, 0.0f, 0.3f, 1.0f);
	gen_grid (m->count, m->data, &grid);
	struct Vertex * v = m->data;
	v += m->count;
	v [0].p_xyzw = v4f32_XYZW (0.0f, -0.5f, 0.0f, 1.0f);
	v [0].c_rgba = v4f32_RGBA (1.0f, 1.0f, 1.0f, 1.0f);
	v [1].p_xyzw = v4f32_XYZW (0.0f, 0.5f, 0.0f, 1.0f);
	v [1].c_rgba = v4f32_RGBA (1.0f, 1.0f, 1.0f, 1.0f);
	m->count += 2;
}


void add_poi (struct VA * va)
{
	TRACE ("Adding points of interest (poi)");
	struct Mesh * m = va->meshes + 2;
	m->flags = MESH_UPDATE_ONCE | MESH_DRAW_ALLWAYS;
	m->mode = GL_POINTS;
	m->first = m [-1].first + m [-1].count;
	int count4 = 1000;
	m->count = count4 * 4;
	struct v3f32_SIMD4_TXY * p = malloc (sizeof (struct v3f32_SIMD4_TXY) * count4);
	poi_readfile4 (&count4, p, "c1_20180626_190007.txt");
	m->data = malloc (sizeof (struct Vertex4) * count4);
	vertex_from_poi (count4, m->data, p, v4f32_RGBA (1.0f, 1.0f, 0.0f, 1.0f));
	m->count = count4 * 4;
	//vertex_from_poi (m->count, m->data, p, 1.0, 1.0, 1.0, 1.0);
}



int main (int argc, char *argv[])
{
	struct Application app;
	app_init (&app, argc, argv);
	
	struct Mesh meshes [10] = {0};
	struct VA va;
	va.count = COUNTOF (meshes);
	va.meshes = meshes;
	va_init (&va, app.program, 1000000, NULL);
	
	
	add_triangle (&va);
	add_grid (&va);
	add_poi (&va);
	
	
	va_update (&va);
	
	

	
	
	TRACE ("main loop");
	while (1)
	{
		app_frame_begin (&app);
		if (app.flags & APP_QUIT) {break;}
		if (app.flags & APP_OPENGL_ERROR) {break;}
		while (SDL_PollEvent (&app.event))
		{
			app_handle_input (&app);
		}
		app_update_camera (&app);
		va_draw (&va);
		app_frame_end (&app);
	}

	app_destroy (&app);

	return 0;
}










