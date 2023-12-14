#pragma sokol @vs vs_debug_circle

in vec4 position;

out vec4 frag_pos;

uniform vs_debug_circle_params {
	mat4 mvp;
	vec4 pos;
};

mat4 translate(vec2 vec)
{
	return mat4(vec4(1., 0., 0., 0.),
		    vec4(0., 1., 0., 0.),
		    vec4(0., 0., 1., 0.),
		    vec4(vec.xy, 0., 1.));
}

void main()
{
	gl_Position = mvp * translate(pos.xy) * position;
	frag_pos = mvp * position;
}

#pragma sokol @end

#pragma sokol @fs fs_debug_circle

in vec4 frag_pos;

out vec4 frag_color;

float circle(vec2 pos, float radius)
{
	return 1.0 - smoothstep(radius - (radius * 0.01),
				radius + (radius * 0.01),
				dot(pos, pos) * 4.0);
}

void main()
{
	float circ = circle(frag_pos.xy, 0.0005);
	frag_color = vec4(1. * circ, 0., 0., 1. - (0.5 * circ));
}

#pragma sokol @end

#pragma sokol @vs vs_debug_triangle

in float index;

uniform vs_debug_triangle_params {
	mat4 mvp;
	vec4 pos[3];
};

void main()
{
	gl_Position = mvp * vec4(pos[int(index)].xy, 0., 1.);
}

#pragma sokol @end

#pragma sokol @fs fs_debug_triangle

out vec4 frag_color;

void main()
{
	frag_color = vec4(0., 0., 1., 1.);
}

#pragma sokol @end

#pragma sokol @program debug_circle vs_debug_circle fs_debug_circle
#pragma sokol @program debug_triangle vs_debug_triangle fs_debug_triangle
