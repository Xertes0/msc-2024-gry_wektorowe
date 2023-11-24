#include "pipelines.h"

#include <assert.h>

#include "lines.glsl.h"

sg_pipeline g_pipelines[PIPTYPE_COUNT] = { 0 };
sg_bindings g_bindings[BINDTYPE_COUNT] = { 0 };
size_t g_index_count[BINDTYPE_COUNT] = { 0 };

void load_pipelines(void)
{
	static bool called = false;
	assert(!called); called = true;

	g_pipelines[PIPTYPE_LINES] = sg_make_pipeline(&(sg_pipeline_desc) {
			.shader = sg_make_shader(lines_shader_desc(sg_query_backend())),
			.layout = {
				.attrs = {
					[ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
				},
			},
			.label = "lines-pipeline",
			.primitive_type = SG_PRIMITIVETYPE_LINES,
			.index_type = SG_INDEXTYPE_UINT16,
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

	float asteroida_vertices[] = {
		-1.0f, 1.0f, 0.f,
		-1.0f, -1.0f, 0.f,
		1.0f, 1.0f, 0.f,
		1.0f, -1.0f, 0.f,
	};
	uint16_t asteroida_indices[] = {
		0, 1,
		0, 2,
		3, 1,
		3, 2,
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
}
