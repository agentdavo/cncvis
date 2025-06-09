#ifndef GL_SELECTION_H
#define GL_SELECTION_H
#include "zgl.h"

#ifdef __cplusplus
extern "C" {
#endif

GLint glRenderMode(GLint mode);
void glSelectBuffer(GLint size, GLuint* buf);
void glFeedbackBuffer(GLint size, GLenum type, GLfloat* buf);
void glInitNames(void);
void glPushName(GLuint name);
void glPopName(void);
void glLoadName(GLuint name);
void gl_add_select(GLuint zmin, GLuint zmax);
void gl_add_feedback(GLfloat token, GLVertex* v1, GLVertex* v2, GLVertex* v3, GLfloat passthrough_token_value);

#ifdef __cplusplus
}
#endif

#endif /* GL_SELECTION_H */
