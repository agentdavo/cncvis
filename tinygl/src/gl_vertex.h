#ifndef GL_VERTEX_H
#define GL_VERTEX_H

#include "internal.h"
#include "zgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Vertex specification and drawing API */
void glBegin(GLint mode);
void glEnd(void);
void glVertex2f(GLfloat x, GLfloat y);
void glVertex3f(GLfloat x, GLfloat y, GLfloat z);
void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void glVertex3fv(GLfloat* v);
void glNormal3f(GLfloat x, GLfloat y, GLfloat z);
void glNormal3fv(GLfloat* v);
void glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void glColor4fv(GLfloat* v);
void glColor3f(GLfloat r, GLfloat g, GLfloat b);
void glColor3fv(GLfloat* v);
void glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
void glTexCoord2f(GLfloat s, GLfloat t);
void glTexCoord2fv(GLfloat* v);
void glEdgeFlag(GLint flag);
void glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

#ifdef __cplusplus
}
#endif

#endif /* GL_VERTEX_H */
