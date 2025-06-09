#ifndef GL_FOG_H
#define GL_FOG_H
#include "zgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void glFogf(GLint pname, GLfloat param);
void glFogi(GLint pname, GLint param);
void glFogfv(GLint pname, const GLfloat* params);
void glFogiv(GLint pname, const GLint* params);

#ifdef __cplusplus
}
#endif

#endif /* GL_FOG_H */
