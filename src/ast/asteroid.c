#include "object.h"

#include "debug.h"
#include "state.h"

#include <assert.h>
#include <stdlib.h>

#define ASTEROID_SCALE (0.2f)

static void ast_tick(object_t *obj)
{
	/* Movement */
	obj->move.rot += ((obj->move.vel.X * obj->move.vel.X) +
			  (obj->move.vel.Y * obj->move.vel.Y)) *
			 10.f * obj->ast.rot_dir * g_state.dtime;
	obj->move.pos = HMM_AddV2(obj->move.pos,
				  HMM_MulV2F(obj->move.vel, g_state.dtime));

	/* Debug hitboxes */
	if (g_state.draw_debug) {
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

			/* TODO: Fix laggy debug_point(). */
			/* debug_point(a, true); */
			/* debug_point(b, true); */
			/* debug_point(c, true); */
		}
	}
}

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
		.flags = OF_MOVING | OF_BULLET_TARGET | OF_ASTEROID,
		.model_mat = HMM_Scale(HMM_V3(ASTEROID_SCALE * stage_factor,
					      ASTEROID_SCALE * stage_factor,
					      1.f)),
		.pip_type = PIPTYPE_LINES,
		.bind_type = (bind_type_t) (BINDTYPE_ASTEROIDA + i),
		.ast.rot_dir = 1.f - (2.f * (float) (rand() % 2)),
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

object_t *register_random_asteroid(void)
{
	object_t ast = build_base_asteroid(0);
	ast.move.pos.X = (float) rand() / (float) RAND_MAX * 2.f - 1.f;
	ast.move.pos.Y = (float) rand() / (float) RAND_MAX * 2.f - 1.f;
	ast.move.vel = HMM_V2(((float) (rand() % 10) - 5.f) * 0.075f,
			      ((float) (rand() % 10) - 5.f) * 0.075f);
	return add_object(ast);
}

void asteroid_hit(object_t *ast, object_t *bullet)
{
	uint8_t stage = ast->ast.stage;
	if (stage == 2)
		goto ret;

	for (size_t i=0; i<2; ++i) {
		object_t new = build_base_asteroid(stage + 1);
		new.move = ast->move;
		if (i == 0) {
			new.move.vel = HMM_AddV2(new.move.vel,
						 HMM_MulV2F(bullet->move.vel, 0.25f));
		} else {
			new.move.vel = HMM_AddV2(new.move.vel,
						 HMM_MulV2F(bullet->move.vel, -(0.25f / 2.f)));
		}
		new.move.vel = HMM_RotateV2(new.move.vel, 90.f);
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
