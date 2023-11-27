#include "object.h"

#include "actor.h"

static void ast_tick(actor_t *act)
{
	object_t *obj = act->obj;

        /* Movement */

        /* TODO: Some kind of delta time */
	obj->move.rot += 0.005f;
	obj->move.pos = HMM_AddV2(obj->move.pos, obj->move.vel);
}

#define AST_SCALE 0.1f
void register_new_asteroid(void)
{
	for (size_t i=BINDTYPE_ASTEROIDA; i <= BINDTYPE_ASTEROIDC; ++i) {
		float offset = (float) (i - BINDTYPE_ASTEROIDA);
		object_t *ast = add_object((object_t) {
				.flags = OF_MOVING,
				.model_mat = HMM_Scale(HMM_V3(AST_SCALE, AST_SCALE, 1.f)),
				.pip_type = PIPTYPE_LINES,
				.bind_type = (bind_type_t) i,
				.move.pos = HMM_V2(-0.5f + (0.5f * offset), 0.f),
				.move.vel = HMM_V2(0.0001f, 0.00005f),
			});
		add_actor((actor_t) {
				.tick = ast_tick,
				.obj = ast,
			});
	}
}
