#include <stdio.h>
#include <math.h>

#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <sokol_log.h>

#include "actor.h"
#include "object.h"
#include "pipelines.h"
#include "state.h"

#include "lines.glsl.h"

#include "offscreen0.glsl.h"
#include "offscreen1.glsl.h"
#include "offscreen2.glsl.h"
#include "offscreen3.glsl.h"
/* Now in state.h */
/* #define OFFSCREEN_SHADER_COUNT 4 */

#include <HandmadeMath.h>

#define TARGET_RATIO (4.f / 3.f)
#define OFFSCREEN_HEIGHT (256.f)
#define OFFSCREEN_WIDTH (OFFSCREEN_HEIGHT * TARGET_RATIO)

static void sokol_init(void)
{
	sg_setup(&(sg_desc) {
			.context = sapp_sgcontext(),
			.logger.func = slog_func,
		});

	g_state.pass_action = (sg_pass_action) {
		.colors[0] = {
			.load_action = SG_LOADACTION_CLEAR,
			.clear_value = { 0.f, 0.f, 0.f, 1.f },
		},
		.depth = {
			.load_action = SG_LOADACTION_DONTCARE,
			.store_action = SG_STOREACTION_DONTCARE,
		},
	};

	g_state.projection = HMM_Orthographic_RH_NO(-TARGET_RATIO, TARGET_RATIO,
	                                          -1.f, 1.f,
	                                          -1.f, 1.f);

	const sg_image offscr_img = sg_make_image(&(sg_image_desc) {
			.render_target = true,
			.width = (int) OFFSCREEN_WIDTH,
			.height = (int) OFFSCREEN_HEIGHT,
			.pixel_format = PIP_PIXEL_FORMAT,
			.sample_count = PIP_SAMPLE_COUNT,
			.label = "offscreen-image",
		});
	g_state.offscr.pass = sg_make_pass(&(sg_pass_desc) {
			.color_attachments[0].image = offscr_img,
			.label = "offscreen-pass",
		});

	const sg_sampler smp = sg_make_sampler(&(sg_sampler_desc) {
			.min_filter = SG_FILTER_NEAREST,
			.mag_filter = SG_FILTER_NEAREST,
			.wrap_u = SG_WRAP_REPEAT,
			.wrap_v = SG_WRAP_REPEAT,
		});

	float offscr_vertices[] = {
		-1.0f,  1.0f, 0.0f,	0.0f, 1.0f,
		1.0f,  1.0f, 0.0f,	1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,	1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,	0.0f, 0.0f,
	};
	uint16_t offscr_indices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	g_state.offscr.bind = (sg_bindings) {
		.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
				.type = SG_BUFFERTYPE_VERTEXBUFFER,
				.data = SG_RANGE(offscr_vertices),
				.label = "offscr-vertices"
			}),
		.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
				.type = SG_BUFFERTYPE_INDEXBUFFER,
				.data = SG_RANGE(offscr_indices),
				.label = "offscr-indices"
			}),
		.fs = {
			.images[SLOT_tex] = offscr_img,
			.samplers[SLOT_smp] = smp,
		},
	};

	g_state.offscr.selected = 1;
	const sg_shader offscr_shaders[OFFSCREEN_SHADER_COUNT] = {
		sg_make_shader(offscreen0_shader_desc(sg_query_backend())),
		sg_make_shader(offscreen1_shader_desc(sg_query_backend())),
		sg_make_shader(offscreen2_shader_desc(sg_query_backend())),
		sg_make_shader(offscreen3_shader_desc(sg_query_backend())),
	};
	for (size_t i=0; i<OFFSCREEN_SHADER_COUNT; ++i) {
		g_state.offscr.pip[i] = sg_make_pipeline(&(sg_pipeline_desc) {
				.layout = {
                                        /* Attrs always the same. */
					.attrs = {
						[ATTR_vs_offscr0_position].format = SG_VERTEXFORMAT_FLOAT3,
						[ATTR_vs_offscr0_tex_coord].format = SG_VERTEXFORMAT_FLOAT2,
					},
				},
				.shader = offscr_shaders[i],
				.index_type = SG_INDEXTYPE_UINT16,
				.label = "offscreen-to-display-nth-pipeline",
			});
	}

	load_pipelines();
	load_bindings();
	register_new_ship();
	register_new_asteroid();
}

