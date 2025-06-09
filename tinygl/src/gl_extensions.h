#ifndef GL_EXTENSIONS_H
#define GL_EXTENSIONS_H
#include "zgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void glTextSize(GLTEXTSIZE mode);
void glDrawText(const GLubyte* text, GLint x, GLint y, GLuint pixel);
void glPlotPixel(GLint x, GLint y, GLuint pixel);
void glPostProcess(GLuint (*postprocess)(GLint x, GLint y, GLuint pixel, GLushort z));

#ifdef __cplusplus
}
#endif

#endif /* GL_EXTENSIONS_H */
