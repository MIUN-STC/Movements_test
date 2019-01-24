#include "app.h"
#include "alg.h"
#include "grid.h"
#include "glva.h"


void add_triangle (struct GLVA * va)
{
	TRACE ("Adding a triangle");
	struct GLMesh * m;
	BUFV_PUSH (va->meshes, 1, m);
	if (m == NULL) {return;}
	m->flags = MESH_UPDATE_ONCE | MESH_DRAW_ALLWAYS;
	m->mode = GL_TRIANGLES;
	m->first = 0;
	m->count = 3;
	m->data = malloc (sizeof (struct Vertex) * m->count);
	vertex_sample1 (&m->count, m->data);
}


void add_grid (struct GLVA * va)
{
	TRACE ("Adding a grid and a line");
	struct GLMesh * m;
	BUFV_PUSH (va->meshes, 1, m);
	if (m == NULL) {return;}
	m->flags = MESH_UPDATE_ONCE | MESH_DRAW_ALLWAYS;
	m->mode = GL_LINES;
	m->first = m [-1].first + m [-1].count;
	m->count = 4*20;
	m->data = malloc (sizeof (struct Vertex) * (m->count + 2));
	struct Grid grid;
	vf32_setl (grid. p, 4, -1.0, 0.0, -1.0, 1.0);
	vf32_setl (grid.d1, 4, 0.1, 0.0, 0.0, 0.0);
	vf32_setl (grid.d2, 4, 0.0, 0.0, 0.1, 0.0);
	vf32_setl (grid. c, 4, 0.3, 0.0, 0.3, 1.0);
	gen_grid (m->count, m->data, &grid);
	struct Vertex * v = m->data;
	v += m->count;
	vf32_setl (v [0].p, 4, 0.0, -0.5, 0.0, 1.0);
	vf32_setl (v [0].c, 4, 1.0,  1.0, 1.0, 1.0);
	vf32_setl (v [1].p, 4, 0.0,  0.5, 0.0, 1.0);
	vf32_setl (v [1].c, 4, 1.0,  1.0, 1.0, 1.0);
	m->count += 2;
}





void add_poi (struct GLVA * va, struct PP * pp)
{
	TRACE ("Adding points of interest (POI)");
	struct GLMesh * m;
	BUFV_PUSH (va->meshes, 1, m);
	if (m == NULL) {return;}
	m->flags = MESH_UPDATE_ONCE | MESH_DRAW_ALLWAYS;
	m->mode = GL_POINTS;
	m->first = m [-1].first + m [-1].count;
	m->count = pp->poi.n;
	m->data = malloc (sizeof (struct Vertex) * m->count);
	vertex_array_from_poi (m->count, m->data, pp->poi.p);
	//vertex_from_poi (m->count, m->data, p, 1.0, 1.0, 1.0, 1.0);
}


void add_edge1 (struct GLVA * va, struct PP * pp, size_t from, size_t to)
{
	TRACE ("Adding edge of interest (EOI)");
	struct GLMesh * m;
	BUFV_PUSH (va->meshes, 1, m);
	if (m == NULL) {return;}
	m->flags = MESH_UPDATE_ONCE | MESH_DRAW_ALLWAYS;
	m->mode = GL_LINES;
	m->first = m [-1].first + m [-1].count;
	m->count = 400000;
	m->data = malloc (sizeof (struct Vertex) * m->count);
	size_t const n = MIN (pp->poi.n, to);
	pp_process_bcount (pp);
	pp_process_filter (pp);
	for (size_t i = from; i < n; ++ i)
	{
		pp_process_distance (pp, i);
	}
	pp_process_backtrack (pp);
	pp_process_grouper (pp);
	pp_process_dotavg (pp);
	printf ("Vertices : %li\n", pp->poi.n);
	printf ("Edges    : %li\n", pp->edge_n);
	printf ("Groups   : %li\n", pp->g);
	
	FILE * f = fopen ("data/c1_20180627_090007_class.txt", "w");
	BUFV_LOOP (pp->poi, struct POI *, e)
	{
		poi_write (f, e);
	}
	fclose (f);
	
	vertex_array_from_poi_edge (&m->count, m->data, n, pp->poi.p);
}


