
#include "dbgmsg.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>

/* Return the name of the source parameter of the debug message callback. */
static char* opengl_dbgmsg_source_name(GLenum source);

/* Return the name of the type parameter of the debug message callback. */
static char* opengl_dbgmsg_type_name(GLenum type);

/* Return the name of the severity parameter of the debug message callback. */
static char* opengl_dbgmsg_severity_name(GLenum type);

/* Debug message callback given to glDebugMessageCallback. Print an error
 * message to stderr. */
static void GLAPIENTRY opengl_dbgmsg_callback(
	GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const void* user_param);

static char* opengl_dbgmsg_source_name(GLenum source)
{
	switch (source)
	{
		case GL_DEBUG_SOURCE_API:
			return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			return "WINDOW_SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			return "SHADER_COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			return "THIRD_PARTY";
		case GL_DEBUG_SOURCE_APPLICATION:
			return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER:
			return "OTHER";
		default:
			return "< NOT A SOURCE >";
	}
}

static char* opengl_dbgmsg_type_name(GLenum type)
{
	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:
			return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY:
			return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE:
			return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER:
			return "MARKER";
		case GL_DEBUG_TYPE_PUSH_GROUP:
			return "PUSH_GROUP";
		case GL_DEBUG_TYPE_POP_GROUP:
			return "POP_GROUP";
		case GL_DEBUG_TYPE_OTHER:
			return "OTHER";
		default:
			return "< NOT A TYPE >";
	}
}

static char* opengl_dbgmsg_severity_name(GLenum type)
{
	switch (type)
	{
		case GL_DEBUG_SEVERITY_HIGH:
			return "HIGH";
		case GL_DEBUG_SEVERITY_MEDIUM:
			return "MEDIUM";
		case GL_DEBUG_SEVERITY_LOW:
			return "LOW";
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			return "NOTIFICATION";
		default:
			return "< NOT A SEVERITY >";
	}
}

static void GLAPIENTRY opengl_dbgmsg_callback(
	GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const void* user_param)
{
	(void)length; (void)user_param;
	#ifndef DEBUG
		/* Filter out non-error debug messages if not in a debug build. */
		if (type != GL_DEBUG_TYPE_ERROR)
		{
			return;
		}
	#endif
	fprintf((type == GL_DEBUG_TYPE_ERROR) ? stderr : stdout,
		"OpenGL dbgmsg (%s severity) %s:%s(%u) %s\"%s\"\x1b[39m\n",
		opengl_dbgmsg_severity_name(severity),
		opengl_dbgmsg_source_name(source),
		opengl_dbgmsg_type_name(type),
		(unsigned int)id,
		(type == GL_DEBUG_TYPE_ERROR) ? "\x1b[31m" : "\x1b[34m",
		message);
}

void enable_opengl_dbgmsg(void)
{
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(opengl_dbgmsg_callback, NULL);
}

void disable_opengl_dbgmsg(void)
{
	glDisable(GL_DEBUG_OUTPUT);
}

void error_sdl2_fail(const char* operation)
{
	fprintf(stderr, "SDL2 error: %s failed: \x1b[31m\"%s\"\x1b[39m\n",
		operation, SDL_GetError());
}
