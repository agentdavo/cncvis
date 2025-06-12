#include "../include/zbuffer.h"
#include "zgl.h"
#include <stdlib.h>

#define ZCMP(z, zpix) (!(zbdt) || ZB_depth_test(zb, z, zpix))

/* TODO: Implement point size. */
/* TODO: Implement blending for lines and points. */

void ZB_plot(ZBuffer* zb, ZBufferPoint* p) {

	GLint zz, y, x;
	GLContext* c = gl_get_context();
	GLubyte zbdw = zb->depth_write;
	GLubyte zbdt = zb->depth_test;
	GLfloat zbps = zb->pointsize;
	TGL_BLEND_VARS
	zz = p->z >> ZB_POINT_Z_FRAC_BITS;

	if (zbps == 1) {
		GLushort* pz;
		PIXEL* pp;
		if (c->scissor_enabled &&
			(p->x < c->scissor_x || p->x >= c->scissor_x + c->scissor_width || p->y < c->scissor_y || p->y >= c->scissor_y + c->scissor_height))
			return;
		pz = zb->zbuf + (p->y * zb->xsize + p->x);
		pp = (PIXEL*)((GLbyte*)zb->pbuf + zb->linesize * p->y + p->x * PSZB);

		if (ZCMP(zz, *pz)) {
#if TGL_FEATURE_BLEND == 1
			if (!zb->enable_blend)
				*pp = RGB_TO_PIXEL(p->r, p->g, p->b);
			else
				TGL_BLEND_FUNC_RGB(p->r, p->g, p->b, (*pp))
#else
			*pp = RGB_TO_PIXEL(p->r, p->g, p->b);
#endif
			if (zbdw)
				*pz = zz;
		}
	} else {
		PIXEL col = RGB_TO_PIXEL(p->r, p->g, p->b);
		GLfloat hzbps = zbps / 2.0f;
		GLint bx = (GLfloat)p->x - hzbps;
		GLint ex = (GLfloat)p->x + hzbps;
		GLint by = (GLfloat)p->y - hzbps;
		GLint ey = (GLfloat)p->y + hzbps;
		if (c->scissor_enabled) {
			GLint sx = c->scissor_x;
			GLint sy = c->scissor_y;
			GLint sw = c->scissor_width;
			GLint sh = c->scissor_height;
			if (bx < sx)
				bx = sx;
			if (by < sy)
				by = sy;
			if (ex > sx + sw)
				ex = sx + sw;
			if (ey > sy + sh)
				ey = sy + sh;
		} else {
			if (bx < 0)
				bx = 0;
			if (by < 0)
				by = 0;
			if (ex > zb->xsize)
				ex = zb->xsize;
			if (ey > zb->ysize)
				ey = zb->ysize;
		}
		for (y = by; y < ey; y++)
			for (x = bx; x < ex; x++) {
				GLushort* pz = zb->zbuf + (y * zb->xsize + x);
				PIXEL* pp = (PIXEL*)((GLbyte*)zb->pbuf + zb->linesize * y + x * PSZB);

				if (ZCMP(zz, *pz)) {
#if TGL_FEATURE_BLEND == 1
					if (!zb->enable_blend)
						*pp = col;
					else
						TGL_BLEND_FUNC_RGB(p->r, p->g, p->b, (*pp))
#else
					*pp = col;
#endif
					if (zbdw)
						*pz = zz;
				}
			}
	}
}

#define INTERP_Z
static void ZB_line_flat_z(ZBuffer* zb, ZBufferPoint* p1, ZBufferPoint* p2, GLint color) {

	GLubyte zbdt = zb->depth_test;
	GLubyte zbdw = zb->depth_write;
#include "zline.h"
}

/* line with color GLinterpolation */
#define INTERP_Z
#define INTERP_RGB
static void ZB_line_interp_z(ZBuffer* zb, ZBufferPoint* p1, ZBufferPoint* p2) {

	GLubyte zbdt = zb->depth_test;
	GLubyte zbdw = zb->depth_write;
#include "zline.h"
}

/* no Z GLinterpolation */

static void ZB_line_flat(ZBuffer* zb, ZBufferPoint* p1, ZBufferPoint* p2, GLint color) {

#include "zline.h"
}

#define INTERP_RGB
static void ZB_line_interp(ZBuffer* zb, ZBufferPoint* p1, ZBufferPoint* p2) {

#include "zline.h"
}

void ZB_line_z(ZBuffer* zb, ZBufferPoint* p1, ZBufferPoint* p2) {
	GLint color1, color2;

	color1 = RGB_TO_PIXEL(p1->r, p1->g, p1->b);
	color2 = RGB_TO_PIXEL(p2->r, p2->g, p2->b);

	/* choose if the line should have its color GLinterpolated or not */
	if (color1 == color2) {
		ZB_line_flat_z(zb, p1, p2, color1);
	} else {
		ZB_line_interp_z(zb, p1, p2);
	}
}

void ZB_line(ZBuffer* zb, ZBufferPoint* p1, ZBufferPoint* p2) {
	GLint color1, color2;

	color1 = RGB_TO_PIXEL(p1->r, p1->g, p1->b);
	color2 = RGB_TO_PIXEL(p2->r, p2->g, p2->b);

	/* choose if the line should have its color GLinterpolated or not */
	if (color1 == color2) {
		ZB_line_flat(zb, p1, p2, color1);
	} else {
		ZB_line_interp(zb, p1, p2);
	}
}
