
#version 430 core
layout(location = 1) uniform sampler2D u_font_texture;
in vec2 g_font_pos;
out vec4 out_color;

void main()
{
	out_color = vec4(texture(u_font_texture, g_font_pos));
}
