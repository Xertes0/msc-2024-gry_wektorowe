#include "ship.h"

#include "lines.glsl.h"

struct ship make_ship(void)
{
	/* A shape */
	float vertices[] = {
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
	uint16_t indices[] = {
		0, 1,
		0, 2,
		3, 4,
		5, 6,
		5, 7,
	};

	sg_buffer vertex_buffer = sg_make_buffer(&(sg_buffer_desc) {
			.type = SG_BUFFERTYPE_VERTEXBUFFER,
			.data = SG_RANGE(vertices),
			.label = "ship-vertices",
		});
	sg_buffer index_buffer = sg_make_buffer(&(sg_buffer_desc) {
			.type = SG_BUFFERTYPE_INDEXBUFFER,
			.data = SG_RANGE(indices),
			.label = "ship-indices",
		});

	sg_shader shd = sg_make_shader(lines_shader_desc(sg_query_backend()));
	sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc) {
			.shader = shd,
			.layout = {
				.attrs = {
					[ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
				},
			},
			.label = "ship-pipeline",
			.primitive_type = SG_PRIMITIVETYPE_LINES,
			.index_type = SG_INDEXTYPE_UINT16,
		});

	return (struct ship) {
		.bind = {
			.vertex_buffers[0] = vertex_buffer,
			.index_buffer = index_buffer,
		},
		.pip = pip,
	};
}
