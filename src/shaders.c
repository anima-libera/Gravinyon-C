
#include "shaders.h"
#include "utils.h"
#include "embedded.h"
#include <stdio.h>
#include <GL/glew.h>

/* Wrapper around glCompileShader that prints out the error message to stderr
 * if the shader compilation fails. The dbg_info parameter can be NULL, but
 * should actually provide information that describe the shader type and shader
 * source given to glShaderSource when creating the shader (like a file path)
 * to be added in the error message if any.
 * Returns -1 if anything goes wrong, returns 0 if everything goes well. */
static int compile_shader(GLuint shader_id, const char* dbg_info)
{
	glCompileShader(shader_id);
	GLint status = 0;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length = 0;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
		GLchar* message = xmalloc(length * sizeof(GLchar));
		glGetShaderInfoLog(shader_id, length, &length, message);
		fprintf(stderr,
			"OpenGL shader compilation error (%s): \x1b[31m\"%s\"\x1b[39m\n",
			(dbg_info != NULL) ? dbg_info : "\0", (const char*)message);
		return -1;
	}
	return 0;
}

/* Creates an OpenGL shader object with the given GLSL source code, then
 * compiles it and returns its id. */
static GLuint shader_src_compile(const char* shader_src, GLuint shader_type)
{
	GLuint shader_id = glCreateShader(shader_type);
	glShaderSource(shader_id, 1, (const GLchar* const*)&shader_src, NULL);
	compile_shader(shader_id, NULL);
	return shader_id;
}

/* Creates an OpenGL shader program object and attaches the given shaders to
 * it, then links it and returns its id.
 * The src_geom parameter can be NULL as a geometry shader is optional in the
 * OpenGL rendering pipeline. */
static GLuint shprog_build(
	const char* src_vert, const char* src_geom, const char* src_frag)
{
	GLuint shprog_id = glCreateProgram();
	GLuint shader_vert_id = shader_src_compile(src_vert, GL_VERTEX_SHADER);
	glAttachShader(shprog_id, shader_vert_id);
	GLuint shader_geom_id;
	if (src_geom != NULL)
	{
		shader_geom_id = shader_src_compile(src_geom, GL_GEOMETRY_SHADER);
		glAttachShader(shprog_id, shader_geom_id);
	}
	GLuint shader_frag_id = shader_src_compile(src_frag, GL_FRAGMENT_SHADER);
	glAttachShader(shprog_id, shader_frag_id);
	glLinkProgram(shprog_id);
	glDetachShader(shprog_id, shader_vert_id);
	glDeleteShader(shader_vert_id);
	if (src_geom != NULL)
	{
		glDetachShader(shprog_id, shader_geom_id);
		glDeleteShader(shader_geom_id);
	}
	glDetachShader(shprog_id, shader_frag_id);
	glDeleteShader(shader_frag_id);
	return shprog_id;
}

/* Creates an OpenGL shader program object and attaches the given compute
 * shader to it, then links it and returns its id. */
static GLuint shprog_compute_build(const char* src_comp)
{
	GLuint shprog_id = glCreateProgram();
	GLuint shader_comp_id = shader_src_compile(src_comp, GL_COMPUTE_SHADER);
	glAttachShader(shprog_id, shader_comp_id);
	glLinkProgram(shprog_id);
	glDetachShader(shprog_id, shader_comp_id);
	glDeleteShader(shader_comp_id);
	return shprog_id;
}

GLuint g_shprog_draw_ship = 0;
GLuint g_shprog_draw_enemies = 0;
GLuint g_shprog_draw_bullets = 0;
GLuint g_shprog_draw_parts = 0;

void shprog_build_all(void)
{
	g_shprog_draw_ship = shprog_build(
		g_shader_ship_vert,
		g_shader_ship_geom,
		g_shader_ship_frag);
	g_shprog_draw_enemies = shprog_build(
		g_shader_enemies_vert,
		g_shader_enemies_geom,
		g_shader_enemies_frag);
	g_shprog_draw_bullets = shprog_build(
		g_shader_bullets_vert,
		g_shader_bullets_geom,
		g_shader_bullets_frag);
	g_shprog_draw_parts = shprog_build(
		g_shader_parts_vert,
		g_shader_parts_geom,
		g_shader_parts_frag);
}

void shprog_destroy_all(void)
{
	glDeleteProgram(g_shprog_draw_ship);
	g_shprog_draw_ship = 0;
	glDeleteProgram(g_shprog_draw_enemies);
	g_shprog_draw_enemies = 0;
	glDeleteProgram(g_shprog_draw_bullets);
	g_shprog_draw_bullets = 0;
	glDeleteProgram(g_shprog_draw_parts);
	g_shprog_draw_parts = 0;
}
