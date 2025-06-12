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
void glGetIntegerv(GLint pname, GLint* params);
void glGetFloatv(GLint pname, GLfloat* v);
void glPixelStorei(GLint pname, GLint param);
void glPixelStoref(GLint pname, GLfloat param);
void glFlush(void);
void glDebug(GLint mode);

extern int tgl_profile_enabled;
void tgl_enable_profiling(int enable);
void tgl_profile_report(void);
void tgl_profile_call(int op, void (*func)(GLParam*), GLParam* p);

#ifdef __cplusplus
}
#endif

#endif /* GL_UTILS_H */
