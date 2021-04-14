
#version 430 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
in int v_safe_time[];
in float v_angle[];
in vec2 v_tail_pos[];
out vec3 g_color;

#define TAU 6.28318530717
#define BULLET_RADIUS 0.006

void main()
{
	float safe_time_float = float(v_safe_time[0]);
	g_color = vec3(1.0, 1.0, 1.0) -
		vec3(0.0, 0.0, 0.0) * (safe_time_float/20.0);
	/* TODO: The code above doesn't do what it is supposed to do, does it ? */

	float angle = v_angle[0];
	for (int i = 0; i < 3; i++)
	{
		gl_Position = gl_in[0].gl_Position +
			vec4(cos(angle), sin(angle), 0.0, 0.0) * BULLET_RADIUS;
		EmitVertex();
		angle += TAU/3.0;
	}

	gl_Position = vec4(v_tail_pos[0], 0.0, 1.0);
	EmitVertex();

	EndPrimitive();
}
