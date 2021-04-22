
#version 430 core
layout(location = 0) in vec4 in_pos_xywh;
layout(location = 1) in vec4 in_font_xywh;
out vec4 v_font_xywh;

void main()
{
	gl_Position = in_pos_xywh;
	v_font_xywh = in_font_xywh;
}
