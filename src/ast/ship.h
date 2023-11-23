#pragma once

#include <HandmadeMath.h>

/* TODO: Consider removing this header
         and jam everything into object.h */
typedef struct {
	HMM_Vec3 pos;
	HMM_Vec3 vel;
	float rot;
} ship_t;

/* TODO: this name is lame */
void register_new_ship(void);
