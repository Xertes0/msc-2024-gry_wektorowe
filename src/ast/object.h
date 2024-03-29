#pragma once

#include <time.h>

#include <HandmadeMath.h>
#include <sokol_app.h>

#include "pipelines.h"
#include "utility.h"

typedef enum {
	OF_NONE,
	OF_MOVING = (1 << 0),
	OF_BULLET_TARGET = (1 << 1),
	OF_ASTEROID = (1 << 2),
} object_flag_t;

struct object {
	void (*tick)(struct object *);
	void (*event)(struct object *, const sapp_event *);

	object_flag_t flags;

	pip_type_t pip_type;
	bind_type_t bind_type;
	HMM_Mat4 model_mat;

	/* If OF_BULLET_TARGET, holds an array of triangle vertices. */
	SPAN(HMM_Vec2) collision;

	/* Optional */
	uint64_t spawn_time;

	struct {
		HMM_Vec2 pos;
		HMM_Vec2 vel;
		float rot;
	} move;

	union {
		struct {
			bool pup : 1;
			bool pleft : 1;
			bool pright : 1;
			uint8_t flip;
		} ship;
		struct {
			float rot_dir;
			uint8_t stage;
		} ast;
	};
};

typedef struct object object_t;

extern object_t g_objects[32];
extern size_t g_object_count;

#define _FOREACH_OBJECT(MODIF, VAR)	  \
	for (MODIF object_t *VAR = g_objects; VAR < g_objects + g_object_count; ++VAR)

#define FOREACH_OBJECT(VAR) _FOREACH_OBJECT(, VAR)
#define FOREACH_CONST_OBJECT(VAR) _FOREACH_OBJECT(const, VAR)

object_t *add_object(object_t obj);
void tick_objects(void);
void event_objects(const sapp_event *);

HMM_Mat4 object_mat(const object_t *obj);

void register_new_ship(void);
object_t *register_random_asteroid(void);

void asteroid_hit(object_t *ast, object_t *bullet);

extern const SPAN(HMM_Vec2) asteroida_collision_data;
extern const SPAN(HMM_Vec2) asteroidb_collision_data;
extern const SPAN(HMM_Vec2) asteroidc_collision_data;
