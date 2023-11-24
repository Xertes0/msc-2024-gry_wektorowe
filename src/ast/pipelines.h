#pragma once

#include <sokol_gfx.h>

/* LINES will probably be the only pipeline
   needed for objects (or at all) */
/* TODO: Handle 0 types. */
typedef enum {
	_PIPTYPE_DEFAULT = 0,
	PIPTYPE_LINES,
	PIPTYPE_COUNT,
} pip_type_t;

typedef enum {
	_BINDTYPE_DEFAULT = 0,
	BINDTYPE_SHIPA,
	BINDTYPE_SHIPB,
	BINDTYPE_ASTEROIDA,
	BINDTYPE_COUNT,
} bind_type_t;

extern sg_pipeline g_pipelines[PIPTYPE_COUNT];
extern sg_bindings g_bindings[BINDTYPE_COUNT];
extern size_t g_index_count[BINDTYPE_COUNT];

/* Load shaders at startup */
void load_pipelines(void);
void load_bindings(void);
