#ifndef GL_RASTER_H
#define GL_RASTER_H

#include "../include/GL/gl.h"

#ifdef __cplusplus
extern "C" {
#endif

void glRasterPos2f(GLfloat x, GLfloat y);
void glRasterPos3f(GLfloat x, GLfloat y, GLfloat z);
void glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void glRasterPos2fv(GLfloat* v);
void glRasterPos3fv(GLfloat* v);
void glRasterPos4fv(GLfloat* v);
void glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, void* data);
void glPixelZoom(GLfloat x, GLfloat y);

#ifdef __cplusplus
}
#endif

#endif /* GL_RASTER_H */
