#include "pipelines.h"

#include <assert.h>

#include "lines.glsl.h"
#include "debug.glsl.h"

sg_pipeline g_pipelines[PIPTYPE_COUNT] = { 0 };
sg_bindings g_bindings[BINDTYPE_COUNT] = { 0 };
size_t g_index_count[BINDTYPE_COUNT] = { 0 };

void load_pipelines(void)
{
	static bool called = false;
	assert(!called); called = true;

	g_pipelines[PIPTYPE_LINES] = sg_make_pipeline(&(sg_pipeline_desc) {
		.layout = {
			.attrs = {
				[ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
			},
		},
		.shader = sg_make_shader(lines_shader_desc(sg_query_backend())),
		.index_type = SG_INDEXTYPE_UINT16,
		.cull_mode = PIP_CULLMODE,
		.sample_count = PIP_SAMPLE_COUNT,
		.depth.pixel_format = SG_PIXELFORMAT_NONE,
		.colors[0].pixel_format = PIP_PIXEL_FORMAT,
		.primitive_type = SG_PRIMITIVETYPE_LINES,
		.label = "lines-pipeline",
	});

	g_pipelines[PIPTYPE_DEBUG_CIRCLE] = sg_make_pipeline(&(sg_pipeline_desc) {
		.layout = {
			.attrs = {
				[ATTR_vs_debug_circle_position].format = SG_VERTEXFORMAT_FLOAT3,
			},
		},
		.shader = sg_make_shader(debug_circle_shader_desc(sg_query_backend())),
		.colors[0].blend.enabled = true,
		.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
		.index_type = SG_INDEXTYPE_UINT16,
		.primitive_type = SG_PRIMITIVETYPE_TRIANGLES,
		.label = "debug-circle-pipeline",
	});
	g_pipelines[PIPTYPE_DEBUG_LINES] = sg_make_pipeline(&(sg_pipeline_desc) {
		.layout = {
			.attrs = {
				[ATTR_vs_debug_lines_index].format = SG_VERTEXFORMAT_FLOAT,
			},
		},
		.shader = sg_make_shader(debug_lines_shader_desc(sg_query_backend())),
		.colors[0].blend.enabled = true,
		.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
		.primitive_type = SG_PRIMITIVETYPE_LINES,
		.label = "debug-lines-pipeline",
	});
	g_pipelines[PIPTYPE_DEBUG_TRIANGLE] = sg_make_pipeline(&(sg_pipeline_desc) {
		.layout = {
			.attrs = {
				[ATTR_vs_debug_triangle_index].format = SG_VERTEXFORMAT_FLOAT,
			},
		},
		.shader = sg_make_shader(debug_triangle_shader_desc(sg_query_backend())),
		.colors[0].blend.enabled = true,
		.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
		.primitive_type = SG_PRIMITIVETYPE_TRIANGLES,
		.label = "debug-triangle-pipeline",
	});

}

void load_bindings(void)
{
	static bool called = false;
	assert(!called); called = true;

	/* A shape */
	float ship_vertices[] = {
		/* /\ */
		0.0f,  1.0f, 0.f,
		0.5f, -0.8f, 0.f,
		-0.5f, -0.8f, 0.f,

		/* - */
		0.425f, -0.65f, 0.f,
		-0.425f, -0.65f, 0.f,

		/* Thrust */
		0.0f, -1.0f, 0.f,
		0.2f, -0.65f, 0.f,
		-0.2f, -0.65f, 0.f,
	};
	uint16_t ship_indices[] = {
		0, 1,
		0, 2,
		3, 4,
		5, 6,
		5, 7,
	};

	size_t ship_index_count = sizeof(ship_indices) / sizeof(uint16_t);
	g_index_count[BINDTYPE_SHIPA] = ship_index_count - 4;
	g_index_count[BINDTYPE_SHIPB] = ship_index_count;
	g_bindings[BINDTYPE_SHIPA] = (sg_bindings) {
		.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
			.type = SG_BUFFERTYPE_VERTEXBUFFER,
			.data = SG_RANGE(ship_vertices),
			.label = "ship-vertices",
		}),
		.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
			.type = SG_BUFFERTYPE_INDEXBUFFER,
			.data = SG_RANGE(ship_indices),
			.label = "ship-indices",
		}),
	};
	g_bindings[BINDTYPE_SHIPB] = g_bindings[BINDTYPE_SHIPA];

	/* TODO: Bad shape. */
	float asteroida_vertices[] = {
		-0.5f, 1.f, 0.f,
		-0.25f, 0.5f, 0.f,
		-1.f, 0.5f, 0.f,
		-1.f, -0.25f, 0.f,
		-0.5f, -1.f, 0.f,
		0.25f, -0.75f, 0.f,
		0.5f, -1.f, 0.f,
		1.f, -0.5f, 0.f,
		0.25f, 0.f, 0.f,
		1.f, 0.25f, 0.f,
		1.f, 0.5f, 0.f,
		0.25f, 1.f, 0.f,
	};
	uint16_t asteroida_indices[] = {
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5,
		5, 6, 6, 7, 7, 8, 8, 9, 9, 10,
		10, 11, 11, 0,
	};

	g_index_count[BINDTYPE_ASTEROIDA] = sizeof(asteroida_indices) / sizeof(uint16_t);
	g_bindings[BINDTYPE_ASTEROIDA] = (sg_bindings) {
		.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
			.type = SG_BUFFERTYPE_VERTEXBUFFER,
			.data = SG_RANGE(asteroida_vertices),
			.label = "asteroida-vertices",
		}),
		.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
			.type = SG_BUFFERTYPE_INDEXBUFFER,
			.data = SG_RANGE(asteroida_indices),
			.label = "asteroida-indices",
		}),
	};

	float asteroidb_vertices[] = {
		0.f, 0.5f, 0.f,
		0.5f, 1.f, 0.f,
		1.f, 0.5f, 0.f,
		0.75f, 0.f, 0.f,
		1.f, -0.5f, 0.f,
		0.25f, -1.f, 0.f,
		-0.5f, -1.f, 0.f,
		-1.f, -0.5f, 0.f,
		-1.f, 0.5f, 0.f,
		-0.5f, 1.f, 0.f,
	};
	uint16_t asteroidb_indices[] = {
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5,
		5, 6, 6, 7, 7, 8, 8, 9, 9, 0,
	};

	g_index_count[BINDTYPE_ASTEROIDB] = sizeof(asteroidb_indices) / sizeof(uint16_t);
	g_bindings[BINDTYPE_ASTEROIDB] = (sg_bindings) {
		.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
			.type = SG_BUFFERTYPE_VERTEXBUFFER,
			.data = SG_RANGE(asteroidb_vertices),
			.label = "asteroidb-vertices",
		}),
		.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
			.type = SG_BUFFERTYPE_INDEXBUFFER,
			.data = SG_RANGE(asteroidb_indices),
			.label = "asteroidb-indices",
		}),
	};

	float asteroidc_vertices[] = {
		0.f, 0.75f, 0.f,
		0.5f, 1.f, 0.f,
		1.f, 0.5f, 0.f,
		0.5f, 0.25f, 0.f,
		1.f, -0.25f, 0.f,
		0.5f, -1.f, 0.f,
		-0.25f, -0.75f, 0.f,
		-0.5f, -1.f, 0.f,
		-1.f, -0.5f, 0.f,
		-0.75f, 0.f, 0.f,
		-1.f, 0.5f, 0.f,
		-0.5, 1.f, 0.f,
	};
	uint16_t asteroidc_indices[] = {
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5,
		5, 6, 6, 7, 7, 8, 8, 9, 9, 10,
		10, 11, 11, 0,
	};

	g_index_count[BINDTYPE_ASTEROIDC] = sizeof(asteroidc_indices) / sizeof(uint16_t);
	g_bindings[BINDTYPE_ASTEROIDC] = (sg_bindings) {
		.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
			.type = SG_BUFFERTYPE_VERTEXBUFFER,
			.data = SG_RANGE(asteroidc_vertices),
			.label = "asteroidc-vertices",
		}),
		.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
			.type = SG_BUFFERTYPE_INDEXBUFFER,
			.data = SG_RANGE(asteroidc_indices),
			.label = "asteroidc-indices",
		}),
	};

	float bullet_vertices[] = {
		-1.0f, 1.0f, 0.f,
		-1.0f, -1.0f, 0.f,
		1.0f, 1.0f, 0.f,
		1.0f, -1.0f, 0.f,
	};
	uint16_t bullet_indices[] = {
		0, 1,
		0, 2,
		3, 1,
		3, 2,
	};
	g_index_count[BINDTYPE_BULLET] = sizeof(bullet_indices) / sizeof(uint16_t);
	g_bindings[BINDTYPE_BULLET] = (sg_bindings) {
		.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
			.type = SG_BUFFERTYPE_VERTEXBUFFER,
			.data = SG_RANGE(bullet_vertices),
			.label = "bullet-vertices",
		}),
		.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
			.type = SG_BUFFERTYPE_INDEXBUFFER,
			.data = SG_RANGE(bullet_indices),
			.label = "bullet-indices",
		}),
	};
}
