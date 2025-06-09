#ifndef GL_INIT_H
#define GL_INIT_H

#include "internal.h"
#include "zgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize a TinyGL context using the provided ZBuffer. */
void glInit(void* zbuffer);

/** Destroy the current TinyGL context and free resources. */
void glClose(void);

#ifdef __cplusplus
}
#endif

#endif /* GL_INIT_H */
