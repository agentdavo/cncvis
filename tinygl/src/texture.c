/*
 * Texture Manager
 */

#include "gl_texture.h"
#include "lockstepthread.h"
#include "zgl.h"

#define NUM_TEX_THREADS TGL_NUM_THREADS
typedef void (*RowFunc)(const void*, void*, GLint);
typedef struct {
	const void* src;
	void* dst;
	GLint src_stride;
	GLint dst_stride;
	GLint width;
	GLint lines;
	RowFunc fn;
} TexJob;
static c11_lsthread tex_threads[NUM_TEX_THREADS];
static TexJob tex_jobs[NUM_TEX_THREADS];

static inline void row_copy_pixels(const void* restrict src, void* restrict dst, GLint w) { memcpy(dst, src, (size_t)w * sizeof(PIXEL)); }
#if TGL_FEATURE_NO_COPY_COLOR == 1
static inline void row_copy_pixels_ncc(const void* restrict src, void* restrict dst, GLint w) {
	const PixelQuad* s = (const PixelQuad*)src;
	PixelQuad* d = (PixelQuad*)dst;
	GLint n = w >> 2;
	for (GLint i = 0; i < n; ++i) {
		PixelQuad q = s[i];
		if ((q.px[0] & TGL_COLOR_MASK) != TGL_NO_COPY_COLOR)
			d[i].px[0] = q.px[0];
		if ((q.px[1] & TGL_COLOR_MASK) != TGL_NO_COPY_COLOR)
			d[i].px[1] = q.px[1];
		if ((q.px[2] & TGL_COLOR_MASK) != TGL_NO_COPY_COLOR)
			d[i].px[2] = q.px[2];
		if ((q.px[3] & TGL_COLOR_MASK) != TGL_NO_COPY_COLOR)
			d[i].px[3] = q.px[3];
	}
}
#endif
#if TGL_FEATURE_RENDER_BITS == 32
static inline void row_convert_rgb(const void* restrict src, void* restrict dst, GLint w) {
	const GLubyte* s = (const GLubyte*)src;
	PIXEL* d = (PIXEL*)dst;
	for (GLint i = 0; i < w; ++i) {
		d[i] = ((PIXEL)s[0] << 16) | ((PIXEL)s[1] << 8) | (PIXEL)s[2];
		s += 3;
	}
}
static inline void row_convert_bgr(const void* restrict src, void* restrict dst, GLint w) {
	const GLubyte* s = (const GLubyte*)src;
	PIXEL* d = (PIXEL*)dst;
	for (GLint i = 0; i < w; ++i) {
		d[i] = ((PIXEL)s[2] << 16) | ((PIXEL)s[1] << 8) | (PIXEL)s[0];
		s += 3;
	}
}
static inline void row_convert_bgra(const void* restrict src, void* restrict dst, GLint w) {
	const GLubyte* s = (const GLubyte*)src;
	PIXEL* d = (PIXEL*)dst;
	for (GLint i = 0; i < w; ++i) {
		d[i] = ((PIXEL)s[2] << 16) | ((PIXEL)s[1] << 8) | (PIXEL)s[0];
		s += 4;
	}
}
#elif TGL_FEATURE_RENDER_BITS == 16
static inline void row_convert_rgb(const void* restrict src, void* restrict dst, GLint w) {
	const GLubyte* s = (const GLubyte*)src;
	GLushort* d = (GLushort*)dst;
	for (GLint i = 0; i < w; ++i) {
		d[i] = (GLushort)(((s[0] & 0xF8) << 8) | ((s[1] & 0xFC) << 3) | ((s[2] & 0xF8) >> 3));
		s += 3;
	}
}
static inline void row_convert_bgr(const void* restrict src, void* restrict dst, GLint w) {
	const GLubyte* s = (const GLubyte*)src;
	GLushort* d = (GLushort*)dst;
	for (GLint i = 0; i < w; ++i) {
		d[i] = (GLushort)(((s[2] & 0xF8) << 8) | ((s[1] & 0xFC) << 3) | ((s[0] & 0xF8) >> 3));
		s += 3;
	}
}
static inline void row_convert_bgra(const void* restrict src, void* restrict dst, GLint w) {
	const GLubyte* s = (const GLubyte*)src;
	GLushort* d = (GLushort*)dst;
	for (GLint i = 0; i < w; ++i) {
		d[i] = (GLushort)(((s[2] & 0xF8) << 8) | ((s[1] & 0xFC) << 3) | ((s[0] & 0xF8) >> 3));
		s += 4;
	}
}
#endif

