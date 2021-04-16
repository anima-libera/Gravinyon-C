
#version 430 core
layout(location = 0) uniform float u_aspect_ratio;
layout(location = 0) in vec2 in_pos;
layout(location = 1) in float in_size;
layout(location = 2) in vec3 in_color;
out vec3 v_color;

void main()
{
	gl_Position = vec4(in_pos, 0.0, 1.0);
	gl_Position.y *= u_aspect_ratio;
	gl_PointSize = in_size;
	v_color = in_color;
}
