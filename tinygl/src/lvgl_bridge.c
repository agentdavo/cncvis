#include "lvgl_bridge.h"

#if defined(TINYGL_WITH_LVGL)
#include <lvgl.h>
#endif

void tgl_to_lvgl(const PixelQuad* src, void* dst, unsigned pixel_count) {
	const PIXEL* s = (const PIXEL*)src;
#if LV_COLOR_DEPTH == 16
	uint16_t* d = (uint16_t*)dst;
	for (unsigned i = 0; i < pixel_count; ++i) {
		uint32_t v = s[i];
		d[i] = ((v >> 8) & 0xf800) | ((v >> 5) & 0x07e0) | ((v >> 3) & 0x001f);
	}
#elif LV_COLOR_DEPTH == 24
	uint8_t* d = (uint8_t*)dst;
	for (unsigned i = 0; i < pixel_count; ++i) {
		uint32_t v = s[i];
		d[3 * i + 0] = v & 0xff;
		d[3 * i + 1] = (v >> 8) & 0xff;
		d[3 * i + 2] = (v >> 16) & 0xff;
	}
#elif LV_COLOR_DEPTH == 32
	uint32_t* d = (uint32_t*)dst;
	for (unsigned i = 0; i < pixel_count; ++i) {
		uint32_t v = s[i];
		d[i] = 0xff000000u | (v & 0x00ffffffu);
	}
#else
#error "Unsupported LV_COLOR_DEPTH"
#endif
}
