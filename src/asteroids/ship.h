#pragma once

#include <sokol_gfx.h>

struct ship {
	sg_bindings bind;
	sg_pipeline pip;
};

struct ship make_ship(void);
