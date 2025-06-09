#include "../include/zbuffer.h"
#include "msghandling.h"
#include "zgl.h"
#include <math.h>
#include <stdalign.h>
#include <stdbool.h>
#include <threads.h>

#if TGL_FEATURE_MULTITHREADED_ZB_TRIANGLE == 1
#include "lockstepthread.h"
#endif

/* simple barycentric rasterizer for 32-bit PixelQuad frame buffer */

typedef struct {
	ZBuffer* zb;
	ZBufferPoint* p0;
	ZBufferPoint* p1;
	ZBufferPoint* p2;
	int y_start;
	int y_end;
	int mode; /*0 flat,1 smooth,2 textured*/
	PIXEL flat_color;
} RasterJob;

#if TGL_FEATURE_MULTITHREADED_ZB_TRIANGLE == 1
#ifndef NUM_RASTER_THREADS
#define NUM_RASTER_THREADS 4
#endif
static c11_lsthread raster_threads[NUM_RASTER_THREADS];
static RasterJob raster_jobs[NUM_RASTER_THREADS];
#endif

static inline float edgef(float ax, float ay, float bx, float by, float cx, float cy) { return (cx - ax) * (by - ay) - (cy - ay) * (bx - ax); }

static inline PIXEL sample_tex(PIXEL* tex, int s, int t) { return *(PIXEL*)((unsigned char*)tex + ST_TO_TEXTURE_BYTE_OFFSET(s, t)); }

static void raster_job(void* arg) {
	RasterJob* job = arg;
	ZBuffer* zb = job->zb;
	TGL_BLEND_VARS
	ZBufferPoint *p0 = job->p0, *p1 = job->p1, *p2 = job->p2;

	float x0 = p0->x, y0 = p0->y, z0 = p0->z;
	float x1 = p1->x, y1 = p1->y, z1 = p1->z;
	float x2 = p2->x, y2 = p2->y, z2 = p2->z;

	float area = edgef(x0, y0, x1, y1, x2, y2);
	if (area == 0.0f)
		return;
	float inv_area = 1.0f / area;

	int xmin = (int)fmaxf(floorf(fminf(x0, fminf(x1, x2))), 0);
	int xmax = (int)fminf(ceilf(fmaxf(x0, fmaxf(x1, x2))), zb->xsize - 1);
	int ymin = (int)fmaxf(floorf(fminf(y0, fminf(y1, y2))), job->y_start);
	int ymax = (int)fminf(ceilf(fmaxf(y0, fmaxf(y1, y2))), job->y_end - 1);

	float s0 = p0->s, t0 = p0->t;
	float s1 = p1->s, t1 = p1->t;
	float s2 = p2->s, t2 = p2->t;
	float r0 = p0->r, g0 = p0->g, b0 = p0->b;
	float r1 = p1->r, g1 = p1->g, b1 = p1->b;
	float r2 = p2->r, g2 = p2->g, b2 = p2->b;

	PIXEL* tex = zb->current_texture;

	for (int y = ymin; y <= ymax; y++) {
		for (int x = xmin; x <= xmax; x++) {
			float px = x + 0.5f, py = y + 0.5f;
			float w0 = edgef(x1, y1, x2, y2, px, py);
			float w1 = edgef(x2, y2, x0, y0, px, py);
			float w2 = edgef(x0, y0, x1, y1, px, py);
			if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
				w0 *= inv_area;
				w1 *= inv_area;
				w2 *= inv_area;
				float iz = w0 / z0 + w1 / z1 + w2 / z2;
				float z = 1.0f / iz;
				unsigned int zz = (unsigned int)(z / (1 << ZB_POINT_Z_FRAC_BITS));
				GLushort* pz = zb->zbuf + y * zb->xsize + x;
				if (!zb->depth_test || zz >= *pz) {
					PIXEL* pp = (PIXEL*)((unsigned char*)zb->pbuf + y * zb->linesize + x * PSZB);
					PIXEL col;
					if (job->mode == 0) {
						col = job->flat_color;
					} else if (job->mode == 1) {
						float r = (w0 * r0 / z0 + w1 * r1 / z1 + w2 * r2 / z2) * z;
						float g = (w0 * g0 / z0 + w1 * g1 / z1 + w2 * g2 / z2) * z;
						float b = (w0 * b0 / z0 + w1 * b1 / z1 + w2 * b2 / z2) * z;
						col = RGB_TO_PIXEL((int)r, (int)g, (int)b);
					} else {
						float s = (w0 * s0 / z0 + w1 * s1 / z1 + w2 * s2 / z2) * z;
						float t = (w0 * t0 / z0 + w1 * t1 / z1 + w2 * t2 / z2) * z;
						PIXEL tpix = sample_tex(tex, (int)s, (int)t);
#if TGL_FEATURE_LIT_TEXTURES == 1
						float r = (w0 * r0 / z0 + w1 * r1 / z1 + w2 * r2 / z2) * z;
						float g = (w0 * g0 / z0 + w1 * g1 / z1 + w2 * g2 / z2) * z;
						float b = (w0 * b0 / z0 + w1 * b1 / z1 + w2 * b2 / z2) * z;
						col = RGB_MIX_FUNC((int)r, (int)g, (int)b, tpix);
#else
						col = tpix;
#endif
					}
					if (zb->enable_blend) {
						TGL_BLEND_FUNC(col, *pp);
					} else {
						*pp = col;
					}
					if (zb->depth_write)
						*pz = zz;
				}
			}
		}
	}
}

