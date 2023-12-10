#pragma once

typedef struct {
	void *data;
	size_t count;
} span_t;

#define SPAN(T) span_t
