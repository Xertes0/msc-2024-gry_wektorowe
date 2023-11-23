#pragma sokol @vs vs

in vec4 position;

out vec4 color;

uniform vs_params {
	mat4 mvp;
};

void main()
{
	gl_Position = mvp * position;
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
