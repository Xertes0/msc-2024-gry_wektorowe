#include <stdio.h>

#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <sokol_log.h>

#include "ship.h"
#include "lines.glsl.h"

static struct {
	sg_pass_action pass_action;
	struct ship ship;
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

	state.ship = make_ship();
}

static void sokol_frame(void)
{
	const int width = sapp_widthf();
	const int height = sapp_heightf();

	vs_params_t vs_params = {
		.u_res = { width, height },
	};

	sg_begin_default_pass(&state.pass_action, (int) width, (int) height);
	sg_apply_pipeline(state.ship.pip);
	sg_apply_bindings(&state.ship.bind);
	sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
	static unsigned flip_i = 0; flip_i++;
	sg_draw(0, (flip_i>>2)%2==0?10:6, 1);
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
		.window_title = "MSC 2024 Gry wektorowe - Asteroids (ezn_undefined)",
		.logger.func = slog_func,
	};
}
