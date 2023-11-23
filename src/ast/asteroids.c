#include <stdio.h>
#include <math.h>

#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <sokol_log.h>

#include "actor.h"
#include "object.h"
#include "pipelines.h"

#include "lines.glsl.h"

#include <HandmadeMath.h>

static struct {
	sg_pass_action pass_action;
	HMM_Mat4 projection;
} state;

static void sokol_init(void)
{
	sg_setup(&(sg_desc) {
		.context = sapp_sgcontext(),
		.logger.func = slog_func,
	});

	state.pass_action = (sg_pass_action) {
		.colors[0] = {
			.load_action = SG_LOADACTION_CLEAR,
			.clear_value = { 0.f, 0.f, 0.f, 1.f },
		}
	};

	state.projection = HMM_Orthographic_RH_NO(-1.f, 1.f,
	                                          -1.f, 1.f,
	                                          -1.f, 1.f);

	load_pipelines();
	load_bindings();
	register_new_ship();
}

static void sokol_frame(void)
{
	actors_tick();

        /* Keep aspect ratio 1:1 */
        /* TODO: Make it 4:3 */
	const int width = sapp_width();
	const int height = sapp_height();
	const int size = width<height?width:height;
	sg_begin_default_pass(&state.pass_action, size, size);

	for (size_t i=0; i<g_object_count; ++i) {
		const object_t *obj = &g_objects[i];

		sg_apply_pipeline(g_pipelines[obj->pip_type]);
		sg_apply_bindings(&g_bindings[obj->bind_type]);

		HMM_Mat4 mvp = HMM_MulM4(obj->model_mat, state.projection);
		vs_params_t vs_params;
		memcpy(vs_params.mvp, mvp.Elements, sizeof(float) * 16);
		sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
		sg_draw(0, (int) g_index_count[obj->bind_type], 1);
	}

	sg_end_pass();
	sg_commit();
}

static void sokol_event(const sapp_event *event)
{
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
		.width = 1000,
		.height = 1000,
		.init_cb = sokol_init,
		.frame_cb = sokol_frame,
		.event_cb = sokol_event,
		.cleanup_cb = sokol_cleanup,
		.window_title = "MSC 2024 Gry wektorowe - Asteroids (ezn_undefined)",
		.logger.func = slog_func,
	};
}
