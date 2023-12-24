#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
	void *data;
	size_t count;
} span_t;

#define SPAN(T) span_t

uint64_t get_msec(void);
