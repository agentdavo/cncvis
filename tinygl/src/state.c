#include "gl_state.h"

/* Implementation moved from api.c */
void glEnable(GLint cap) {
	GLParam p[3];
#include "error_check_no_context.h"
	p[0].op = OP_EnableDisable;
	p[1].i = cap;
	p[2].i = 1;
	gl_add_op(p);
}

void glDisable(GLint cap) {
	GLParam p[3];
#include "error_check_no_context.h"
	p[0].op = OP_EnableDisable;
	p[1].i = cap;
	p[2].i = 0;
	gl_add_op(p);
}

void glDepthMask(GLint i) {
#include "error_check_no_context.h"
	gl_get_context()->zb->depth_write = (i == GL_TRUE);
}

void glMatrixMode(GLint mode) {
	GLParam p[2];
#include "error_check_no_context.h"
	p[0].op = OP_MatrixMode;
	p[1].i = mode;
	gl_add_op(p);
}

void glLoadMatrixf(const GLfloat* m) {
	GLParam p[17];
	GLint i;
#include "error_check_no_context.h"
	p[0].op = OP_LoadMatrix;
	for (i = 0; i < 16; i++)
		p[i + 1].f = m[i];
	gl_add_op(p);
}

void glLoadIdentity(void) {
	GLParam p[1];
#include "error_check_no_context.h"
	p[0].op = OP_LoadIdentity;
	gl_add_op(p);
}

void glMultMatrixf(const GLfloat* m) {
	GLParam p[17];
	GLint i;
#include "error_check_no_context.h"
	p[0].op = OP_MultMatrix;
	for (i = 0; i < 16; i++)
		p[i + 1].f = m[i];
	gl_add_op(p);
}

void glPushMatrix(void) {
	GLParam p[1];
#include "error_check_no_context.h"
	p[0].op = OP_PushMatrix;
	gl_add_op(p);
}

void glPopMatrix(void) {
	GLParam p[1];
#include "error_check_no_context.h"
	p[0].op = OP_PopMatrix;
	gl_add_op(p);
}

void glClear(GLint mask) {
	GLParam p[2];
#include "error_check_no_context.h"
	p[0].op = OP_Clear;
	p[1].i = mask;
	gl_add_op(p);
}

void glClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
	GLParam p[5];
#include "error_check_no_context.h"
	p[0].op = OP_ClearColor;
	p[1].f = r;
	p[2].f = g;
	p[3].f = b;
	p[4].f = a;
	gl_add_op(p);
}

void glClearDepth(GLdouble depth) {
	GLParam p[2];
#include "error_check_no_context.h"
	p[0].op = OP_ClearDepth;
	p[1].f = depth;
	gl_add_op(p);
}

void glShadeModel(GLint mode) {
	GLParam p[2];
#define NEED_CONTEXT
#include "error_check_no_context.h"
#if TGL_FEATURE_ERROR_CHECK == 1
	if (mode != GL_FLAT && mode != GL_SMOOTH)
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else
	if (mode != GL_FLAT && mode != GL_SMOOTH)
		return;
#endif
		p[0].op = OP_ShadeModel;
	p[1].i = mode;
	gl_add_op(p);
}

void glCullFace(GLint mode) {
	GLParam p[2];
#define NEED_CONTEXT
#include "error_check_no_context.h"
#if TGL_FEATURE_ERROR_CHECK == 1
	if (!(mode == GL_BACK || mode == GL_FRONT || mode == GL_FRONT_AND_BACK))
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else
	;
#endif
		p[0].op = OP_CullFace;
	p[1].i = mode;
	gl_add_op(p);
}

void glFrontFace(GLint mode) {
	GLParam p[2];
#define NEED_CONTEXT
#include "error_check_no_context.h"
#if TGL_FEATURE_ERROR_CHECK == 1
	if (!(mode == GL_CCW || mode == GL_CW))
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else
	;
#endif
		mode = (mode != GL_CCW);
	p[0].op = OP_FrontFace;
	p[1].i = mode;
	gl_add_op(p);
}

void glPolygonMode(GLint face, GLint mode) {
	GLParam p[3];
#define NEED_CONTEXT
#include "error_check_no_context.h"
#if TGL_FEATURE_ERROR_CHECK == 1
	if (!((face == GL_BACK || face == GL_FRONT || face == GL_FRONT_AND_BACK) && (mode == GL_POINT || mode == GL_LINE || mode == GL_FILL)))
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#endif
		p[0].op = OP_PolygonMode;
	p[1].i = face;
	p[2].i = mode;
	gl_add_op(p);
}

void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
	GLParam p[5];
#include "error_check_no_context.h"
	p[0].op = OP_Rotate;
	p[1].f = angle;
	p[2].f = x;
	p[3].f = y;
	p[4].f = z;
	gl_add_op(p);
}

void glTranslatef(GLfloat x, GLfloat y, GLfloat z) {
	GLParam p[4];
#include "error_check_no_context.h"
	p[0].op = OP_Translate;
	p[1].f = x;
	p[2].f = y;
	p[3].f = z;
	gl_add_op(p);
}

void glScalef(GLfloat x, GLfloat y, GLfloat z) {
	GLParam p[4];
#include "error_check_no_context.h"
	p[0].op = OP_Scale;
	p[1].f = x;
	p[2].f = y;
	p[3].f = z;
	gl_add_op(p);
}

void glViewport(GLint x, GLint y, GLint width, GLint height) {
	GLParam p[5];
#include "error_check_no_context.h"
	p[0].op = OP_Viewport;
	p[1].i = x;
	p[2].i = y;
	p[3].i = width;
	p[4].i = height;
	gl_add_op(p);
}

void glScissor(GLint x, GLint y, GLsizei width, GLsizei height) {
	GLParam p[5];
#include "error_check_no_context.h"
	p[0].op = OP_Scissor;
	p[1].i = x;
	p[2].i = y;
	p[3].i = width;
	p[4].i = height;
	gl_add_op(p);
}

void glFrustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat nearv, GLfloat farv) {
	GLParam p[7];
#include "error_check_no_context.h"
	p[0].op = OP_Frustum;
	p[1].f = left;
	p[2].f = right;
	p[3].f = bottom;
	p[4].f = top;
	p[5].f = nearv;
	p[6].f = farv;
	gl_add_op(p);
}

void glOrtho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat nearv, GLfloat farv) {
	GLParam p[7];
#include "error_check_no_context.h"
	p[0].op = OP_Ortho;
	p[1].f = left;
	p[2].f = right;
	p[3].f = bottom;
	p[4].f = top;
	p[5].f = nearv;
	p[6].f = farv;
	gl_add_op(p);
}

void glPolygonOffset(GLfloat factor, GLfloat units) {
	GLParam p[3];
#include "error_check_no_context.h"
	p[0].op = OP_PolygonOffset;
	p[1].f = factor;
	p[2].f = units;
	gl_add_op(p);
}

void glHint(GLint target, GLint mode) {
	(void)target;
	(void)mode;
}
