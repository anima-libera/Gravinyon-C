
/* This file is overwritten at each compilation.
 * Do not modify, see "embedded.h" or "_comp.py" instead. */

/* Content of "shaders/ship.vert". */
const char* g_ship_vert = "\n#version 430 core\nlayout(location = 0) in vec2 in_pos;\nlayout(location = 1) in float in_draw_angle;\nout float v_draw_angle;\n\nvoid main()\n{\n\tgl_Position = vec4(in_pos, 0.0, 1.0);\n\tv_draw_angle = in_draw_angle;\n}\n";

/* Content of "shaders/ship.geom". */
const char* g_ship_geom = "\n#version 430 core\nlayout(points) in;\nlayout(triangle_strip, max_vertices = 5) out;\nin float v_draw_angle[];\nout vec3 g_color;\n\n#define TAU 6.28318530717\n#define SHIP_RADIUS 0.032\n\nvoid main()\n{\n\tg_color = vec3(1.0, 1.0, 1.0);\n\tfloat angle = v_draw_angle[0];\n\n\tgl_Position = gl_in[0].gl_Position +\n\t\tvec4(cos(angle), sin(angle), 0.0, 0.0) * SHIP_RADIUS;\n\tEmitVertex();\n\n\tangle += TAU * 3.0/8.0;\n\tgl_Position = gl_in[0].gl_Position +\n\t\tvec4(cos(angle), sin(angle), 0.0, 0.0) * SHIP_RADIUS;\n\tEmitVertex();\n\n\tgl_Position = gl_in[0].gl_Position;\n\tEmitVertex();\n\n\tangle += TAU / 4.0 + TAU * 3.0/8.0;\n\tgl_Position = gl_in[0].gl_Position +\n\t\tvec4(cos(angle), sin(angle), 0.0, 0.0) * SHIP_RADIUS;\n\tEmitVertex();\n\n\tangle -= TAU * 3.0/8.0;\n\tgl_Position = gl_in[0].gl_Position +\n\t\tvec4(cos(angle), sin(angle), 0.0, 0.0) * SHIP_RADIUS;\n\tEmitVertex();\n\n\tEndPrimitive();\n}\n";

/* Content of "shaders/ship.frag". */
const char* g_ship_frag = "\n#version 430 core\nin vec3 g_color;\nout vec4 out_color;\n\nvoid main()\n{\n\tout_color = vec4(g_color, 1.0);\n}\n";

/* Content of "shaders/enemies.vert". */
const char* g_enemies_vert = "\n#version 430 core\nlayout(location = 0) in vec2 in_pos;\nlayout(location = 1) in vec3 in_color;\nlayout(location = 2) in float in_angle;\nout vec3 v_color;\nout float v_angle;\n\nvoid main()\n{\n\tgl_Position = vec4(in_pos, 0.0, 1.0);\n\tv_color = in_color;\n\tv_angle = in_angle;\n}\n";

/* Content of "shaders/enemies.geom". */
const char* g_enemies_geom = "\n#version 430 core\nlayout(points) in;\nlayout(triangle_strip, max_vertices = 5) out;\nin vec3 v_color[];\nin float v_angle[];\nout vec3 g_color;\n\n#define TAU 6.28318530717\n#define ENEMY_RADIUS 0.020\n\nvoid main()\n{\n\tg_color = v_color[0];\n\tfloat angle = v_angle[0];\n\tfor (int i = 0; i < 5; i++)\n\t{\n\t\tgl_Position = gl_in[0].gl_Position +\n\t\t\tvec4(cos(angle), sin(angle), 0.0, 0.0) * ENEMY_RADIUS;\n\t\tEmitVertex();\n\t\tangle += TAU/4.0;\n\t}\n\tEndPrimitive();\n}\n";

/* Content of "shaders/enemies.frag". */
const char* g_enemies_frag = "\n#version 430 core\nin vec3 g_color;\nout vec4 out_color;\n\nvoid main()\n{\n\tout_color = vec4(g_color, 1.0);\n}\n";

