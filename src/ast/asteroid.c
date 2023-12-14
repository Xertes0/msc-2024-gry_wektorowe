#include "object.h"

#include "debug.h"
#include "state.h"

#include <assert.h>
#include <stdlib.h>

static void ast_tick(object_t *obj)
{
        /* Movement */

        /* TODO: Some kind of delta time */
	obj->move.rot += 0.005f;
	obj->move.pos = HMM_AddV2(obj->move.pos, obj->move.vel);

	/* Debug hitboxes */
#if 0
	HMM_Mat4 obj_mat = HMM_MulM4(g_state.projection, object_mat(obj));

	for (size_t i=0; i<obj->collision.count; i += 3) {
		HMM_Vec2 a = ((HMM_Vec2 *) obj->collision.data)[i];
		HMM_Vec2 b = ((HMM_Vec2 *) obj->collision.data)[i+1];
		HMM_Vec2 c = ((HMM_Vec2 *) obj->collision.data)[i+2];
		a = HMM_MulM4V4(obj_mat, HMM_V4(a.X, a.Y, 0.f, 1.f)).XY;
		b = HMM_MulM4V4(obj_mat, HMM_V4(b.X, b.Y, 0.f, 1.f)).XY;
		c = HMM_MulM4V4(obj_mat, HMM_V4(c.X, c.Y, 0.f, 1.f)).XY;

		debug_triangle((HMM_Vec2[3]) {a, b, c},
		               true);

		debug_point(a, true);
		debug_point(b, true);
		debug_point(c, true);
	}
#endif
}

#define AST_SCALE 0.2f
static object_t build_base_asteroid(uint8_t stage)
{
	int i = (rand() % 3);

	float stage_factor = 1.f;
	switch (stage) {
	case 0:
		break;
	case 1:
		stage_factor = 0.5f;
		break;
	case 2:
		stage_factor = 0.25f;
		break;
	default: assert(false);
	}

	object_t ast = {
		.tick = ast_tick,
		.flags = OF_MOVING | OF_BULLET_TARGET,
		.model_mat = HMM_Scale(HMM_V3(AST_SCALE * stage_factor,
		                              AST_SCALE * stage_factor,
		                              1.f)),
		.pip_type = PIPTYPE_LINES,
		.bind_type = (bind_type_t) (BINDTYPE_ASTEROIDA + i),
		.ast.stage = stage,
	};

	switch (i) {
	case 0:
		ast.collision = asteroida_collision_data;
		break;
	case 1:
		ast.collision = asteroidb_collision_data;
		break;
	case 2:
		ast.collision = asteroidc_collision_data;
		break;
	default: assert(false);
	}

	return ast;
}

void register_new_asteroid(void)
{
	for (size_t i=BINDTYPE_ASTEROIDA; i <= BINDTYPE_ASTEROIDC; ++i) {
		float offset = (float) (i - BINDTYPE_ASTEROIDA);
		object_t ast = build_base_asteroid(0);
		ast.move.pos = HMM_V2(-0.5f + (0.5f * offset), 0.f);
		ast.move.vel = HMM_V2(0.0001f, 0.00005f);
		add_object(ast);
	}
}

void asteroid_hit(object_t *ast, object_t *bullet)
{
	uint8_t stage = ast->ast.stage;
	if (stage == 2)
		goto ret;

	for (size_t i=0; i<2; ++i) {
		object_t new = build_base_asteroid(stage + 1);
		new.move = ast->move;
		new.move.vel = HMM_AddV2(new.move.vel,
		                         HMM_MulV2F(bullet->move.vel, 0.25f));
		#define RND (1.f - ((float) rand() / (float) RAND_MAX) * 2.f)
		new.move.pos = HMM_AddV2(new.move.pos,
		                         HMM_V2(RND/10.f,
		                                RND/10.f));
		#undef RND
		add_object(new);
	}

ret:
	*ast = g_objects[--g_object_count];
}

