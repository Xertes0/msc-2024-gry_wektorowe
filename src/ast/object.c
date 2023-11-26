#include "object.h"

#include <assert.h>
#include <stddef.h>

object_t g_objects[16];
size_t g_object_count = 0;

/* TODO: Make g_objects sorted by obj.pip_type. */
object_t *add_object(object_t obj)
{
	g_objects[g_object_count++] = obj;
	return &g_objects[g_object_count - 1];
}

HMM_Mat4 object_mat(const object_t *obj)
{
	assert(obj->flags & OF_MOVING);

	/* HMM_Mat4 mat = HMM_MulM4(obj->model_mat, */
	/*                          HMM_Translate(obj->move.pos)); */
	/* mat = HMM_MulM4(mat, */
	/*                 HMM_Rotate_RH(obj->move.rot, HMM_V3(0.f, 0.f, 1.f))); */

	HMM_Mat4 mat = HMM_MulM4(HMM_Translate(HMM_V3(obj->move.pos.X,
	                                              obj->move.pos.Y,
	                                              0.f)),
	                         HMM_Rotate_RH(obj->move.rot, HMM_V3(0.f, 0.f, 1.f)));
	mat = HMM_MulM4(mat, obj->model_mat);
	return mat;
}
