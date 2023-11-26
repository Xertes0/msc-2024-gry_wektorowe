#include "object.h"

#include <assert.h>

#include "actor.h"

#define SHIP_SCALE (0.075f)
#define BULLET_SCALE (0.005f)

/* TODO: Move this into object flag? */
static void bullet_tick(actor_t *act)
{
	act->obj->move.pos = HMM_AddV3(act->obj->move.pos, act->obj->move.vel);
}

static void register_new_bullet(HMM_Vec3 pos, HMM_Vec3 ship_vel, float rot)
{
	HMM_Vec2 offset = HMM_RotateV2(HMM_V2(0.f, SHIP_SCALE), rot);
	pos = HMM_AddV3(pos, HMM_V3(offset.X, offset.Y, 0.f));
	HMM_Vec2 rotv = HMM_RotateV2(HMM_V2(0.f, 0.01f), rot);
	HMM_Vec3 vel = HMM_AddV3(HMM_V3(rotv.X, rotv.Y, 0.f), ship_vel);
	object_t *obj = add_object((object_t) {
			.flags = OF_MOVING,
			.pip_type = PIPTYPE_LINES,
			.bind_type = BINDTYPE_BULLET,
			.model_mat = HMM_Scale(HMM_V3(BULLET_SCALE, BULLET_SCALE, 1.f)),
			.move = {
				.pos = pos,
				.vel = vel,
				.rot = rot,
			},
		});
	add_actor((actor_t) {
			.tick = bullet_tick,
			.obj = obj,
		});
}

static void ship_event(actor_t *act, const sapp_event *event)
{
	switch (event->type) {
	case SAPP_EVENTTYPE_KEY_DOWN: {
		switch (event->key_code) {
		case SAPP_KEYCODE_UP: {
			act->ship_mov.pup = true;
		}; break;
		case SAPP_KEYCODE_LEFT: {
			act->ship_mov.pleft = true;
		}; break;
		case SAPP_KEYCODE_RIGHT: {
			act->ship_mov.pright = true;
		}; break;
		case SAPP_KEYCODE_SPACE: {
			register_new_bullet(act->obj->move.pos,
			                    act->obj->move.vel,
			                    act->obj->move.rot);
		}; break;
		default: break;
		}
	}; break;
	case SAPP_EVENTTYPE_KEY_UP: {
		switch (event->key_code) {
		case SAPP_KEYCODE_UP: {
			act->ship_mov.pup = false;
		}; break;
		case SAPP_KEYCODE_LEFT: {
			act->ship_mov.pleft = false;
		}; break;
		case SAPP_KEYCODE_RIGHT: {
			act->ship_mov.pright = false;
		}; break;
		default: break;
		}
	}; break;
	default: break;
	}
}

static void ship_tick(actor_t *act)
{
        /* Movement */

	object_t *obj = act->obj;
        /* TODO: Some kind of delta time */
	if (act->ship_mov.pleft || act->ship_mov.pright) {
		const float angle = act->ship_mov.pleft?1.f:-1.f;
		obj->move.rot += angle * 0.05f;
	}
	if (act->ship_mov.pup) {
		HMM_Vec2 vec = HMM_RotateV2(HMM_V2(0.f, 0.0005f), obj->move.rot);
		obj->move.vel = HMM_AddV3(obj->move.vel, HMM_V3(vec.X, vec.Y, 0.f));
	}
	obj->move.pos = HMM_AddV3(obj->move.pos, obj->move.vel);
	obj->move.vel = HMM_MulV3F(obj->move.vel, 0.99f);

        /* Switch variant every few ticks */

	if (act->ship_mov.pup) {
		act->ship_mov.flip++;
		act->obj->bind_type =
			(act->ship_mov.flip>>2)%2==0?BINDTYPE_SHIPA:BINDTYPE_SHIPB;
	} else {
		act->obj->bind_type = BINDTYPE_SHIPA;
	}
}

void register_new_ship(void)
{
	static bool called = false;
	assert(!called); called = true;

	object_t *ship = add_object((object_t) {
			.flags = OF_MOVING,
			.model_mat = HMM_Scale(HMM_V3(SHIP_SCALE, SHIP_SCALE, 1.f)),
			.pip_type = PIPTYPE_LINES,
			.bind_type = BINDTYPE_SHIPA,
		});
	add_actor((actor_t) {
			.tick = ship_tick,
			.event = ship_event,
			.obj = ship,
		});
}