static void tex_job_func(void* arg) {
	TexJob* job = (TexJob*)arg;
	for (GLint y = 0; y < job->lines; ++y) {
		const void* s = (const GLbyte*)job->src + y * job->src_stride;
		void* d = (GLbyte*)job->dst + y * job->dst_stride;
		job->fn(s, d, job->width);
	}
}

static GLTexture* find_texture(GLint h) {
	GLTexture* t;
	GLContext* c = gl_get_context();
	t = c->shared_state.texture_hash_table[h & TEXTURE_HASH_TABLE_MASK];
	while (t != NULL) {
		if (t->handle == h)
			return t;
		t = t->next;
	}
	return NULL;
}

GLboolean glAreTexturesResident(GLsizei n, const GLuint* textures, GLboolean* residences) {
#define RETVAL GL_FALSE
	GLboolean retval = GL_TRUE;
	GLint i;
#include "error_check_no_context.h"

	for (i = 0; i < n; i++)
		if (find_texture(textures[i])) {
			residences[i] = GL_TRUE;
		} else {
			residences[i] = GL_FALSE;
			retval = GL_FALSE;
		}
	return retval;
}
GLboolean glIsTexture(GLuint texture) {
#define RETVAL GL_FALSE
#include "error_check.h"
	if (find_texture(texture))
		return GL_TRUE;
	return GL_FALSE;
}

void* glGetTexturePixmap(GLint text, GLint level, GLint* xsize, GLint* ysize) {
	GLTexture* tex;
	GLContext* c = gl_get_context();
#if TGL_FEATURE_ERROR_CHECK == 1
	if (!(text >= 0 && level < MAX_TEXTURE_LEVELS))
#define ERROR_FLAG GL_INVALID_ENUM
#define RETVAL NULL
#include "error_check.h"
#else
	/*assert(text >= 0 && level < MAX_TEXTURE_LEVELS);*/
#endif
		tex = find_texture(text);
	if (!tex)
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_INVALID_ENUM
#define RETVAL NULL
#include "error_check.h"
#else
		return NULL;
#endif
		*xsize = tex->images[level].xsize;
	*ysize = tex->images[level].ysize;
	return tex->images[level].pixmap;
}

static void free_texture(GLContext* c, GLint h) {
	GLTexture *t, **ht;

	t = find_texture(h);
	if (t->prev == NULL) {
		ht = &c->shared_state.texture_hash_table[t->handle & TEXTURE_HASH_TABLE_MASK];
		*ht = t->next;
	} else {
		t->prev->next = t->next;
	}
	if (t->next != NULL)
		t->next->prev = t->prev;

	gl_free(t);
}

GLTexture* alloc_texture(GLint h) {
	GLContext* c = gl_get_context();
	GLTexture *t, **ht;
#define RETVAL NULL
#include "error_check.h"
	t = gl_zalloc(sizeof(GLTexture));
	if (!t)
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#define RETVAL NULL
#include "error_check.h"
#else
		gl_fatal_error("GL_OUT_OF_MEMORY");
#endif

		ht = &c->shared_state.texture_hash_table[h & TEXTURE_HASH_TABLE_MASK];

	t->next = *ht;
	t->prev = NULL;
	if (t->next != NULL)
		t->next->prev = t;
	*ht = t;

	t->handle = h;
	t->wrap_s = GL_REPEAT;
	t->wrap_t = GL_REPEAT;
	t->min_filter = GL_NEAREST;
	t->mag_filter = GL_NEAREST;

	return t;
}

void glInitTextures() {
	/* textures */
	GLContext* c = gl_get_context();
	c->texture_2d_enabled = 0;
	c->texture_env_mode = GL_MODULATE;
	c->current_texture = find_texture(0);
	for (int i = 0; i < NUM_TEX_THREADS; ++i) {
		init_c11_lsthread(&tex_threads[i]);
		tex_threads[i].execute = tex_job_func;
		tex_threads[i].argument = &tex_jobs[i];
		start_c11_lsthread(&tex_threads[i]);
	}
}

void glEndTextures() {
	for (int i = 0; i < NUM_TEX_THREADS; ++i) {
		kill_c11_lsthread(&tex_threads[i]);
		destroy_c11_lsthread(&tex_threads[i]);
	}
}

