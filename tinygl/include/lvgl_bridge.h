#ifndef LVGL_BRIDGE_H
#define LVGL_BRIDGE_H

#include "zbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Convert TinyGL's PixelQuad buffer (ARGB8888) to LVGL's native color format.
 * @param src         Source PixelQuad buffer
 * @param dst         Destination buffer (lv_color_t array)
 * @param pixel_count Number of pixels to convert
 */
void tgl_to_lvgl(const PixelQuad *src, void *dst, unsigned pixel_count);

#ifdef __cplusplus
}
#endif

#endif /* LVGL_BRIDGE_H */
