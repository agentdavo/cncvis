#ifndef GL_UTILS_H
#define GL_UTILS_H

#include "internal.h"
#include "zgl.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Message helpers */
void tgl_warning(const char* format, ...);
void tgl_trace(const char* format, ...);
void tgl_fixme(const char* format, ...);
void gl_fatal_error(char* format, ...);

/* GL utility functions */
const GLubyte* glGetString(GLenum name);
GLenum glGetError(void);
void glFlush(void);
void glDebug(GLint mode);

#ifdef __cplusplus
}
#endif

#endif /* GL_UTILS_H */