static void sokol_frame(void)
{
	actors_tick();

	sg_begin_pass(g_state.offscr.pass, &g_state.pass_action);

	pip_type_t last_pip = PIPTYPE_COUNT;
	for (size_t i=0; i<g_object_count; ++i) {
		const object_t *obj = &g_objects[i];

		if (last_pip != obj->pip_type) {
			sg_apply_pipeline(g_pipelines[obj->pip_type]);
			last_pip = obj->pip_type;
		}
		sg_apply_bindings(&g_bindings[obj->bind_type]);

		HMM_Mat4 mvp = HMM_MulM4(g_state.projection, object_mat(obj));
		vs_params_t vs_params;
		memcpy(vs_params.mvp, mvp.Elements, sizeof(float) * 16);
		sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
		sg_draw(0, (int) g_index_count[obj->bind_type], 1);
	}
	sg_end_pass();

	const float win_width = sapp_widthf();
	const float win_height = sapp_heightf();
	sg_begin_default_pass(&g_state.pass_action, (int) win_width, (int) win_height);

	const float width = win_height * TARGET_RATIO;
	const float height = win_height;

	sg_apply_viewportf((win_width - width) / 2.f, 0, width, height, false);

	sg_apply_pipeline(g_state.offscr.pip[g_state.offscr.selected]);
	sg_apply_bindings(&g_state.offscr.bind);

	switch (g_state.offscr.selected) {
	case 0: break;
	case 1: // Shader 1 and 2 take the same parameters.
	case 2: {
		fs_offscr1_params_t fs_offscr_params = {
			.u_resolution = {
				[0] = width,
				[1] = height,
			},
			.u_time = g_state.time,
		};
		sg_apply_uniforms(SG_SHADERSTAGE_FS, SLOT_fs_offscr1_params, &SG_RANGE(fs_offscr_params));
	} break;
	case 3: {
		vs_offscr3_params_t vs_offscr_params = {
			.SourceSize = {
				[0] = OFFSCREEN_WIDTH,
				[1] = OFFSCREEN_HEIGHT,
			},
			.OutputSize = {
				[0] = width,
				[1] = height,
			},
		};
		sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_offscr3_params, &SG_RANGE(vs_offscr_params));
	} break;
	default:
		fprintf(stderr, "ERROR: Bad shader selected\n");
		sapp_request_quit();
	}

	sg_draw(0, 6, 1);

	sg_end_pass();

	sg_commit();

	g_state.time += (float) sapp_frame_duration();
}

static void sokol_event(const sapp_event *event)
{
	if (event->type == SAPP_EVENTTYPE_KEY_DOWN &&
	    event->modifiers & SAPP_MODIFIER_CTRL) {
		switch (event->key_code) {
		case SAPP_KEYCODE_0: {
			g_state.offscr.selected = 0;
		} break;
		case SAPP_KEYCODE_1: {
			g_state.offscr.selected = 1;
		} break;
		case SAPP_KEYCODE_2: {
			g_state.offscr.selected = 2;
		} break;
		case SAPP_KEYCODE_3: {
			g_state.offscr.selected = 3;
		} break;
		default: break;
		}
	}

	actors_event(event);
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
		.width = (int) (720.f * TARGET_RATIO),
		.height = 720,
		.init_cb = sokol_init,
		.frame_cb = sokol_frame,
		.event_cb = sokol_event,
		.cleanup_cb = sokol_cleanup,
		.window_title = "MSC 2024 Gry wektorowe - Asteroids (ezn_undefined)",
		.logger.func = slog_func,
	};
}
