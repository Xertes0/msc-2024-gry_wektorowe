#include <stdio.h>

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#define SOKOL_NO_DEPRECATED

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"

#include "triangle.glsl.h"

static struct {
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_action;
} state;

static void sokol_init(void)
{
	sg_setup(&(sg_desc) {
		.context = sapp_sgcontext(),
		.logger.func = slog_func,
	});

	float vertices[] = {
		// positions            // colors
		0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
	};
	state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
			.data = SG_RANGE(vertices),
			.label = "triangle-vertices",
		});

	sg_shader shd = sg_make_shader(triangle_shader_desc(sg_query_backend()));
	state.pip = sg_make_pipeline(&(sg_pipeline_desc) {
			.shader = shd,
			.layout = {
				.attrs = {
					[ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
					[ATTR_vs_color0].format = SG_VERTEXFORMAT_FLOAT4,
				},
			},
			.label = "triangle-pipeline",
		});

	state.pass_action = (sg_pass_action) {
		.colors[0] = {
			.load_action = SG_LOADACTION_CLEAR,
			.clear_value = { 0.f, 0.f, 0.f, 1.f },
		}
	};
}

static void sokol_frame(void)
{
	sg_begin_default_pass(&state.pass_action, sapp_width(), sapp_height());
	sg_apply_pipeline(state.pip);
	sg_apply_bindings(&state.bind);
	sg_draw(0, 3, 1);
	sg_end_pass();
	sg_commit();
}

static void sokol_cleanup(void)
{
	sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[])
{
	(void) argc;
	(void) argv;

	return (sapp_desc) {
		.width = 800,
		.height = 600,
		.init_cb = sokol_init,
		.frame_cb = sokol_frame,
		.cleanup_cb = sokol_cleanup,
		/* .event_cb = sokol_event, */
		.window_title = "MSC 2024 Gry wektorowe (ezn_undefined)",
		.logger.func = slog_func,
	};
}
