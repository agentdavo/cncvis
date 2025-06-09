#ifndef TINYGL_EXT_H
#define TINYGL_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

/* TinyGL specific extensions */
void glSetEnableSpecular(GLint s);
void *glGetTexturePixmap(GLint text, GLint level, GLint *xsize, GLint *ysize);
void glDrawText(const GLubyte *text, GLint x, GLint y, GLuint pixel);
void glTextSize(GLTEXTSIZE mode);
void glPlotPixel(GLint x, GLint y, GLuint pixel);

#ifdef __cplusplus
}
#endif

#endif /* TINYGL_EXT_H */
