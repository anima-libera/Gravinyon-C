
/* This file is overwritten at each compilation.
 * Do not modify, see "embedded.h" or "_comp.py" instead. */

/* Content of "shaders/enemies.vert". */
const char* g_enemies_vert = "\n#version 430 core\nlayout(location = 0) in vec2 in_pos;\nlayout(location = 1) in vec3 in_color;\nlayout(location = 2) in float in_angle;\nout vec3 v_color;\nout float v_angle;\n\nvoid main()\n{\n\tgl_Position = vec4(in_pos, 0.0, 1.0);\n\tv_color = in_color;\n\tv_angle = in_angle;\n}\n";

/* Content of "shaders/enemies.geom". */
const char* g_enemies_geom = "\n#version 430 core\nlayout(points) in;\nlayout(triangle_strip, max_vertices = 5) out;\nin vec3 v_color[];\nin float v_angle[];\nout vec3 g_color;\n\n#define TAU 6.28318530717\n#define ENEMY_RADIUS 0.010\n\nvoid main()\n{\n\tg_color = v_color[0];\n\tfloat angle = v_angle[0];\n\tfor (int i = 0; i < 5; i++)\n\t{\n\t\tgl_Position = gl_in[0].gl_Position +\n\t\t\tvec4(cos(angle), sin(angle), 0.0, 0.0) * ENEMY_RADIUS;\n\t\tEmitVertex();\n\t\tangle += TAU/4.0;\n\t}\n\tEndPrimitive();\n}\n";

/* Content of "shaders/enemies.frag". */
const char* g_enemies_frag = "\n#version 430 core\nin vec3 g_color;\nout vec4 out_color;\n\nvoid main()\n{\n\tout_color = vec4(g_color, 1.0);\n}\n";
