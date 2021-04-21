
#version 430 core
layout(location = 0) in vec2 in_pos;
layout(location = 1) in float in_draw_angle;
out float v_draw_angle;

void main()
{
	gl_Position = vec4(in_pos, 0.0, 1.0);
	v_draw_angle = in_draw_angle;
}
