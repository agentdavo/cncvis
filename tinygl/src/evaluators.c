#include "gl_evaluators.h"
#include "gl_utils.h"

/* Currently TinyGL does not implement NURBS evaluators. */
void glMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat* points) {
	(void)target;
	(void)u1;
	(void)u2;
	(void)stride;
	(void)order;
	(void)points;
	tgl_warning("glMap1f is not implemented\n");
}

void glEvalCoord1f(GLfloat u) {
	(void)u;
	tgl_warning("glEvalCoord1f is not implemented\n");
}

void glMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat* points) {
	(void)target;
	(void)u1;
	(void)u2;
	(void)ustride;
	(void)uorder;
	(void)v1;
	(void)v2;
	(void)vstride;
	(void)vorder;
	(void)points;
	tgl_warning("glMap2f is not implemented\n");
}

void glEvalCoord2f(GLfloat u, GLfloat v) {
	(void)u;
	(void)v;
	tgl_warning("glEvalCoord2f is not implemented\n");
}
