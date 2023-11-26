#include "actor.h"

#include <stddef.h>

static actor_t actors[16];
static size_t actor_count = 0;

void add_actor(actor_t act)
{
	actors[actor_count++] = act;
}

void actors_tick(void)
{
	for (size_t i=0; i<actor_count; ++i) {
		if (actors[i].tick) {
			(*actors[i].tick)(&actors[i]);
		}
	}
}

void actors_event(const sapp_event *event)
{
	for (size_t i=0; i<actor_count; ++i) {
		if (actors[i].event) {
			(*actors[i].event)(&actors[i], event);
		}
	}
}
