#pragma sokol @vs vs_offscr0

in vec4 position;
in vec2 tex_coord;

out vec2 f_uv;

void main()
{
	gl_Position = position;
	f_uv = tex_coord;
}

#pragma sokol @end

#pragma sokol @fs fs_offscr0

uniform texture2D tex;
uniform sampler smp;

in vec2 f_uv;

out vec4 frag_color;

void main()
{
	frag_color = texture(sampler2D(tex, smp), f_uv);
}

#pragma sokol @end

#pragma sokol @program offscreen0 vs_offscr0 fs_offscr0
