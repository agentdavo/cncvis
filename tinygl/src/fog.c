#include "gl_fog.h"
#include "gl_utils.h"

void glFogf(GLint pname, GLfloat param) {
	GLContext* c = gl_get_context();
	switch (pname) {
	case GL_FOG_MODE:
		c->fog_mode = (GLint)param;
		break;
	case GL_FOG_DENSITY:
		c->fog_density = param;
		break;
	case GL_FOG_START:
		c->fog_start = param;
		break;
	case GL_FOG_END:
		c->fog_end = param;
		break;
	default:
		break;
	}
}

void glFogi(GLint pname, GLint param) { glFogf(pname, (GLfloat)param); }

void glFogfv(GLint pname, const GLfloat* params) {
	GLContext* c = gl_get_context();
	switch (pname) {
	case GL_FOG_COLOR:
		for (int i = 0; i < 4; ++i)
			c->fog_color[i] = params[i];
		break;
	default:
		glFogf(pname, params[0]);
		break;
	}
}

void glFogiv(GLint pname, const GLint* params) {
	GLfloat tmp[4];
	switch (pname) {
	case GL_FOG_COLOR:
		for (int i = 0; i < 4; ++i)
			tmp[i] = (GLfloat)params[i] / 255.0f;
		glFogfv(pname, tmp);
		break;
	default:
		glFogf(pname, (GLfloat)params[0]);
		break;
	}
}
