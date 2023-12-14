#pragma once

#include <HandmadeMath.h>
#include <sokol_gfx.h>

#define OFFSCREEN_SHADER_COUNT 4

typedef struct {
	sg_pass_action pass_action;
	HMM_Mat4 projection;
	struct {
		sg_pass pass;
		sg_bindings bind;
		sg_pipeline pip[OFFSCREEN_SHADER_COUNT];
		size_t selected;
	} offscr;
	/* TODO: Current time tracing is very sketchy. */
	float time;
	bool draw_hitboxes;
} game_state_t;

extern game_state_t g_state;
