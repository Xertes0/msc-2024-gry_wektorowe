#pragma once

#include <stdbool.h>

#include <HandmadeMath.h>

void init_debug(void);
void debug_end_frame(void);
void draw_debug(void);

void debug_point(HMM_Vec2 pos, bool projected);
void debug_triangle(HMM_Vec2 verts[3], bool projected);
