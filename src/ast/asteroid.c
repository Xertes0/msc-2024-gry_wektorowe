#include "object.h"

#include <assert.h>

static void ast_tick(object_t *obj)
{
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
				.tick = ast_tick,
				.flags = OF_MOVING | OF_BULLET_TARGET,
				.model_mat = HMM_Scale(HMM_V3(AST_SCALE, AST_SCALE, 1.f)),
				.pip_type = PIPTYPE_LINES,
				.bind_type = (bind_type_t) i,
				.move.pos = HMM_V2(-0.5f + (0.5f * offset), 0.f),
				.move.vel = HMM_V2(0.0001f, 0.00005f),
			});
		switch (i) {
		case BINDTYPE_ASTEROIDA:
			ast->collision = asteroida_collision_data;
			break;
		case BINDTYPE_ASTEROIDB:
			ast->collision = asteroidb_collision_data;
			break;
		case BINDTYPE_ASTEROIDC:
			ast->collision = asteroidc_collision_data;
			break;
		default: assert(false);
		}
	}
}

void asteroid_hit(object_t *ast, object_t *bullet)
{
	(void) bullet;
	*ast = g_objects[--g_object_count];
}

static HMM_Vec2 asteroida_collision_data_data[] = {
	{{ 0.25f, -0.75f }},
	{{ -0.5f, 1.f }},
	{{ -0.25f, 0.5f }},
	{{ 0.5f, -1.f }},
	{{ 0.5f, 0.5f }},
	{{ -1.f, -0.25f }},
	{{ -0.5f, -1.f }},
	{{ 0.25f, 0.25f }},
	{{ -0.5f, 1.f }},
	{{ 1.f, 0.5f }},
	{{ 0.5f, 0.5f }},
	{{ -0.25f, -0.5f }},
	{{ -0.5f, 0.25f }},
	{{ 1.f, 1.f }},
	{{ 0.5f, -0.5f }},
};
#define ASTEROIDA_COLLISION_DATA_COUNT 15
static_assert(ASTEROIDA_COLLISION_DATA_COUNT % 3 == 0, "");

static HMM_Vec2 asteroidb_collision_data_data[] = {
	{{ 0.f, -0.5f }},
	{{ 1.f, 1.f }},
	{{ 0.f, 0.75f }},
	{{ 0.75, 0.5f }},
	{{ 1.f, 1.f }},
	{{ 0.5, 0.5f }},
	{{ 0.f, 1.f }},
	{{ 0.75, 0.75f }},
	{{ 1.f, 1.f }},
	{{ 1.f, 0.5f }},
	{{ 0.f, 0.f }},
	{{ 0.75f, 1.f }},
};
#define ASTEROIDB_COLLISION_DATA_COUNT 12
static_assert(ASTEROIDB_COLLISION_DATA_COUNT % 3 == 0, "");

static HMM_Vec2 asteroidc_collision_data_data[] = {
	{{ 0.f, -1.f }},
	{{ 0.5f, -0.25f }},
	{{ 1.f, 0.25f }},
	{{ 0.5f, 0.5f }},
	{{ 1.f, 1.f }},
	{{ 0.5f, 0.75f }},
	{{ 0.f, 0.5f }},
	{{ -0.25f, -0.25f }},
	{{ 0.25f, 0.5f }},
	{{ 0.5f, 1.f }},
	{{ 1.f, 1.f }},
	{{ 0.75f, 0.f }},
	{{ 0.f, -0.25f }},
	{{ 0.5f, 0.5f }},
	{{ 1.f, 0.f }},
};
#define ASTEROIDC_COLLISION_DATA_COUNT 15
static_assert(ASTEROIDC_COLLISION_DATA_COUNT % 3 == 0, "");

const SPAN(HMM_Vec2) asteroida_collision_data = {
	.data = &asteroida_collision_data_data,
	.count = ASTEROIDA_COLLISION_DATA_COUNT,
};

const SPAN(HMM_Vec2) asteroidb_collision_data = {
	.data = &asteroidb_collision_data_data,
	.count = ASTEROIDB_COLLISION_DATA_COUNT,
};

const SPAN(HMM_Vec2) asteroidc_collision_data = {
	.data = &asteroidc_collision_data_data,
	.count = ASTEROIDC_COLLISION_DATA_COUNT,
};
