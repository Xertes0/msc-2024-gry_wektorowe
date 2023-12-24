#include "object.h"

#include <assert.h>

#include "debug.h"
#include "state.h"

#define SHIP_SCALE (0.075f)
#define BULLET_SCALE (0.005f)

static void bullet_tick(object_t *obj)
{
	if (get_msec() >= obj->spawn_time + 1500) {
		g_objects[obj - g_objects] = g_objects[--g_object_count];
		return;
	}

	/* TODO: Move this into object flag? */
	obj->move.pos = HMM_AddV2(obj->move.pos, obj->move.vel);

	HMM_Mat4 my_mp = HMM_MulM4(g_state.projection, object_mat(obj));
	HMM_Vec2 p = HMM_MulM4V4(my_mp, HMM_V4(0.f, 0.f, 0.f, 1.f)).XY;

	FOREACH_OBJECT(other) {
		if (!(other->flags & OF_BULLET_TARGET))
			continue;

		HMM_Mat4 other_mp = HMM_MulM4(g_state.projection, object_mat(other));

		/* https://math.stackexchange.com/a/51459 */
		for (size_t i=0; i<other->collision.count; i += 3) {
			#define CROSS(L, R) \
				(((L).X * (R).Y) - ((L).Y * (R).X))
			HMM_Vec2 a = ((HMM_Vec2 *) other->collision.data)[i];
			HMM_Vec2 b = ((HMM_Vec2 *) other->collision.data)[i+1];
			HMM_Vec2 c = ((HMM_Vec2 *) other->collision.data)[i+2];
			a = HMM_MulM4V4(other_mp, HMM_V4(a.X, a.Y, 0.f, 1.f)).XY;
			b = HMM_MulM4V4(other_mp, HMM_V4(b.X, b.Y, 0.f, 1.f)).XY;
			c = HMM_MulM4V4(other_mp, HMM_V4(c.X, c.Y, 0.f, 1.f)).XY;

			float xd = CROSS(a, b) + CROSS(b, c) + CROSS(c, a);
			if (fabsf(xd) <= 1e-13)
				continue;

			float xa = CROSS(b, c) + CROSS(p, HMM_SubV2(b, c));
			float xb = CROSS(c, a) + CROSS(p, HMM_SubV2(c, a));
			float xc = CROSS(a, b) + CROSS(p, HMM_SubV2(a, b));

			if (xa >= 0.f && xb >= 0.f && xc >= 0.f &&
			    xa <= 1.f && xb <= 1.f && xc <= 1.f) {
                                /* Destory bullet. */
				asteroid_hit(other, obj);
				*obj = g_objects[--g_object_count];
				return;
			}
			#undef CROSS
		}
	}
}

static void register_new_bullet(HMM_Vec2 pos, HMM_Vec2 ship_vel, float rot)
{
	HMM_Vec2 offset = HMM_RotateV2(HMM_V2(0.f, SHIP_SCALE), rot);
	pos = HMM_AddV2(pos, offset);
	HMM_Vec2 vel = HMM_AddV2(HMM_RotateV2(HMM_V2(0.f, 0.01f), rot),
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

static void ship_tick(object_t *obj)
{
        /* Movement */

        /* TODO: Some kind of delta time */
	if (obj->ship.pleft || obj->ship.pright) {
		const float angle = obj->ship.pleft?1.f:-1.f;
		obj->move.rot += angle * 0.05f;
	}
	if (obj->ship.pup) {
		obj->move.vel = HMM_AddV2(obj->move.vel,
		                          HMM_RotateV2(HMM_V2(0.f, 0.0005f),
		                                       obj->move.rot));
	}
	obj->move.pos = HMM_AddV2(obj->move.pos, obj->move.vel);
	obj->move.vel = HMM_MulV2F(obj->move.vel, 0.99f);

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
