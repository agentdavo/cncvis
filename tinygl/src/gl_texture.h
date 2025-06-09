#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#include "../include/GL/gl.h"

#ifdef __cplusplus
extern "C" {
#endif

void glGenTextures(GLint n, GLuint* textures);
void glDeleteTextures(GLint n, const GLuint* textures);
void glBindTexture(GLint target, GLint texture);
void glTexImage2D(GLint target, GLint level, GLint components, GLint width, GLint height, GLint border, GLint format, GLint type, void* pixels);
void glTexImage1D(GLint target, GLint level, GLint components, GLint width, GLint border, GLint format, GLint type, void* pixels);
void glTexEnvi(GLint target, GLint pname, GLint param);
void glTexParameteri(GLint target, GLint pname, GLint param);

#ifdef __cplusplus
}
#endif

#endif /* GL_TEXTURE_H */
