#include "object.h"

#include <assert.h>
#include <stdio.h>

#include "debug.h"
#include "state.h"
#include "utility.h"

#define SHIP_SCALE (0.075f)
#define BULLET_SCALE (0.005f)

static float cross2(HMM_Vec2 a, HMM_Vec2 b)
{
	return (a.X * b.Y) - (a.Y * b.X);
}

static bool point_in_triangle(HMM_Vec2 p,
                              HMM_Vec2 a, HMM_Vec2 b, HMM_Vec2 c)
{
	float xd = cross2(a, b) + cross2(b, c) + cross2(c, a);
	if (fabsf(xd) <= 1e-13)
		return false;

	float xa = cross2(b, c) + cross2(p, HMM_SubV2(b, c));
	float xb = cross2(c, a) + cross2(p, HMM_SubV2(c, a));
	float xc = cross2(a, b) + cross2(p, HMM_SubV2(a, b));

	return (xa >= 0.f && xb >= 0.f && xc >= 0.f &&
	        xa <= 1.f && xb <= 1.f && xc <= 1.f);
}

static void bullet_tick(object_t *obj)
{
	if (get_msec() >= obj->spawn_time + 1500) {
		*obj = g_objects[--g_object_count];
		return;
	}

	/* TODO: Move this into object flag? */
	obj->move.pos = HMM_AddV2(obj->move.pos,
	                          HMM_MulV2F(obj->move.vel, g_state.dtime));

	HMM_Mat4 my_mp = HMM_MulM4(g_state.projection, object_mat(obj));
	HMM_Vec2 p = HMM_MulM4V4(my_mp, HMM_V4(0.f, 0.f, 0.f, 1.f)).XY;

	FOREACH_OBJECT(other) {
		if (!(other->flags & OF_BULLET_TARGET))
			continue;

		if (HMM_LenSqrV2(HMM_SubV2(other->move.pos, obj->move.pos)) > 0.1f)
			continue;

		if (g_state.draw_debug)
			debug_line(obj->move.pos, other->move.pos, false);

		HMM_Mat4 other_mp = HMM_MulM4(g_state.projection, object_mat(other));

		/* https://math.stackexchange.com/a/51459 */
		for (size_t i=0; i<other->collision.count; i += 3) {
			HMM_Vec2 a = ((HMM_Vec2 *) other->collision.data)[i];
			HMM_Vec2 b = ((HMM_Vec2 *) other->collision.data)[i+1];
			HMM_Vec2 c = ((HMM_Vec2 *) other->collision.data)[i+2];
			a = HMM_MulM4V4(other_mp, HMM_V4(a.X, a.Y, 0.f, 1.f)).XY;
			b = HMM_MulM4V4(other_mp, HMM_V4(b.X, b.Y, 0.f, 1.f)).XY;
			c = HMM_MulM4V4(other_mp, HMM_V4(c.X, c.Y, 0.f, 1.f)).XY;

			if (point_in_triangle(p, a, b, c)) {
                                /* Destory bullet. */
				asteroid_hit(other, obj);
				*obj = g_objects[--g_object_count];
				return;
			}
		}
	}
}

static void register_new_bullet(HMM_Vec2 pos, HMM_Vec2 ship_vel, float rot)
{
	HMM_Vec2 offset = HMM_RotateV2(HMM_V2(0.f, SHIP_SCALE), rot);
	pos = HMM_AddV2(pos, offset);
	HMM_Vec2 vel = HMM_AddV2(HMM_RotateV2(HMM_V2(0.f, 0.5f), rot),
	                         ship_vel);
	add_object((object_t) {
			.tick = bullet_tick,
			.flags = OF_MOVING,
			.pip_type = PIPTYPE_LINES,
			.bind_type = BINDTYPE_BULLET,
			.model_mat = HMM_Scale(HMM_V3(BULLET_SCALE, BULLET_SCALE, 1.f)),
			.spawn_time = get_msec(),
			.move = {
				.pos = pos,
				.vel = vel,
				.rot = rot,
			},
		});
}

static void ship_event(object_t *obj, const sapp_event *event)
{
	switch (event->type) {
	case SAPP_EVENTTYPE_KEY_DOWN: {
		switch (event->key_code) {
		case SAPP_KEYCODE_UP: {
			obj->ship.pup = true;
		}; break;
		case SAPP_KEYCODE_LEFT: {
			obj->ship.pleft = true;
		}; break;
		case SAPP_KEYCODE_RIGHT: {
			obj->ship.pright = true;
		}; break;
		case SAPP_KEYCODE_SPACE: {
			register_new_bullet(obj->move.pos,
			                    obj->move.vel,
			                    obj->move.rot);
		}; break;
		default: break;
		}
	}; break;
	case SAPP_EVENTTYPE_KEY_UP: {
		switch (event->key_code) {
		case SAPP_KEYCODE_UP: {
			obj->ship.pup = false;
		}; break;
		case SAPP_KEYCODE_LEFT: {
			obj->ship.pleft = false;
		}; break;
		case SAPP_KEYCODE_RIGHT: {
			obj->ship.pright = false;
		}; break;
		default: break;
		}
	}; break;
	default: break;
	}
}