void add_edge2 (struct GLVA * va, struct PP * pp, size_t from, size_t to)
{
	TRACE ("Adding edge of interest (EOI)");
	struct GLMesh * m;
	BUFV_PUSH (va->meshes, 1, m);
	if (m == NULL) {return;}
	m->flags = MESH_UPDATE_ONCE | MESH_DRAW_ALLWAYS;
	m->mode = GL_LINES;
	m->first = m [-1].first + m [-1].count;
	m->count = 400000;
	m->data = malloc (sizeof (struct Vertex) * m->count);
	size_t const n = MIN (pp->poi.n, to);
	for (size_t i = from; i < n; ++ i)
	{
		pp_process_distance (pp, i);
	}
	printf ("Vertices : %li\n", pp->poi.n);
	printf ("Edges    : %li\n", pp->edge_n);
	printf ("Groups   : %li\n", pp->g);
	vertex_array_from_poi_edge (&m->count, m->data, n, pp->poi.p);
}




int main (int argc, char *argv[])
{
	struct Application app;
	app_init (&app, argc, argv);
	

	
	
	
	struct GLVA va;
	va.vcount = 10 * 1000 * 1000;
	va.esize8 = sizeof (struct Vertex);
	va.program = app.program;
	va.uniform_mvp = app.uniform_mvp;
	va.PVM = app.cam.PVM;
	vertex_layout (va.program, &va.vao, &va.vbo, va.vcount * va.esize8, NULL);
	BUFV_INIT (struct GLMesh, va.meshes, 1000);
	glva_init (&va);
	
	struct PP pp;
	//load_pp (&pp, "c2_20180627_040007.txt");
	//load_pp (&pp, "c1_20180626_190007.txt");
	//load_pp (&pp, "test1.txt");
	//load_pp (&pp, "data/pointsL.txt");
	//load_pp (&pp, "data/c2_20180626_190007.txt");
	load_pp (&pp, "data/c1_20180627_090007.txt");
	//load_pp (&pp, "data/calib_c2.txt");
	
	add_triangle (&va);
	add_grid (&va);
	add_poi (&va, &pp);
	add_edge1 (&va, &pp, 0, pp.poi.n);
	//add_edge2 (&va, &pp, 0, pp.poi.n);
	glva_update (&va);
	
	

	vf32_setl (app.cam.p, 4, -6.363856, -0.165287, 4.056676, 0.0000009);
	vf32_setl (app.cam.q, 4,  0.010884, -0.984987, 0.162742, 0.056541);

	
	
	//va.meshes.p [0].flags &= ~MESH_DRAW_ALLWAYS;
	//va.meshes.p [1].flags &= ~MESH_DRAW_ALLWAYS;
	va.meshes.p [2].flags &= ~MESH_DRAW_ALLWAYS;
	va.meshes.p [3].flags &= ~MESH_DRAW_ALLWAYS;
	va.meshes.p [4].flags &= ~MESH_DRAW_ALLWAYS;
	va.meshes.p [5].flags &= ~MESH_DRAW_ALLWAYS;
	
	
	TRACE ("main loop");
	
	//size_t n = 0;
	//int edger = 0;
	
	while (1)
	{
		app_frame_begin (&app);
		if (app.flags & APP_QUIT) {break;}
		if (app.flags & APP_OPENGL_ERROR) {break;}
		while (SDL_PollEvent (&app.event))
		{
			app_handle_input (&app);
			switch (app.event.type)
			{
				case SDL_KEYDOWN:
				switch (app.event.key.keysym.sym)
				{
					case SDLK_n:
					//edger = !edger;
					break;
					
					
					case SDLK_c:
					vf32_print (stdout, app.cam.p, 4, "%f ");
					vf32_print (stdout, app.cam.q, 4, "%f ");
					break;
					
					case SDLK_1:
					va.meshes.p [2].flags ^= MESH_DRAW_ALLWAYS;
					break;
					
					case SDLK_2:
					va.meshes.p [2].flags ^= MESH_DRAW_ALLWAYS;
					va.meshes.p [3].flags ^= MESH_DRAW_ALLWAYS;
					break;
					
					case SDLK_3:
					va.meshes.p [3].flags ^= MESH_DRAW_ALLWAYS;
					va.meshes.p [4].flags ^= MESH_DRAW_ALLWAYS;
					break;
					
					case SDLK_4:
					va.meshes.p [4].flags ^= MESH_DRAW_ALLWAYS;
					va.meshes.p [5].flags ^= MESH_DRAW_ALLWAYS;
					break;
				}
				break;
			}
		}
		

		app_update_camera (&app);
		glUseProgram (app.program);
		glUniformMatrix4fv (app.uniform_mvp, 1, GL_FALSE, app.cam.PVM);
		glva_draw (&va);
		app_frame_end (&app);
	}

	app_destroy (&app);

	return 0;
}