void glGenTextures(GLint n, GLuint* textures) {
	GLContext* c = gl_get_context();
	GLint max, i;
	GLTexture* t;
#include "error_check.h"
	max = 0;
	for (i = 0; i < TEXTURE_HASH_TABLE_SIZE; i++) {
		t = c->shared_state.texture_hash_table[i];
		while (t != NULL) {
			if (t->handle > max)
				max = t->handle;
			t = t->next;
		}
	}
	for (i = 0; i < n; i++) {
		textures[i] = max + i + 1; /* MARK: How texture handles are created.*/
	}
}

void glDeleteTextures(GLint n, const GLuint* textures) {
	GLint i;
	GLTexture* t;
	GLContext* c = gl_get_context();
#include "error_check.h"
	for (i = 0; i < n; i++) {
		t = find_texture(textures[i]);
		if (t != NULL && t != 0) {
			if (t == c->current_texture) {
				glBindTexture(GL_TEXTURE_2D, 0);
#include "error_check.h"
			}
			free_texture(c, textures[i]);
		}
	}
}

/* Wrappers moved from api.c */
void glTexImage2D(GLint target, GLint level, GLint components, GLint width, GLint height, GLint border, GLint format, GLint type, void* pixels) {
	GLParam p[10];
#include "error_check_no_context.h"
	p[0].op = OP_TexImage2D;
	p[1].i = target;
	p[2].i = level;
	p[3].i = components;
	p[4].i = width;
	p[5].i = height;
	p[6].i = border;
	p[7].i = format;
	p[8].i = type;
	p[9].p = pixels;

	gl_add_op(p);
}

void glTexImage1D(GLint target, GLint level, GLint components, GLint width, GLint border, GLint format, GLint type, void* pixels) {
	GLParam p[10];
#include "error_check_no_context.h"
	p[0].op = OP_TexImage1D;
	p[1].i = target;
	p[2].i = level;
	p[3].i = components;
	p[4].i = width;
	p[5].i = border;
	p[6].i = format;
	p[7].i = type;
	p[8].p = pixels;
	gl_add_op(p);
}

void glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type,
				  const void* pixels) {
	if (depth != 1) {
		gl_fatal_error("glTexImage3D: only depth 1 supported");
		return;
	}
	glTexImage2D(target, level, internalformat, width, height, border, format, type, (void*)pixels);
}

void glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format,
					 GLenum type, const void* pixels) {
	if (zoffset != 0 || depth != 1) {
		gl_fatal_error("glTexSubImage3D: only depth 1 supported");
		return;
	}
	glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

void glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) {
	if (zoffset != 0) {
		gl_fatal_error("glCopyTexSubImage3D: only depth 1 supported");
		return;
	}
	glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
}

void glBindTexture(GLint target, GLint texture) {
	GLParam p[3];
#include "error_check_no_context.h"
	p[0].op = OP_BindTexture;
	p[1].i = target;
	p[2].i = texture;

	gl_add_op(p);
}

void glTexEnvi(GLint target, GLint pname, GLint param) {
	GLParam p[4];
#include "error_check_no_context.h"
	p[0].op = OP_TexEnvi;
	p[1].i = target;
	p[2].i = pname;
	p[3].i = param;
	gl_add_op(p);
}

void glTexEnvf(GLint target, GLint pname, GLfloat param) { glTexEnvi(target, pname, (GLint)param); }

void glTexEnvfv(GLint target, GLint pname, const GLfloat* params) { glTexEnvf(target, pname, params[0]); }

void glTexParameteri(GLint target, GLint pname, GLint param) {
	GLParam p[4];
#include "error_check_no_context.h"
	p[0].op = OP_TexParameter;
	p[1].i = target;
	p[2].i = pname;
	p[3].i = param;
	gl_add_op(p);
}

void glTexParameterf(GLint target, GLint pname, GLfloat param) { glTexParameteri(target, pname, (GLint)param); }

void glTexParameterfv(GLint target, GLint pname, const GLfloat* params) { glTexParameterf(target, pname, params[0]); }

void glopBindTexture(GLParam* p) {
	GLint target = p[1].i;
	GLint texture = p[2].i;
	GLTexture* t;
	GLContext* c = gl_get_context();
#if TGL_FEATURE_ERROR_CHECK == 1
	if (!(target == GL_TEXTURE_2D && target > 0))
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else

#endif
		t = find_texture(texture);
	if (t == NULL) {
		t = alloc_texture(texture);
#include "error_check.h"
	}
	if (t == NULL) {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
		gl_fatal_error("GL_OUT_OF_MEMORY");
#endif
	}
	c->current_texture = t;
}

