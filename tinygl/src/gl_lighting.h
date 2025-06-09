#ifndef GL_LIGHTING_H
#define GL_LIGHTING_H

#include "internal.h"
#include "zgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void glMaterialfv(GLint mode, GLint type, GLfloat* v);
void glMaterialf(GLint mode, GLint type, GLfloat v);
void glColorMaterial(GLint mode, GLint type);
void glLightfv(GLint light, GLint type, GLfloat* v);
void glLightf(GLint light, GLint type, GLfloat v);
void glLightModeli(GLint pname, GLint param);
void glLightModelfv(GLint pname, GLfloat* param);

#ifdef __cplusplus
}
#endif

#endif /* GL_LIGHTING_H */
