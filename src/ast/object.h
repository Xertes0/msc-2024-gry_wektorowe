#pragma once

#include <sokol_app.h>
#include <HandmadeMath.h>

#include "object.h"
#include "pipelines.h"
#include "utility.h"

typedef enum {
	OF_NONE,
	OF_MOVING = (1 << 0),
	OF_BULLET_TARGET = (1 << 1),
} object_flag_t;

typedef struct {
	object_flag_t flags;

	pip_type_t pip_type;
	bind_type_t bind_type;
	HMM_Mat4 model_mat;

        /* If OF_BULLET_TARGET, holds an array of triangle vertices. */
	SPAN(HMM_Vec2) collision;

	struct {
		HMM_Vec2 pos;
		HMM_Vec2 vel;
		float rot;
	} move;
} object_t;

extern object_t g_objects[16];
extern size_t g_object_count;

#define FOREACH_OBJECT(VAR) \
	for (object_t *VAR = g_objects; VAR < g_objects + g_object_count; ++VAR)

object_t *add_object(object_t obj);

HMM_Mat4 object_mat(const object_t *obj);

/* TODO: Lame names */
void register_new_ship(void);
void register_new_asteroid(void);

extern const SPAN(HMM_Vec2) asteroida_collision_data;
extern const SPAN(HMM_Vec2) asteroidb_collision_data;
extern const SPAN(HMM_Vec2) asteroidc_collision_data;
