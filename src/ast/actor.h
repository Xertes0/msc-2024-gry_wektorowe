#pragma once

#include <sokol_app.h>

#include "object.h"

struct actor {
	/* Every function is optional */
	void (*tick)(struct actor *);
	void (*event)(struct actor *, const sapp_event *);

        /* Object being acted on */
	object_t *obj;
	union {
		struct {
			bool pup : 1;
			bool pleft : 1;
			bool pright : 1;
			uint8_t flip;
		} ship_mov;
	};
};
typedef struct actor actor_t;

void add_actor(actor_t act);
void actors_tick();
void actors_event(const sapp_event *);
