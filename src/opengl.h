
#ifndef GRAVINYON_HEADER_OPENGL_
#define GRAVINYON_HEADER_OPENGL_

#ifdef USE_GLEW

	#include <GL/glew.h>

#else

	#define GL_GLEXT_PROTOTYPES
	#include <GL/gl.h>
	#include <GL/glext.h>
	#include <GL/glx.h>

#endif

#endif /* GRAVINYON_HEADER_OPENGL_ */