void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) {
	GLParam p[9];
#include "error_check_no_context.h"

	p[0].op = OP_CopyTexImage2D;
	p[1].i = target;
	p[2].i = level;
	p[3].i = internalformat;
	p[4].i = x;
	p[5].i = y;
	p[6].i = width;
	p[7].i = height;
	p[8].i = border;
	gl_add_op(p);
}
void glopCopyTexImage2D(GLParam* p) {
	GLImage* im;
	PIXEL* data;
	GLint i, j;
	GLint target = p[1].i;
	GLint level = p[2].i;
	GLint x = p[4].i;
	GLint y = p[5].i;
	GLsizei w = p[6].i;
	GLsizei h = p[7].i;
	GLint border = p[8].i;
	GLContext* c = gl_get_context();
	/* convert GL coordinates to TinyGL's bottom-left origin */
	y -= h;

	if (c->readbuffer != GL_FRONT || c->current_texture == NULL || target != GL_TEXTURE_2D || border != 0 ||
		w != TGL_FEATURE_TEXTURE_DIM || /*TODO Implement image interp*/
		h != TGL_FEATURE_TEXTURE_DIM) {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
		return;
#endif
	}
	im = &c->current_texture->images[level];
	data = c->current_texture->images[level].pixmap;
	im->xsize = TGL_FEATURE_TEXTURE_DIM;
	im->ysize = TGL_FEATURE_TEXTURE_DIM;
	/* Simple memcpy when the region lies within the framebuffer */
	if (x >= 0 && y >= 0 && x + w <= c->zb->xsize && y + h <= c->zb->ysize) {
		PIXEL* src = c->zb->pbuf + y * c->zb->xsize + x;
		if (tgl_threads_enabled && w * h >= 4096) {
			int total = NUM_TEX_THREADS + 1;
			GLint strip = h / total;
			for (int t = 0; t < NUM_TEX_THREADS; ++t) {
				GLint start = t * strip;
				tex_jobs[t].src = src + start * c->zb->xsize;
				tex_jobs[t].dst = data + start * w;
				tex_jobs[t].src_stride = c->zb->xsize * sizeof(PIXEL);
				tex_jobs[t].dst_stride = w * sizeof(PIXEL);
				tex_jobs[t].width = w;
				tex_jobs[t].lines = strip;
#if TGL_FEATURE_NO_COPY_COLOR == 1
				tex_jobs[t].fn = row_copy_pixels_ncc;
#else
				tex_jobs[t].fn = row_copy_pixels;
#endif
				step_c11_lsthread(&tex_threads[t]);
			}
			GLint start = NUM_TEX_THREADS * strip;
			for (j = start; j < h; ++j) {
#if TGL_FEATURE_NO_COPY_COLOR == 1
				row_copy_pixels_ncc(src + j * c->zb->xsize, data + j * w, w);
#else
				memcpy(data + j * w, src + j * c->zb->xsize, (size_t)w * sizeof(PIXEL));
#endif
			}
			for (int t = 0; t < NUM_TEX_THREADS; ++t)
				lock_c11_lsthread(&tex_threads[t]);
		} else {
			for (j = 0; j < h; ++j) {
#if TGL_FEATURE_NO_COPY_COLOR == 1
				row_copy_pixels_ncc(src + j * c->zb->xsize, data + j * w, w);
#else
				memcpy(data + j * w, src + j * c->zb->xsize, (size_t)w * sizeof(PIXEL));
#endif
			}
		}
	} else {
		for (j = 0; j < h; ++j) {
			int src_y = j + y;
			src_y %= c->zb->ysize;
			if (src_y < 0)
				src_y += c->zb->ysize;
			PIXEL* src_line = c->zb->pbuf + src_y * c->zb->xsize;
			for (i = 0; i < w; ++i) {
				int src_x = i + x;
				src_x %= c->zb->xsize;
				if (src_x < 0)
					src_x += c->zb->xsize;
				data[i + j * w] = src_line[src_x];
			}
		}
	}
}

