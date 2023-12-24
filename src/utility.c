#include "utility.h"

#include <assert.h>
#include <stdio.h>
#include <time.h>

uint64_t get_msec(void)
{
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
		perror("clock_gettime()");
		assert(0);
	}
	return (((uint64_t) ts.tv_sec) * 1000) +
		(((uint64_t) ts.tv_nsec) / 1000000);
}
