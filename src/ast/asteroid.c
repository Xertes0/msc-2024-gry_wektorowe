#include "object.h"

#include "actor.h"

static void ast_tick(actor_t *act)
{
	object_t *obj = act->obj;

        /* Movement */

        /* TODO: Some kind of delta time */
	obj->move.rot += 0.05f;
	obj->move.pos = HMM_AddV3(obj->move.pos, obj->move.vel);
}

#define AST_SCALE 0.075f
void register_new_asteroid(void)
{
	object_t *ast = add_object((object_t) {
			.flags = OF_MOVING,
			.model_mat = HMM_Scale(HMM_V3(AST_SCALE, AST_SCALE, 1.f)),
			.pip_type = PIPTYPE_LINES,
			.bind_type = BINDTYPE_ASTEROIDA,
			.move.vel = HMM_V3(0.005f, 0.01f, 0.f),
		});
	add_actor((actor_t) {
			.tick = ast_tick,
			.obj = ast,
		});
}
