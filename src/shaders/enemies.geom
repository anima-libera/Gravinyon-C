
#version 430 core
layout(location = 0) uniform float u_aspect_ratio;
layout(points) in;
layout(triangle_strip, max_vertices = 5) out;
in vec3 v_color[];
in float v_angle[];
out vec3 g_color;

#define TAU 6.28318530717
#define ENEMY_RADIUS 0.012

void main()
{
	g_color = v_color[0];
	float angle = v_angle[0];
	for (int i = 0; i < 5; i++)
	{
		gl_Position = gl_in[0].gl_Position +
			vec4(cos(angle), sin(angle), 0.0, 0.0) * ENEMY_RADIUS;
		gl_Position.y *= u_aspect_ratio;
		EmitVertex();
		angle += TAU/4.0;
	}
	EndPrimitive();
}