/* Content of "shaders/bullets.vert". */
const char* g_bullets_vert = "\n#version 430 core\nlayout(location = 0) in vec2 in_pos;\nlayout(location = 1) in int in_safe_time;\nlayout(location = 2) in float in_angle;\nlayout(location = 3) in vec2 in_tail_pos;\nout int v_safe_time;\nout float v_angle;\nout vec2 v_tail_pos;\n\nvoid main()\n{\n\tgl_Position = vec4(in_pos, 0.0, 1.0);\n\tv_safe_time = in_safe_time;\n\tv_angle = in_angle;\n\tv_tail_pos = in_tail_pos;\n}\n";

/* Content of "shaders/bullets.geom". */
const char* g_bullets_geom = "\n#version 430 core\nlayout(points) in;\nlayout(triangle_strip, max_vertices = 4) out;\nin int v_safe_time[];\nin float v_angle[];\nin vec2 v_tail_pos[];\nout vec3 g_color;\n\n#define TAU 6.28318530717\n#define BULLET_RADIUS 0.006\n\nvoid main()\n{\n\tfloat safe_time_float = float(v_safe_time[0]);\n\tg_color = vec3(1.0, 0.0, 0.0) +\n\t\tvec3(0.0, 1.0, 1.0) * (safe_time_float/20.0);\n\t/* TODO: The code above doesn't do what it is supposed to do, does it ? */\n\n\tfloat angle = v_angle[0];\n\tfor (int i = 0; i < 3; i++)\n\t{\n\t\tgl_Position = gl_in[0].gl_Position +\n\t\t\tvec4(cos(angle), sin(angle), 0.0, 0.0) * BULLET_RADIUS;\n\t\tEmitVertex();\n\t\tangle += TAU/3.0;\n\t}\n\n\tgl_Position = vec4(v_tail_pos[0], 0.0, 1.0);\n\tEmitVertex();\n\n\tEndPrimitive();\n}\n";

/* Content of "shaders/bullets.frag". */
const char* g_bullets_frag = "\n#version 430 core\nin vec3 g_color;\nout vec4 out_color;\n\nvoid main()\n{\n\tout_color = vec4(g_color, 1.0);\n}\n";

/* Content of "shaders/parts.vert". */
const char* g_parts_vert = "\n#version 430 core\nlayout(location = 0) in vec2 in_pos;\nlayout(location = 1) in vec3 in_color;\nlayout(location = 2) in float in_angle;\nlayout(location = 3) in float in_radius;\nout vec3 v_color;\nout float v_angle;\nout float v_radius;\n\nvoid main()\n{\n\tgl_Position = vec4(in_pos, 0.0, 1.0);\n\tv_color = in_color;\n\tv_angle = in_angle;\n\tv_radius = in_radius;\n}\n";

/* Content of "shaders/parts.geom". */
const char* g_parts_geom = "\n#version 430 core\nlayout(points) in;\nlayout(triangle_strip, max_vertices = 3) out;\nin vec3 v_color[];\nin float v_angle[];\nin float v_radius[];\nout vec3 g_color;\n\n#define TAU 6.28318530717\n\nvoid main()\n{\n\tg_color = v_color[0];\n\n\tfloat angle = v_angle[0];\n\tfor (int i = 0; i < 3; i++)\n\t{\n\t\tgl_Position = gl_in[0].gl_Position +\n\t\t\tvec4(cos(angle), sin(angle), 0.0, 0.0) * v_radius[0];\n\t\tEmitVertex();\n\t\tangle += TAU/3.0;\n\t}\n\n\tEndPrimitive();\n}\n";

/* Content of "shaders/parts.frag". */
const char* g_parts_frag = "\n#version 430 core\nin vec3 g_color;\nout vec4 out_color;\n\nvoid main()\n{\n\tout_color = vec4(g_color, 1.0);\n}\n";
