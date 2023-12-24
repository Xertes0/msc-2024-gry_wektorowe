#include "object.h"

#include <assert.h>
#include <stddef.h>

#include "state.h"

object_t g_objects[32];
size_t g_object_count = 0;

/* TODO: Make g_objects sorted by obj.pip_type. */
object_t *add_object(object_t obj)
{
	g_objects[g_object_count++] = obj;
	return &g_objects[g_object_count - 1];
}


void tick_objects(void)
{
	for (size_t i=0; i<g_object_count; ++i) {
		object_t *obj = &g_objects[i];
		HMM_Mat4 mp = HMM_MulM4(g_state.projection, object_mat(obj));
		HMM_Vec2 p = HMM_MulM4V4(mp, HMM_V4(0.f, 0.f, 0.f, 1.f)).XY;

                /* TODO: Shouldn't this depend on screen's aspect ratio? */
		const float xborder = 1.f + 1.f * obj->model_mat.Elements[0][0];
		const float yborder = 1.f + 1.f * obj->model_mat.Elements[1][1];
		if (p.X > xborder || p.X < -xborder) {
			obj->move.pos.X *= -1.f;
		}
		if (p.Y > yborder || p.Y < -yborder) {
			obj->move.pos.Y *= -1.f;
		}

		if (obj->tick)
			obj->tick(obj);
	}
}

void event_objects(const sapp_event *event)
{
	for (size_t i=0; i<g_object_count; ++i) {
		if (g_objects[i].event)
			(*g_objects[i].event)(&g_objects[i], event);
	}
}

HMM_Mat4 object_mat(const object_t *obj)
{
	assert(obj->flags & OF_MOVING);

	HMM_Mat4 mat = HMM_MulM4(HMM_Translate(HMM_V3(obj->move.pos.X,
	                                              obj->move.pos.Y,
	                                              0.f)),
	                         HMM_Rotate_RH(obj->move.rot, HMM_V3(0.f, 0.f, 1.f)));
	mat = HMM_MulM4(mat, obj->model_mat);
	return mat;
}