void glopTexImage1D(GLParam* p) {
	GLint target = p[1].i;
	GLint level = p[2].i;
	GLint components = p[3].i;
	GLint width = p[4].i;
	/* GLint height = p[5].i;*/
	GLint height = 1;
	GLint border = p[5].i;
	GLint format = p[6].i;
	GLint type = p[7].i;
	void* pixels = p[8].p;
	GLImage* im;
	GLubyte* pixels1;
	GLint do_free = 0;
	GLContext* c = gl_get_context();
	{
#if TGL_FEATURE_ERROR_CHECK == 1
		if (!(c->current_texture != NULL && target == GL_TEXTURE_1D && level == 0 && components == 3 && border == 0 && format == GL_RGB &&
			  type == GL_UNSIGNED_BYTE))
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"

#else
		if (!(c->current_texture != NULL && target == GL_TEXTURE_1D && level == 0 && components == 3 && border == 0 && format == GL_RGB &&
			  type == GL_UNSIGNED_BYTE))
			gl_fatal_error("glTexImage2D: combination of parameters not handled!!");
#endif
	}
	GLint comps = (format == GL_BGRA) ? 4 : 3;
	if ((format != GL_BGRA) && (width != TGL_FEATURE_TEXTURE_DIM || height != TGL_FEATURE_TEXTURE_DIM)) {
		pixels1 = gl_malloc(TGL_FEATURE_TEXTURE_DIM * TGL_FEATURE_TEXTURE_DIM * comps); /* GUARDED*/
		if (pixels1 == NULL) {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
			gl_fatal_error("GL_OUT_OF_MEMORY");
#endif
		}
		/* no GLinterpolation is done here to respect the original image aliasing ! */

		gl_resizeImageNoInterpolate(pixels1, TGL_FEATURE_TEXTURE_DIM, TGL_FEATURE_TEXTURE_DIM, pixels, width, height);
		do_free = 1;
		width = TGL_FEATURE_TEXTURE_DIM;
		height = TGL_FEATURE_TEXTURE_DIM;
	} else {
		pixels1 = pixels;
	}

	im = &c->current_texture->images[level];
	im->xsize = width;
	im->ysize = height;
#if TGL_FEATURE_RENDER_BITS == 32
	gl_convertRGB_to_8A8R8G8B(im->pixmap, pixels1, width, height);
#elif TGL_FEATURE_RENDER_BITS == 16
	gl_convertRGB_to_5R6G5B(im->pixmap, pixels1, width, height);
#else
#error bad TGL_FEATURE_RENDER_BITS
#endif
	if (do_free)
		gl_free(pixels1);
}
void glopTexImage2D(GLParam* p) {
	GLint target = p[1].i;
	GLint level = p[2].i;
	GLint components = p[3].i;
	GLint width = p[4].i;
	GLint height = p[5].i;
	GLint border = p[6].i;
	GLint format = p[7].i;
	GLint type = p[8].i;
	void* pixels = p[9].p;
	GLImage* im;
	GLubyte* pixels1;
	GLint do_free = 0;
	GLint comps = (format == GL_BGRA) ? 4 : 3;
	GLContext* c = gl_get_context();
	{
#if TGL_FEATURE_ERROR_CHECK == 1
		if (!(c->current_texture != NULL && target == GL_TEXTURE_2D && level == 0 && (components == 3 || components == 4) && border == 0 &&
			  (format == GL_RGB || format == GL_BGR || format == GL_BGRA) && type == GL_UNSIGNED_BYTE))
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"

#else
		if (!(c->current_texture != NULL && target == GL_TEXTURE_2D && level == 0 && (components == 3 || components == 4) && border == 0 &&
			  (format == GL_RGB || format == GL_BGR || format == GL_BGRA) && type == GL_UNSIGNED_BYTE))
			gl_fatal_error("glTexImage2D: combination of parameters not handled!!");
#endif
	}
	if (width != TGL_FEATURE_TEXTURE_DIM || height != TGL_FEATURE_TEXTURE_DIM) {
		pixels1 = gl_malloc(TGL_FEATURE_TEXTURE_DIM * TGL_FEATURE_TEXTURE_DIM * 3); /* GUARDED*/
		if (pixels1 == NULL) {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
			gl_fatal_error("GL_OUT_OF_MEMORY");
#endif
		}
		/* no GLinterpolation is done here to respect the original image aliasing ! */

		gl_resizeImageNoInterpolate(pixels1, TGL_FEATURE_TEXTURE_DIM, TGL_FEATURE_TEXTURE_DIM, pixels, width, height);
		do_free = 1;
		width = TGL_FEATURE_TEXTURE_DIM;
		height = TGL_FEATURE_TEXTURE_DIM;
	} else {
		pixels1 = pixels;
	}

	im = &c->current_texture->images[level];
	im->xsize = width;
	im->ysize = height;
	if (tgl_threads_enabled && width * height >= 4096) {
		int total = NUM_TEX_THREADS + 1;
		GLint strip = height / total;
		for (int t = 0; t < NUM_TEX_THREADS; ++t) {
			GLint start = t * strip;
			tex_jobs[t].src = pixels1 + start * width * comps;
			tex_jobs[t].dst = im->pixmap + start * width;
			tex_jobs[t].src_stride = width * comps;
			tex_jobs[t].dst_stride = width * sizeof(PIXEL);
			tex_jobs[t].width = width;
			tex_jobs[t].lines = strip;
			if (format == GL_BGR)
				tex_jobs[t].fn = row_convert_bgr;
			else if (format == GL_BGRA)
				tex_jobs[t].fn = row_convert_bgra;
			else
				tex_jobs[t].fn = row_convert_rgb;
			step_c11_lsthread(&tex_threads[t]);
		}
		GLint start = NUM_TEX_THREADS * strip;
		for (GLint y = start; y < height; ++y)
			if (format == GL_BGR)
				row_convert_bgr(pixels1 + y * width * comps, im->pixmap + y * width, width);
			else if (format == GL_BGRA)
				row_convert_bgra(pixels1 + y * width * comps, im->pixmap + y * width, width);
			else
				row_convert_rgb(pixels1 + y * width * comps, im->pixmap + y * width, width);
		for (int t = 0; t < NUM_TEX_THREADS; ++t)
			lock_c11_lsthread(&tex_threads[t]);
	} else {
		for (GLint y = 0; y < height; ++y) {
			const GLubyte* s = pixels1 + y * width * comps;
			PIXEL* d = im->pixmap + y * width;
			if (format == GL_BGR)
				row_convert_bgr(s, d, width);
			else if (format == GL_BGRA)
				row_convert_bgra(s, d, width);
			else
				row_convert_rgb(s, d, width);
		}
	}
	if (do_free)
		gl_free(pixels1);
}

