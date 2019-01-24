#include "app.h"
#include "alg.h"
#include "grid.h"
#include "glva.h"
#include "classy.h"


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
	m->data = malloc (sizeof (float) * 4 * m->count);
	float * v = (float *)m->data;
	vf32_setl (v, 4, 0.0f,  0.0f, 0.0f, 1.0f);
	v += 4;
	vf32_setl (v, 4, 0.0f,  0.5f, 0.0f, 1.0f);
	v += 4;
	vf32_setl (v, 4, 0.5f,  0.0f, 0.0f, 1.0f);
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
	m->data = malloc (sizeof (float) * 4 * m->count);
	struct Grid grid;
	vf32_setl (grid. p, 4, -1.0f, 0.0f, -1.0f, 1.0f);
	vf32_setl (grid.d1, 4, 0.1f, 0.0f, 0.0f, 0.0f);
	vf32_setl (grid.d2, 4, 0.0f, 0.0f, 0.1f, 0.0f);
	gen_grid (m->count, m->data, &grid);
}


void add_axis (struct GLVA * va)
{
	TRACE ("Adding origin axis");
	struct GLMesh * m;
	BUFV_PUSH (va->meshes, 1, m);
	if (m == NULL) {return;}
	m->flags = MESH_UPDATE_ONCE | MESH_DRAW_ALLWAYS;
	m->mode = GL_LINES;
	m->first = m [-1].first + m [-1].count;
	m->count = 2;
	m->data = malloc (sizeof (float) * 4 * m->count);
	float * v = m->data;
	vf32_setl (v, 4, 0.0f, -0.5f, 0.0f, 1.0f);
	v += 4;
	vf32_setl (v, 4, 0.0f,  0.5f, 0.0f, 1.0f);
}





int main (int argc, char *argv[])
{
	struct Application app;
	app_init (&app, argc, argv);
	

	
	
	
	struct GLVA va;
	va.vcount = 10 * 1000 * 1000;
	va.esize8 = sizeof (float) * 4;
	va.program = app.program;
	va.uniform_mvp = app.uniform_mvp;
	va.PVM = app.cam.PVM;
	vertex_layout (va.program, &va.vao, &va.vbo, va.vcount * va.esize8, NULL);
	BUFV_INIT (struct GLMesh, va.meshes, 1000);
	glva_init (&va);
	
	add_triangle (&va);
	add_grid (&va);
	add_axis (&va);
	
	glva_update (&va);
	
	

	vf32_setl (app.cam.p, 4, -6.363856, -0.165287, 4.056676, 0.0000009);
	vf32_setl (app.cam.q, 4,  0.010884, -0.984987, 0.162742, 0.056541);

	
	
	//va.meshes.p [0].flags &= ~MESH_DRAW_ALLWAYS;
	//va.meshes.p [1].flags &= ~MESH_DRAW_ALLWAYS;
	
	
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










