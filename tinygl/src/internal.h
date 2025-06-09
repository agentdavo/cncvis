#ifndef INTERNAL_H
#define INTERNAL_H

#include "zgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieve the current GL context.
 */
static inline GLContext* get_current_context(void) { return &gl_ctx; }

/**
 * @brief Set the last error code in the current context.
 */
static inline void set_error(GLenum error) {
#if TGL_FEATURE_ERROR_CHECK == 1
	get_current_context()->error_flag = error;
#else
	(void)error;
#endif
}

/* Memory helpers */
void* gl_malloc(GLint size);
void* gl_zalloc(GLint size);
void gl_free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif /* INTERNAL_H */