void glopTexEnvi(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint target = p[1].i;
	GLint pname = p[2].i;
	GLint param = p[3].i;

#if TGL_FEATURE_ERROR_CHECK == 1
	if (target != GL_TEXTURE_ENV || pname != GL_TEXTURE_ENV_MODE)
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#endif
		if (target == GL_TEXTURE_ENV && pname == GL_TEXTURE_ENV_MODE)
			c->texture_env_mode = param;
}

void glopTexParameter(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint target = p[1].i;
	GLint pname = p[2].i;
	GLint param = p[3].i;

#if TGL_FEATURE_ERROR_CHECK == 1
	if (target != GL_TEXTURE_2D && target != GL_TEXTURE_1D)
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#endif

		GLTexture* t = c->current_texture;
	switch (pname) {
	case GL_TEXTURE_WRAP_S:
		t->wrap_s = param;
		break;
	case GL_TEXTURE_WRAP_T:
		t->wrap_t = param;
		break;
	case GL_TEXTURE_MIN_FILTER:
		t->min_filter = param;
		break;
	case GL_TEXTURE_MAG_FILTER:
		t->mag_filter = param;
		break;
	default:
		break;
	}
}

void glTexSubImage1D(GLint target, GLint level, GLint xoffset, GLsizei width, GLint format, GLint type, const void* pixels) {
	GLParam p[9];
#include "error_check_no_context.h"
	p[0].op = OP_TexSubImage1D;
	p[1].i = target;
	p[2].i = level;
	p[3].i = xoffset;
	p[4].i = width;
	p[5].i = format;
	p[6].i = type;
	p[7].p = (void*)pixels;
	gl_add_op(p);
}

void glTexSubImage2D(GLint target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLint format, GLint type, const void* pixels) {
	GLParam p[11];
#include "error_check_no_context.h"
	p[0].op = OP_TexSubImage2D;
	p[1].i = target;
	p[2].i = level;
	p[3].i = xoffset;
	p[4].i = yoffset;
	p[5].i = width;
	p[6].i = height;
	p[7].i = format;
	p[8].i = type;
	p[9].p = (void*)pixels;
	gl_add_op(p);
}

void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) {
	GLParam p[10];
#include "error_check_no_context.h"
	p[0].op = OP_CopyTexSubImage2D;
	p[1].i = target;
	p[2].i = level;
	p[3].i = xoffset;
	p[4].i = yoffset;
	p[5].i = x;
	p[6].i = y;
	p[7].i = width;
	p[8].i = height;
	gl_add_op(p);
}

