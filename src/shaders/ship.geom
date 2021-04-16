
#version 430 core
layout(location = 0) uniform float u_aspect_ratio;
layout(points) in;
layout(triangle_strip, max_vertices = 5) out;
in float v_draw_angle[];
out vec3 g_color;

#define TAU 6.28318530717
#define SHIP_RADIUS 0.018

void main()
{
	g_color = vec3(1.0, 1.0, 1.0);
	float angle = v_draw_angle[0];

	gl_Position = gl_in[0].gl_Position +
		vec4(cos(angle), sin(angle), 0.0, 0.0) * SHIP_RADIUS;
	gl_Position.y *= u_aspect_ratio;
	EmitVertex();

	angle += TAU * 3.0/8.0;
	gl_Position = gl_in[0].gl_Position +
		vec4(cos(angle), sin(angle), 0.0, 0.0) * SHIP_RADIUS;
	gl_Position.y *= u_aspect_ratio;
	EmitVertex();

	gl_Position = gl_in[0].gl_Position;
	gl_Position.y *= u_aspect_ratio;
	EmitVertex();

	angle += TAU / 4.0 + TAU * 3.0/8.0;
	gl_Position = gl_in[0].gl_Position +
		vec4(cos(angle), sin(angle), 0.0, 0.0) * SHIP_RADIUS;
	gl_Position.y *= u_aspect_ratio;
	EmitVertex();

	angle -= TAU * 3.0/8.0;
	gl_Position = gl_in[0].gl_Position +
		vec4(cos(angle), sin(angle), 0.0, 0.0) * SHIP_RADIUS;
	gl_Position.y *= u_aspect_ratio;
	EmitVertex();

	EndPrimitive();
}
