#include "ship.h"

#include <assert.h>

#include "actor.h"
#include "object.h"

#include "lines.glsl.h"

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

	HMM_Mat4 *model = &act->obj->model_mat;
	ship_t *ship = &act->obj->ship;
        /* TODO: Some kind of delta time */
	if (act->ship_mov.pleft || act->ship_mov.pright) {
		const float angle = act->ship_mov.pleft?1.f:-1.f;
		ship->rot += angle * 0.05f;
	}
	if (act->ship_mov.pup) {
		HMM_Vec2 vec = HMM_RotateV2(HMM_V2(0.f, 0.005f), ship->rot);
		ship->vel = HMM_AddV3(ship->vel, HMM_V3(vec.X, vec.Y, 0.f));
	}
	ship->pos = HMM_AddV3(ship->pos, ship->vel);
#define SHIP_SCALE 0.075f
	*model = HMM_MulM4(HMM_Scale(HMM_V3(SHIP_SCALE, SHIP_SCALE, 1.f)),
	                   HMM_Translate(ship->pos));
	*model = HMM_MulM4(*model,
	                   HMM_Rotate_RH(ship->rot, HMM_V3(0.f, 0.f, 1.f)));

	ship->vel = HMM_MulV3F(ship->vel, 0.99f);

        /* Switch variant every few ticks */

	static uint32_t flip = 0; flip++;
	act->obj->bind_type = (flip>>2)%2==0?BINDTYPE_SHIPA:BINDTYPE_SHIPB;
}

void register_new_ship(void)
{
	static bool called = false;
	assert(!called); called = true;

	object_t *ship = add_object((object_t) {});
	add_actor((actor_t) {
			.tick = ship_tick,
			.event = ship_event,
			.obj = ship,
		});
}