static void copy_subimage(GLImage* im, GLint xoff, GLint yoff, GLint w, GLint h, const PIXEL* src, GLint src_stride) {
	PIXEL* dst = im->pixmap + yoff * im->xsize + xoff;
	if (tgl_threads_enabled && w * h >= 4096) {
		int total = NUM_TEX_THREADS + 1;
		GLint strip = h / total;
		for (int t = 0; t < NUM_TEX_THREADS; ++t) {
			GLint start = t * strip;
			tex_jobs[t].src = src + start * src_stride / sizeof(PIXEL);
			tex_jobs[t].dst = dst + start * im->xsize;
			tex_jobs[t].src_stride = src_stride;
			tex_jobs[t].dst_stride = im->xsize * sizeof(PIXEL);
			tex_jobs[t].width = w;
			tex_jobs[t].lines = strip;
			tex_jobs[t].fn = row_copy_pixels;
			step_c11_lsthread(&tex_threads[t]);
		}
		GLint start = NUM_TEX_THREADS * strip;
		for (GLint j = start; j < h; ++j)
			row_copy_pixels(src + j * (src_stride / sizeof(PIXEL)), dst + j * im->xsize, w);
		for (int t = 0; t < NUM_TEX_THREADS; ++t)
			lock_c11_lsthread(&tex_threads[t]);
	} else {
		for (GLint j = 0; j < h; ++j)
			memcpy(dst + j * im->xsize, src + j * (src_stride / sizeof(PIXEL)), (size_t)w * sizeof(PIXEL));
	}
}

void glopTexSubImage1D(GLParam* p) {
	GLint target = p[1].i;
	GLint level = p[2].i;
	GLint xoffset = p[3].i;
	GLint width = p[4].i;
	GLint format = p[5].i;
	GLint type = p[6].i;
	void* pixels = p[7].p;
	GLContext* c = gl_get_context();
	if (!(c->current_texture && target == GL_TEXTURE_1D && level == 0 && format == GL_RGB && type == GL_UNSIGNED_BYTE))
		return;
	GLImage* im = &c->current_texture->images[level];
	if (xoffset < 0 || xoffset + width > im->xsize)
		return;
	GLubyte* buf = pixels;
	PIXEL tmp[width];
	for (GLint i = 0; i < width; ++i) {
		tmp[i] = ((PIXEL)buf[3 * i] << 16) | ((PIXEL)buf[3 * i + 1] << 8) | (PIXEL)buf[3 * i + 2];
	}
	memcpy(im->pixmap + xoffset, tmp, (size_t)width * sizeof(PIXEL));
}

void glopTexSubImage2D(GLParam* p) {
	GLint target = p[1].i;
	GLint level = p[2].i;
	GLint xoffset = p[3].i;
	GLint yoffset = p[4].i;
	GLint width = p[5].i;
	GLint height = p[6].i;
	GLint format = p[7].i;
	GLint type = p[8].i;
	void* pixels = p[9].p;
	GLContext* c = gl_get_context();
	if (!(c->current_texture && target == GL_TEXTURE_2D && level == 0 && format == GL_RGB && type == GL_UNSIGNED_BYTE))
		return;
	GLImage* im = &c->current_texture->images[level];
	if (xoffset < 0 || yoffset < 0 || xoffset + width > im->xsize || yoffset + height > im->ysize)
		return;
	GLubyte* buf = pixels;
	PIXEL line[width];
	for (GLint y = 0; y < height; ++y) {
		for (GLint x = 0; x < width; ++x) {
			GLubyte* src = buf + 3 * (y * width + x);
			line[x] = ((PIXEL)src[0] << 16) | ((PIXEL)src[1] << 8) | (PIXEL)src[2];
		}
		memcpy(im->pixmap + (yoffset + y) * im->xsize + xoffset, line, (size_t)width * sizeof(PIXEL));
	}
}

void glopCopyTexSubImage2D(GLParam* p) {
	GLint target = p[1].i;
	GLint level = p[2].i;
	GLint xoffset = p[3].i;
	GLint yoffset = p[4].i;
	GLint x = p[5].i;
	GLint y = p[6].i;
	GLint width = p[7].i;
	GLint height = p[8].i;
	GLContext* c = gl_get_context();
	if (!(c->readbuffer == GL_FRONT && c->current_texture && target == GL_TEXTURE_2D))
		return;
	GLImage* im = &c->current_texture->images[level];
	if (xoffset < 0 || yoffset < 0 || xoffset + width > im->xsize || yoffset + height > im->ysize)
		return;
	y -= height;
	PIXEL* src = c->zb->pbuf + y * c->zb->xsize + x;
	copy_subimage(im, xoffset, yoffset, width, height, src, c->zb->xsize * sizeof(PIXEL));
}

