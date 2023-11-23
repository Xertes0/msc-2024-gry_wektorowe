#include "object.h"

#include <stddef.h>

object_t g_objects[16];
size_t g_object_count = 0;

object_t *add_object(object_t obj)
{
	g_objects[g_object_count++] = obj;
	return &g_objects[g_object_count - 1];
}
