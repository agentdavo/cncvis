#ifndef GL_STATE_H
#define GL_STATE_H

#include "internal.h"
#include "zgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** State management API extracted from gl.h */
void glEnable(GLint cap);
void glDisable(GLint cap);
void glDepthMask(GLint mask);
void glMatrixMode(GLint mode);
void glLoadIdentity(void);
void glLoadMatrixf(const GLfloat* m);
void glMultMatrixf(const GLfloat* m);
void glPushMatrix(void);
void glPopMatrix(void);
void glClear(GLint mask);
void glClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void glClearDepth(GLdouble depth);
void glShadeModel(GLint mode);
void glCullFace(GLint mode);
void glFrontFace(GLint mode);
void glPolygonMode(GLint face, GLint mode);
void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void glTranslatef(GLfloat x, GLfloat y, GLfloat z);
void glScalef(GLfloat x, GLfloat y, GLfloat z);
void glViewport(GLint x, GLint y, GLint width, GLint height);
void glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
void glFrustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat nearv, GLfloat farv);
void glOrtho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat nearv, GLfloat farv);
void glPolygonOffset(GLfloat factor, GLfloat units);
void glHint(GLint target, GLint mode);

#ifdef __cplusplus
}
#endif

#endif /* GL_STATE_H */