static HMM_Vec2 asteroida_collision_data_data[] = {
	{{ 1.f, 0.5f }},
	{{ 0.25f, 1.f }},
	{{ -0.5f, 1.f }},
	{{ -0.25f, 0.5f }},
	{{ -1.f, 0.5f }},
	{{ -1.f, -0.25f }},
	{{ -1.f, -0.25f }},
	{{ -0.5f, -1.f }},
	{{ 0.25f, -0.75f }},
	{{ 0.25f, -0.75f }},
	{{ 0.5f, -1.f }},
	{{ 1.f, -0.5f }},
	{{ 0.25f, 0.f }},
	{{ 1.f, 0.25f }},
	{{ 1.f, 0.5f }},
	{{ 1.f, 0.5f }},
	{{ -0.5f, 1.f }},
	{{ -0.25f, 0.5f }},
	{{ -0.25f, 0.5f }},
	{{ -1.f, -0.25f }},
	{{ 0.25f, -0.75f }},
	{{ 0.25f, -0.75f }},
	{{ 1.f, -0.5f }},
	{{ 0.25f, 0.f }},
	{{ 0.25f, 0.f }},
	{{ 1.f, 0.5f }},
	{{ -0.25f, 0.5f }},
	{{ -0.25f, 0.5f }},
	{{ 0.25f, -0.75f }},
	{{ 0.25f, 0.f }},
};
#define ASTEROIDA_COLLISION_DATA_COUNT 30
static_assert(ASTEROIDA_COLLISION_DATA_COUNT % 3 == 0, "");

static HMM_Vec2 asteroidb_collision_data_data[] = {
	{{ 0.f, 0.5f }},
	{{ -0.5f, 1.f }},
	{{ -1.f, 0.5f }},
	{{ -1.f, 0.5f }},
	{{ -1.f, -0.5f }},
	{{ -0.5f, -1.f }},
	{{ -0.5f, -1.f }},
	{{ 0.25f, -1.f }},
	{{ 1.f, -0.5f }},
	{{ 0.75f, 0.f }},
	{{ 1.f, 0.5f }},
	{{ 0.5f, 1.f }},
	{{ 0.f, 0.5f }},
	{{ -1.f, 0.5f }},
	{{ -0.5f, -1.f }},
	{{ -0.5f, -1.f }},
	{{ 1.f, -0.5f }},
	{{ 0.75f, 0.f }},
	{{ 0.75f, 0.f }},
	{{ 0.5f, 1.f }},
	{{ 0.f, 0.5f }},
	{{ 0.f, 0.5f }},
	{{ -0.5f, -1.f }},
	{{ 0.75f, 0.f }},
};
#define ASTEROIDB_COLLISION_DATA_COUNT 24
static_assert(ASTEROIDB_COLLISION_DATA_COUNT % 3 == 0, "");

static HMM_Vec2 asteroidc_collision_data_data[] = {
	{{ 0.f, 0.75f }},
	{{ -0.5f, 1.f }},
	{{ -1.f, 0.5f }},
	{{ -0.75f, 0.f }},
	{{ -1.f, -0.5f }},
	{{ -0.5f, -1.f }},
	{{ -0.25f, -0.75f }},
	{{ 0.5f, -1.f }},
	{{ 1.f, -0.25f }},
	{{ 0.5f, 0.25f }},
	{{ 1.f, 0.5f }},
	{{ 0.5f, 1.f }},
	{{ 0.f, 0.75f }},
	{{ -1.f, 0.5f }},
	{{ -0.75f, 0.f }},
	{{ -0.75f, 0.f }},
	{{ -0.5f, -1.f }},
	{{ -0.25f, -0.75f }},
	{{ -0.25f, -0.75f }},
	{{ 1.f, -0.25f }},
	{{ 0.5f, 0.25f }},
	{{ 0.5f, 0.25f }},
	{{ 0.5f, 1.f }},
	{{ 0.f, 0.75f }},
	{{ 0.f, 0.75f }},
	{{ -0.75f, 0.f }},
	{{ -0.25f, -0.75f }},
	{{ -0.25f, -0.75f }},
	{{ 0.5f, 0.25f }},
	{{ 0.f, 0.75f }},
};
#define ASTEROIDC_COLLISION_DATA_COUNT 30
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
