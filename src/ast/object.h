#pragma once

#include <sokol_app.h>
#include <HandmadeMath.h>

#include "ship.h"
#include "pipelines.h"

typedef struct {
	pip_type_t pip_type;
	bind_type_t bind_type;
	HMM_Mat4 model_mat;
	union {
		ship_t ship;
	};
} object_t;

extern object_t g_objects[16];
extern size_t g_object_count;

object_t *add_object(object_t obj);
