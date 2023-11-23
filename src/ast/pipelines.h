#pragma once

#include <sokol_gfx.h>

/* LINES will probably be the only pipeline
   needed for objects (or at all) */
typedef enum {
	PIPTYPE_LINES = 0,
	PIPTYPE_COUNT,
} pip_type_t;

typedef enum {
	BINDTYPE_SHIPA = 0,
	BINDTYPE_SHIPB = 1,
	BINDTYPE_COUNT,
} bind_type_t;

extern sg_pipeline g_pipelines[PIPTYPE_COUNT];
extern sg_bindings g_bindings[BINDTYPE_COUNT];
extern size_t g_index_count[BINDTYPE_COUNT];

/* Load shaders at startup */
void load_pipelines(void);
void load_bindings(void);
