#include "debug.h"

#include <HandmadeMath.h>
#include <sokol_gfx.h>

#include "pipelines.h"
#include "state.h"

#include "debug.glsl.h"

#define BUFFSIZE (1024)

static struct {
	HMM_Vec2 pos;
	bool projected;
} g_circ[BUFFSIZE];
static size_t g_circ_count = 0;

static sg_bindings g_circ_bind;

static struct {
	HMM_Vec2 verts[3];
	bool projected;
} g_tri[BUFFSIZE];
static size_t g_tri_count = 0;

static sg_bindings g_tri_bind;

void init_debug(void)
{
	float circ_vertices[] = {
		-1.0f, 1.0f, 0.f,	// TL
		-1.0f, -1.0f, 0.f,	// BL
		1.0f, 1.0f, 0.f,	// TR
		1.0f, -1.0f, 0.f,	// BR
	};

	uint16_t circ_indices[] = {
		0, 2, 1,
		2, 3, 1,
	};

	g_circ_bind = (sg_bindings) {
		.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
				.type = SG_BUFFERTYPE_VERTEXBUFFER,
				.data = SG_RANGE(circ_vertices),
			}),
		.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
				.type = SG_BUFFERTYPE_INDEXBUFFER,
				.data = SG_RANGE(circ_indices),
			})
	};

	float tri_vertices[] = {
		0.f, 1.f, 2.f,
	};

	g_tri_bind = (sg_bindings) {
		.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
				.type = SG_BUFFERTYPE_VERTEXBUFFER,
				.data = SG_RANGE(tri_vertices),
			}),
	};
}

void debug_end_frame(void)
{
	g_circ_count = 0;
	g_tri_count = 0;
}

void debug_point(HMM_Vec2 pos, bool projected)
{
	g_circ[g_circ_count].pos = pos;
	g_circ[g_circ_count++].projected = projected;
}

void debug_triangle(HMM_Vec2 verts[3], bool projected)
{
	memcpy(g_tri[g_tri_count].verts, verts, sizeof(HMM_Vec2) * 3);
	g_tri[g_tri_count++].projected = projected;
}

/* TODO: Make faster. */
static void draw_circles(void)
{
	sg_apply_pipeline(g_pipelines[PIPTYPE_DEBUG_CIRCLE]);
	sg_apply_bindings(&g_circ_bind);

	for (size_t i=0; i<g_circ_count; ++i) {
		HMM_Mat4 mvp;
		if (g_circ[i].projected) {
			mvp = HMM_M4D(1.f);
		} else {
			mvp = g_state.projection;
		}

		vs_debug_circle_params_t vs_params;

		memcpy(vs_params.mvp, mvp.Elements, sizeof(float) * 16);

		vs_params.pos[0] = g_circ[i].pos.X;
		vs_params.pos[1] = g_circ[i].pos.Y;

		sg_apply_uniforms(SG_SHADERSTAGE_VS,
		                  SLOT_vs_debug_circle_params,
		                  &SG_RANGE(vs_params));
		sg_draw(0, 6, 1);
	}
}

static void draw_triangles(void)
{
	sg_apply_pipeline(g_pipelines[PIPTYPE_DEBUG_TRIANGLE]);
	sg_apply_bindings(&g_tri_bind);

	for (size_t i=0; i<g_tri_count; ++i) {
		HMM_Mat4 mvp;
		if (g_tri[i].projected) {
			mvp = HMM_M4D(1.f);
		} else {
			mvp = g_state.projection;
		}

		vs_debug_triangle_params_t vs_params;

		memcpy(vs_params.mvp, mvp.Elements, sizeof(float) * 16);

		for (size_t pos_i=0; pos_i<3; ++pos_i) {
			vs_params.pos[pos_i][0] = g_tri[i].verts[pos_i].X;
			vs_params.pos[pos_i][1] = g_tri[i].verts[pos_i].Y;
		}

		sg_apply_uniforms(SG_SHADERSTAGE_VS,
		                  SLOT_vs_debug_triangle_params,
		                  &SG_RANGE(vs_params));

		sg_draw(0, 6, 1);
	}
}

void draw_debug(void)
{
	if (g_circ_count != 0)
		draw_circles();
	if (g_tri_count != 0)
		draw_triangles();
}
