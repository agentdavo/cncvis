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
void glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type,
				  const void* pixels);
void glTexEnvi(GLint target, GLint pname, GLint param);
void glTexEnvf(GLint target, GLint pname, GLfloat param);
void glTexEnvfv(GLint target, GLint pname, const GLfloat* params);
void glTexParameteri(GLint target, GLint pname, GLint param);
void glTexParameterf(GLint target, GLint pname, GLfloat param);
void glTexParameterfv(GLint target, GLint pname, const GLfloat* params);
void glTexSubImage1D(GLint target, GLint level, GLint xoffset, GLsizei width, GLint format, GLint type, const void* pixels);
void glTexSubImage2D(GLint target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLint format, GLint type, const void* pixels);
void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format,
					 GLenum type, const void* pixels);
void glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);

#ifdef __cplusplus
}
#endif

#endif /* GL_TEXTURE_H */
