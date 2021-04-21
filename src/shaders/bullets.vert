
#version 430 core
layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec3 in_color;
layout(location = 2) in float in_angle;
layout(location = 3) in vec2 in_tail_pos;
out vec3 v_color;
out float v_angle;
out vec2 v_tail_pos;

void main()
{
	gl_Position = vec4(in_pos, 0.0, 1.0);
	v_color = in_color;
	v_angle = in_angle;
	v_tail_pos = in_tail_pos;
}
