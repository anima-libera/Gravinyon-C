
#version 430 core
in vec3 g_color;
out vec4 out_color;

void main()
{
	out_color = vec4(g_color, 1.0);
}
