
#version 430 core
layout(points) in;
layout(triangle_strip, max_vertices = 5) out;
in float v_draw_angle[];
out vec3 g_color;

#define TAU 6.28318530717
#define SHIP_RADIUS 0.023

void main()
{
	g_color = vec3(1.0, 1.0, 1.0);
	float angle = v_draw_angle[0];
	for (int i = 0; i < 5; i++)
	{
		gl_Position = gl_in[0].gl_Position +
			vec4(cos(angle), sin(angle), 0.0, 0.0) * SHIP_RADIUS;
		EmitVertex();
		angle += TAU/4.0;
	}
	EndPrimitive();
}
