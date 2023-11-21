#pragma sokol @vs vs

in vec4 position;

out vec4 color;

uniform vs_params {
	vec2 u_res;
};

void main()
{
	float x_scale = min(u_res.y / u_res.x, 1.0);
	float y_scale = min(u_res.x / u_res.y, 1.0);
	gl_Position = vec4(position.x * x_scale,
			   position.y * y_scale,
			   position.z, position.w);
	color = vec4(1.0, 1.0, 1.0, 1.0);
}

#pragma sokol @end

#pragma sokol @fs fs

in vec4 color;
out vec4 frag_color;

void main()
{
	frag_color = color;
}

#pragma sokol @end

#pragma sokol @program lines vs fs
