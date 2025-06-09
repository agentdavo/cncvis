#ifndef GL_EVALUATORS_H
#define GL_EVALUATORS_H
#include "zgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Define a 1D evaluator map. */
void glMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat* points);

/** @brief Evaluate a 1D coordinate. */
void glEvalCoord1f(GLfloat u);

/** @brief Define a 2D evaluator map. */
void glMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat* points);

/** @brief Evaluate a 2D coordinate. */
void glEvalCoord2f(GLfloat u, GLfloat v);

#ifdef __cplusplus
}
#endif

#endif /* GL_EVALUATORS_H */