/* https://stackoverflow.com/a/23356273 */
/* TODO: FIXME
static bool lines_intersect(HMM_Vec2 a1, HMM_Vec2 b1,
                            HMM_Vec2 a2, HMM_Vec2 b2)
{
	float d = (b2.Y-a2.Y)*(b1.X-a1.X)-(b2.X-a2.X)*(b1.Y-a1.Y);
	float u = (b2.X-a2.X)*(a1.Y-a2.Y)-(b2.Y-a2.Y)*(a1.X-a2.X);
	float v = (b1.X-a1.X)*(a1.Y-a2.Y)-(b1.Y-a1.Y)*(a1.X-a2.X);
	if (d < 0.f) {
		u = -u;
		v = -v;
		d = -d;
	}

	return (0.f <= u <= d) && (0.f <= v <= d);

	alternative:
	return fabsf(((a1.X - b1.X) * (a2.Y - b2.Y)) -
	             ((a1.Y - b1.Y) * (a2.X - b2.X))) < 0.000001f;
}
*/

static bool triangles_intersect(HMM_Vec2 a1, HMM_Vec2 b1, HMM_Vec2 c1,
                                HMM_Vec2 a2, HMM_Vec2 b2, HMM_Vec2 c2)
{
/*
	if (lines_intersect(a1,b1,a2,b2)) return true;
	if (lines_intersect(a1,b1,a2,c2)) return true;
	if (lines_intersect(a1,b1,b2,c2)) return true;
	if (lines_intersect(a1,c1,a2,b2)) return true;
	if (lines_intersect(a1,c1,a2,c2)) return true;
	if (lines_intersect(a1,c1,b2,c2)) return true;
	if (lines_intersect(b1,c1,a2,b2)) return true;
	if (lines_intersect(b1,c1,a2,c2)) return true;
	if (point_in_triangle(a2, a1,b1,c1)) return true;
	if (point_in_triangle(a1, a2,b2,c2)) return true;
*/
	if (point_in_triangle(a1, a2,b2,c2)) return true;
	if (point_in_triangle(b1, a2,b2,c2)) return true;
	if (point_in_triangle(c1, a2,b2,c2)) return true;

	if (point_in_triangle(a2, a1,b1,c1)) return true;
	if (point_in_triangle(b2, a1,b1,c1)) return true;
	if (point_in_triangle(c2, a1,b1,c1)) return true;

	return false;
}

/* TODO: Make ship's velocity non-linear. */
/* TODO: Make ship collide with it's own bullets. */
static void ship_tick(object_t *obj)
{
        /* Movement */
	if (obj->ship.pleft || obj->ship.pright) {
		const float angle = obj->ship.pleft?1.f:-1.f;
		obj->move.rot += angle * 4.f * g_state.dtime;
	}
	if (obj->ship.pup) {
		obj->move.vel = HMM_AddV2(obj->move.vel,
		                          HMM_RotateV2(HMM_V2(0.f, 0.05f),
		                                       obj->move.rot));
	}
	obj->move.pos = HMM_AddV2(obj->move.pos,
	                          HMM_MulV2F(obj->move.vel, g_state.dtime));
	obj->move.vel = HMM_MulV2F(obj->move.vel, 0.99f);

	HMM_Mat4 my_mp = HMM_MulM4(g_state.projection, object_mat(obj));
        /* From pipelines.c ship_vertices[] */
	HMM_Vec2 ship_a = HMM_MulM4V4(my_mp, HMM_V4(0.f, 1.f, 0.f, 1.f)).XY;
	HMM_Vec2 ship_b = HMM_MulM4V4(my_mp, HMM_V4(0.425f, -0.65f, 0.f, 1.f)).XY;
	HMM_Vec2 ship_c = HMM_MulM4V4(my_mp, HMM_V4(-0.425f, -0.65f, 0.f, 1.f)).XY;
	FOREACH_OBJECT(other) {
		if (!(other->flags & OF_ASTEROID))
			continue;

		if (HMM_LenSqrV2(HMM_SubV2(other->move.pos, obj->move.pos)) > 0.1f)
			continue;

		if (g_state.draw_debug)
			debug_line(obj->move.pos, other->move.pos, false);

		HMM_Mat4 other_mp = HMM_MulM4(g_state.projection, object_mat(other));

		/* https://math.stackexchange.com/a/51459 */
		for (size_t i=0; i<other->collision.count; i += 3) {
			HMM_Vec2 a = ((HMM_Vec2 *) other->collision.data)[i];
			HMM_Vec2 b = ((HMM_Vec2 *) other->collision.data)[i+1];
			HMM_Vec2 c = ((HMM_Vec2 *) other->collision.data)[i+2];
			a = HMM_MulM4V4(other_mp, HMM_V4(a.X, a.Y, 0.f, 1.f)).XY;
			b = HMM_MulM4V4(other_mp, HMM_V4(b.X, b.Y, 0.f, 1.f)).XY;
			c = HMM_MulM4V4(other_mp, HMM_V4(c.X, c.Y, 0.f, 1.f)).XY;

			if (triangles_intersect(ship_a, ship_b, ship_c,
			                        a, b, c)) {
				printf("Hit something");
				assert(false && "Hit something");
				break;
			}
		}
	}

        /* Switch variant every few ticks */
	if (obj->ship.pup) {
		obj->ship.flip++;
		obj->bind_type =
			(obj->ship.flip>>2)%2==0?BINDTYPE_SHIPA:BINDTYPE_SHIPB;
	} else {
		obj->bind_type = BINDTYPE_SHIPA;
	}
}

void register_new_ship(void)
{
	static bool called = false;
	assert(!called); called = true;

	add_object((object_t) {
			.tick = ship_tick,
			.event = ship_event,
			.flags = OF_MOVING,
			.model_mat = HMM_Scale(HMM_V3(SHIP_SCALE, SHIP_SCALE, 1.f)),
			.pip_type = PIPTYPE_LINES,
			.bind_type = BINDTYPE_SHIPA,
		});
}