/*
void glopPixelStore(GLContext* c, GLParam* p) {
	GLint pname = p[1].i;
	GLint param = p[2].i;

		if (pname != GL_UNPACK_ALIGNMENT || param != 1) {
				gl_fatal_error("glPixelStore: unsupported option");
		}
}
*/

/* Utility routines from image_util.c */
void gl_convertRGB_to_5R6G5B(GLushort* pixmap, GLubyte* rgb, GLint xsize, GLint ysize) {
	GLint i, n;
	GLubyte* p = rgb;
	n = xsize * ysize;
	for (i = 0; i < n; i++) {
		pixmap[i] = ((p[0] & 0xF8) << 8) | ((p[1] & 0xFC) << 3) | ((p[2] & 0xF8) >> 3);
		p += 3;
	}
}

void gl_convertRGB_to_8A8R8G8B(GLuint* pixmap, GLubyte* rgb, GLint xsize, GLint ysize) {
	GLint i, n;
	GLubyte* p = rgb;
	n = xsize * ysize;
	for (i = 0; i < n; i++) {
		pixmap[i] = (((GLuint)p[0]) << 16) | (((GLuint)p[1]) << 8) | ((GLuint)p[2]);
		p += 3;
	}
}

#define INTERP_NORM_BITS 16
#define INTERP_NORM (1 << INTERP_NORM_BITS)
static GLint GLinterpolate_imutil(GLint v00, GLint v01, GLint v10, GLint xf, GLint yf) {
	return v00 + (((v01 - v00) * xf + (v10 - v00) * yf) >> INTERP_NORM_BITS);
}

void gl_resizeImage(GLubyte* dest, GLint xsize_dest, GLint ysize_dest, GLubyte* src, GLint xsize_src, GLint ysize_src) {
	GLubyte *pix = dest, *pix_src = src;
	GLfloat x1, y1, x1inc, y1inc;
	GLint xi, yi, j, xf, yf, x, y;

	x1inc = (GLfloat)(xsize_src - 1) / (GLfloat)(xsize_dest - 1);
	y1inc = (GLfloat)(ysize_src - 1) / (GLfloat)(ysize_dest - 1);
	y1 = 0;
	for (y = 0; y < ysize_dest; y++) {
		x1 = 0;
		for (x = 0; x < xsize_dest; x++) {
			xi = (GLint)x1;
			yi = (GLint)y1;
			xf = (GLint)((x1 - floor(x1)) * INTERP_NORM);
			yf = (GLint)((y1 - floor(y1)) * INTERP_NORM);
			if ((xf + yf) <= INTERP_NORM) {
				for (j = 0; j < 3; j++) {
					pix[j] = GLinterpolate_imutil(pix_src[(yi * xsize_src + xi) * 3 + j], pix_src[(yi * xsize_src + xi + 1) * 3 + j],
												  pix_src[((yi + 1) * xsize_src + xi) * 3 + j], xf, yf);
				}
			} else {
				xf = INTERP_NORM - xf;
				yf = INTERP_NORM - yf;
				for (j = 0; j < 3; j++) {
					pix[j] = GLinterpolate_imutil(pix_src[((yi + 1) * xsize_src + xi + 1) * 3 + j], pix_src[((yi + 1) * xsize_src + xi) * 3 + j],
												  pix_src[(yi * xsize_src + xi + 1) * 3 + j], xf, yf);
				}
			}
			pix += 3;
			x1 += x1inc;
		}
		y1 += y1inc;
	}
}

#define FRAC_BITS 16
void gl_resizeImageNoInterpolate(GLubyte* dest, GLint xsize_dest, GLint ysize_dest, GLubyte* src, GLint xsize_src, GLint ysize_src) {
	GLubyte *pix = dest, *pix_src = src, *pix1;
	GLint x1, y1, x1inc, y1inc;
	GLint xi, yi, x, y;

	x1inc = (GLint)((GLfloat)((xsize_src) << FRAC_BITS) / (GLfloat)(xsize_dest));
	y1inc = (GLint)((GLfloat)((ysize_src) << FRAC_BITS) / (GLfloat)(ysize_dest));
	y1 = 0;
	for (y = 0; y < ysize_dest; y++) {
		x1 = 0;
		for (x = 0; x < xsize_dest; x++) {
			xi = x1 >> FRAC_BITS;
			yi = y1 >> FRAC_BITS;
			pix1 = pix_src + (yi * xsize_src + xi) * 3;
			pix[0] = pix1[0];
			pix[1] = pix1[1];
			pix[2] = pix1[2];
			pix += 3;
			x1 += x1inc;
		}
		y1 += y1inc;
	}
}
