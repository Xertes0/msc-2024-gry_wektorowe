#pragma once

#include <sokol_app.h>
#include <HandmadeMath.h>

#include "object.h"
#include "pipelines.h"

typedef enum {
	OF_NONE,
	OF_MOVING = (1 << 0),
} object_flag_t;

typedef struct {
	object_flag_t flags;

	pip_type_t pip_type;
	bind_type_t bind_type;
	HMM_Mat4 model_mat;

	struct {
		HMM_Vec3 pos;
		HMM_Vec3 vel;
		float rot;
	} move;
} object_t;

extern object_t g_objects[16];
extern size_t g_object_count;

object_t *add_object(object_t obj);

HMM_Mat4 object_mat(object_t *obj);

void register_new_ship(void);
void register_new_asteroid(void);
