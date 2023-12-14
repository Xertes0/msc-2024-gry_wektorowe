#pragma once

#include <sokol_gfx.h>

#define PIP_CULLMODE (SG_CULLMODE_NONE)
#define PIP_SAMPLE_COUNT (1)
#define PIP_PIXEL_FORMAT (SG_PIXELFORMAT_RGBA8)

/* LINES will probably be the only pipeline
   needed for objects (or at all) */
/* TODO: Handle 0 types. */
typedef enum {
	_PIPTYPE_DEFAULT = 0,
	PIPTYPE_LINES,
	PIPTYPE_DEBUG_CIRCLE,
	PIPTYPE_DEBUG_TRIANGLE,
	PIPTYPE_COUNT,
} pip_type_t;

typedef enum {
	_BINDTYPE_DEFAULT = 0,
	BINDTYPE_SHIPA,
	BINDTYPE_SHIPB,
	BINDTYPE_BULLET,
	BINDTYPE_ASTEROIDA,
	BINDTYPE_ASTEROIDB,
	BINDTYPE_ASTEROIDC,
	BINDTYPE_COUNT,
} bind_type_t;

extern sg_pipeline g_pipelines[PIPTYPE_COUNT];
extern sg_bindings g_bindings[BINDTYPE_COUNT];
extern size_t g_index_count[BINDTYPE_COUNT];

/* Load shaders at startup */
void load_pipelines(void);
void load_bindings(void);
