
#version 430 core
layout(location = 0) uniform float u_aspect_ratio;
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
in vec4 v_font_xywh[];
out vec2 g_font_pos;

void main()
{
	float x = gl_in[0].gl_Position.x;
	float y = gl_in[0].gl_Position.y;
	float w = gl_in[0].gl_Position.z;
	float h = gl_in[0].gl_Position.w;
	float f_x = v_font_xywh[0].x;
	float f_y = v_font_xywh[0].y;
	float f_w = v_font_xywh[0].z;
	float f_h = v_font_xywh[0].w;

	gl_Position = vec4(x, y * u_aspect_ratio, 0.0, 1.0);
	g_font_pos = vec2(f_x, f_y);
	EmitVertex();

	gl_Position = vec4(x + w, y * u_aspect_ratio, 0.0, 1.0);
	g_font_pos = vec2(f_x + f_w, f_y);
	EmitVertex();

	gl_Position = vec4(x, (y + h) * u_aspect_ratio, 0.0, 1.0);
	g_font_pos = vec2(f_x, f_y + f_h);
	EmitVertex();

	gl_Position = vec4(x + w, (y + h) * u_aspect_ratio, 0.0, 1.0);
	g_font_pos = vec2(f_x + f_w, f_y + f_h);
	EmitVertex();

	EndPrimitive();
}