#if TGL_FEATURE_MULTITHREADED_ZB_TRIANGLE == 1
static void start_raster_jobs(RasterJob base) {
	int h = (base.zb->ysize + NUM_RASTER_THREADS - 1) / NUM_RASTER_THREADS;
	for (int i = 0; i < NUM_RASTER_THREADS; i++) {
		raster_jobs[i] = base;
		raster_jobs[i].y_start = i * h;
		raster_jobs[i].y_end = (i == NUM_RASTER_THREADS - 1) ? base.zb->ysize : (i + 1) * h;
		step_c11_lsthread(&raster_threads[i]);
	}
	for (int i = 0; i < NUM_RASTER_THREADS; i++)
		lock_c11_lsthread(&raster_threads[i]);
}

void init_raster_threads(void) {
	for (int i = 0; i < NUM_RASTER_THREADS; i++) {
		init_c11_lsthread(&raster_threads[i]);
		raster_threads[i].execute = raster_job;
		raster_threads[i].argument = &raster_jobs[i];
		start_c11_lsthread(&raster_threads[i]);
	}
}

void end_raster_threads(void) {
	for (int i = 0; i < NUM_RASTER_THREADS; i++) {
		kill_c11_lsthread(&raster_threads[i]);
		destroy_c11_lsthread(&raster_threads[i]);
	}
}
#endif

void ZB_setTexture(ZBuffer* zb, PIXEL* texture) { zb->current_texture = texture; }

static void draw_triangle(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2, int mode, PIXEL flat) {
#if TGL_FEATURE_MULTITHREADED_ZB_TRIANGLE == 1
	if (tgl_threads_enabled && zb->ysize > 64) {
		RasterJob base = {zb, p0, p1, p2, 0, zb->ysize, mode, flat};
		start_raster_jobs(base);
		return;
	}
#endif
	RasterJob job = {zb, p0, p1, p2, 0, zb->ysize, mode, flat};
	raster_job(&job);
}

void ZB_fillTriangleFlat(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2) {
	PIXEL col = RGB_TO_PIXEL(p2->r, p2->g, p2->b);
	draw_triangle(zb, p0, p1, p2, 0, col);
}

void ZB_fillTriangleFlatNOBLEND(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2) {
	int saved = zb->enable_blend;
	zb->enable_blend = 0;
	PIXEL col = RGB_TO_PIXEL(p2->r, p2->g, p2->b);
	draw_triangle(zb, p0, p1, p2, 0, col);
	zb->enable_blend = saved;
}

void ZB_fillTriangleSmooth(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2) { draw_triangle(zb, p0, p1, p2, 1, 0); }

void ZB_fillTriangleSmoothNOBLEND(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2) {
	int saved = zb->enable_blend;
	zb->enable_blend = 0;
	draw_triangle(zb, p0, p1, p2, 1, 0);
	zb->enable_blend = saved;
}

void ZB_fillTriangleMappingPerspective(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2) { draw_triangle(zb, p0, p1, p2, 2, 0); }

void ZB_fillTriangleMappingPerspectiveNOBLEND(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2) {
	int saved = zb->enable_blend;
	zb->enable_blend = 0;
	draw_triangle(zb, p0, p1, p2, 2, 0);
	zb->enable_blend = saved;
}
