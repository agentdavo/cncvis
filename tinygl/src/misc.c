#include "gl_utils.h"
#include "zgl.h"

void glPolygonStipple(void* a) {
#if TGL_FEATURE_POLYGON_STIPPLE == 1
	GLContext* c = gl_get_context();
#include "error_check.h"
	ZBuffer* zb = c->zb;

	memcpy(zb->stipplepattern, a, TGL_POLYGON_STIPPLE_BYTES);
	for (GLint i = 0; i < TGL_POLYGON_STIPPLE_BYTES; i++) {
		zb->stipplepattern[i] = ((GLubyte*)a)[i];
	}
#endif
}

void glopViewport(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint xsize, ysize, xmin, ymin, xsize_req, ysize_req;

	xmin = p[1].i;
	ymin = p[2].i;
	xsize = p[3].i;
	ysize = p[4].i;

	/* we may need to resize the zbuffer */

	if (c->viewport.xmin != xmin || c->viewport.ymin != ymin || c->viewport.xsize != xsize || c->viewport.ysize != ysize) {

		xsize_req = xmin + xsize;
		ysize_req = ymin + ysize;

		if (c->gl_resize_viewport && c->gl_resize_viewport(&xsize_req, &ysize_req) != 0) {
			gl_fatal_error("glViewport: error while resizing display");
		}
		if (xsize <= 0 || ysize <= 0) {
			gl_fatal_error("glViewport: size too small");
		}

		c->viewport.xmin = xmin;
		c->viewport.ymin = ymin;
		c->viewport.xsize = xsize;
		c->viewport.ysize = ysize;

		gl_eval_viewport();
	}
}
void glBlendFunc(GLenum sfactor, GLenum dfactor) {
	GLParam p[3];
#include "error_check_no_context.h"
	p[0].op = OP_BlendFunc;
	p[1].i = sfactor;
	p[2].i = dfactor;
	gl_add_op(p);
	return;
}
void glopBlendFunc(GLParam* p) {
	GLContext* c = gl_get_context();
	c->zb->sfactor = p[1].i;
	c->zb->dfactor = p[2].i;
}

void glBlendEquation(GLenum mode) {
	GLParam p[2];
#include "error_check_no_context.h"
	p[0].op = OP_BlendEquation;
	p[1].i = mode;
	gl_add_op(p);
}
void glopBlendEquation(GLParam* p) {
	GLContext* c = gl_get_context();
	c->zb->blendeq = p[1].i;
}

void glopPointSize(GLParam* p) {
	GLContext* c = gl_get_context();
	c->zb->pointsize = p[1].f;
}
void glPointSize(GLfloat f) {
	GLParam p[2];
	p[0].op = OP_PointSize;
#include "error_check_no_context.h"
	p[1].f = f;
	gl_add_op(p);
}

void glLineWidth(GLfloat f) {
	GLContext* c = gl_get_context();
#include "error_check.h"
	if (f <= 0.0f)
		return;
	c->zb->line_width = f;
}

void glDepthFunc(GLenum func) {
	GLContext* c = gl_get_context();
#include "error_check.h"
	switch (func) {
	case GL_NEVER:
	case GL_LESS:
	case GL_LEQUAL:
	case GL_GREATER:
	case GL_GEQUAL:
	case GL_EQUAL:
	case GL_NOTEQUAL:
	case GL_ALWAYS:
		c->zb->depth_func = func;
		break;
	default:
		return;
	}
}

void glopEnableDisable(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint code = p[1].i;
	GLint v = p[2].i;

	switch (code) {
	case GL_CULL_FACE:
		c->cull_face_enabled = v;
		break;
	case GL_LIGHTING:
		c->lighting_enabled = v;
		break;
	case GL_COLOR_MATERIAL:
		c->color_material_enabled = v;
		break;
	case GL_TEXTURE_2D:
		c->texture_2d_enabled = v;
		break;
	case GL_BLEND:
		c->zb->enable_blend = v;
		break;
	case GL_FOG:
		c->fog_enabled = v;
		break;
	case GL_ALPHA_TEST:
		c->alpha_test_enabled = v;
		break;
	case GL_STENCIL_TEST:
		c->stencil_test_enabled = v;
		break;
	case GL_NORMALIZE:
		c->normalize_enabled = v;
		break;
	case GL_DEPTH_TEST:
		c->zb->depth_test = v;
		break;
	case GL_POLYGON_OFFSET_FILL:
		if (v)
			c->offset_states |= TGL_OFFSET_FILL;
		else
			c->offset_states &= ~TGL_OFFSET_FILL;
		break;
	case GL_POLYGON_STIPPLE:
#if TGL_FEATURE_POLYGON_STIPPLE == 1
		c->zb->dostipple = v;
#endif
		break;
	case GL_POLYGON_OFFSET_POINT:
		if (v)
			c->offset_states |= TGL_OFFSET_POINT;
		else
			c->offset_states &= ~TGL_OFFSET_POINT;
		break;
	case GL_POLYGON_OFFSET_LINE:
		if (v)
			c->offset_states |= TGL_OFFSET_LINE;
		else
			c->offset_states &= ~TGL_OFFSET_LINE;
		break;
	case GL_SCISSOR_TEST:
		c->scissor_enabled = v;
		break;
	default:
		if (code >= GL_LIGHT0 && code < GL_LIGHT0 + MAX_LIGHTS) {
			gl_enable_disable_light(code - GL_LIGHT0, v);
		} else {
			tgl_warning("glEnableDisable: 0x%X not supported.\n", code);
		}
		break;
	}
}

void glopShadeModel(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint code = p[1].i;
	c->current_shade_model = code;
}

void glopCullFace(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint code = p[1].i;
	c->current_cull_face = code;
}

void glopFrontFace(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint code = p[1].i;
	c->current_front_face = code;
}

void glopPolygonMode(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint face = p[1].i;
	GLint mode = p[2].i;

	switch (face) {
	case GL_BACK:
		c->polygon_mode_back = mode;
		break;
	case GL_FRONT:
		c->polygon_mode_front = mode;
		break;
	case GL_FRONT_AND_BACK:
		c->polygon_mode_front = mode;
		c->polygon_mode_back = mode;
		break;
	default:
		break;
	}
}

void glopPolygonOffset(GLParam* p) {
	GLContext* c = gl_get_context();
	c->offset_factor = p[1].f;
	c->offset_units = p[2].f;
}

void glopScissor(GLParam* p) {
	GLContext* c = gl_get_context();
	c->scissor_x = p[1].i;
	c->scissor_y = p[2].i;
	c->scissor_width = p[3].i;
	c->scissor_height = p[4].i;
}

void glDrawBuffer(GLenum mode) {
	GLContext* c = gl_get_context();
#include "error_check.h"
	if ((mode != GL_FRONT && mode != GL_NONE) || c->in_begin) {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
		return;
#endif
	}
	c->drawbuffer = mode;
}

void glReadBuffer(GLenum mode) {
	GLContext* c = gl_get_context();
#include "error_check.h"
	if ((mode != GL_FRONT && mode != GL_NONE) || c->in_begin) {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
		return;
#endif
	}
	c->readbuffer = mode;
}

void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* data) {
	GLContext* c = gl_get_context();
#include "error_check.h"
	if (c->readbuffer != GL_FRONT || (format != GL_RGBA && format != GL_RGB && format != GL_BGR && format != GL_BGRA && format != GL_DEPTH_COMPONENT) ||
#if TGL_FEATURE_RENDER_BITS == 32
		(type != GL_UNSIGNED_INT && type != GL_UNSIGNED_INT_8_8_8_8)
#elif TGL_FEATURE_RENDER_BITS == 16
		(type != GL_UNSIGNED_SHORT && type != GL_UNSIGNED_SHORT_5_6_5)
#else
#error "Unsupported TGL_FEATURE_RENDER_BITS"
#endif

	) {
#if TGL_FEATURE_ERROR_CHECK
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
		return;
#endif
	}
	ZBuffer* zb = c->zb;
	if (format == GL_DEPTH_COMPONENT) {
		GLushort* dst = data;
		GLint yy = y - height;
		for (GLint j = 0; j < height; ++j) {
			GLushort* src = zb->zbuf + (yy + j) * zb->xsize + x;
			memcpy(dst + j * width, src, (size_t)width * sizeof(GLushort));
		}
		return;
	}
	GLint components = (format == GL_RGBA || format == GL_BGRA) ? 4 : 3;
	GLubyte* dst = data;
	GLint yy = y - height;
	for (GLint j = 0; j < height; ++j) {
		PIXEL* src = zb->pbuf + (yy + j) * zb->xsize + x;
		for (GLint i = 0; i < width; ++i) {
			PIXEL p = src[i];
#if TGL_FEATURE_RENDER_BITS == 32
			GLubyte r = (p >> 16) & 0xff;
			GLubyte g = (p >> 8) & 0xff;
			GLubyte b = p & 0xff;
#elif TGL_FEATURE_RENDER_BITS == 16
			GLubyte r = (p >> 8) & 0xf8;
			GLubyte g = (p >> 3) & 0xfc;
			GLubyte b = (p << 3) & 0xf8;
#endif
			GLubyte* out = dst + components * (j * width + i);
			if (format == GL_BGR || format == GL_BGRA) {
				out[0] = b;
				out[1] = g;
				out[2] = r;
			} else {
				out[0] = r;
				out[1] = g;
				out[2] = b;
			}
			if (components == 4)
				out[3] = 0xff;
		}
	}
}

void glFinish() { return; }

/* --- OpenGL 1.1 stub implementations --- */
void glClearIndex(GLfloat c) { (void)c; }

void glClearStencil(GLint s) { (void)s; }

void glClearAccum(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
	(void)r;
	(void)g;
	(void)b;
	(void)a;
}

void glAccum(GLenum op, GLfloat value) {
	(void)op;
	(void)value;
}

void glAlphaFunc(GLenum func, GLclampf ref) {
	GLContext* c = gl_get_context();
	c->alpha_func = func;
	c->alpha_ref = ref;
}

void glStencilFunc(GLenum func, GLint ref, GLuint mask) {
	GLContext* c = gl_get_context();
	c->stencil_func = func;
	c->stencil_ref = ref;
	c->stencil_mask = mask;
}

void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass) {
	(void)fail;
	(void)zfail;
	(void)zpass;
}

void glStencilMask(GLuint mask) { (void)mask; }

GLboolean glIsEnabled(GLenum cap) {
	GLContext* c = gl_get_context();
	switch (cap) {
	case GL_TEXTURE_2D:
		return c->texture_2d_enabled;
	case GL_LIGHTING:
		return c->lighting_enabled;
	case GL_COLOR_MATERIAL:
		return c->color_material_enabled;
	case GL_BLEND:
		return c->zb->enable_blend;
	case GL_FOG:
		return c->fog_enabled;
	case GL_ALPHA_TEST:
		return c->alpha_test_enabled;
	case GL_STENCIL_TEST:
		return c->stencil_test_enabled;
	case GL_NORMALIZE:
		return c->normalize_enabled;
	case GL_DEPTH_TEST:
		return c->zb->depth_test;
	case GL_CULL_FACE:
		return c->cull_face_enabled;
	case GL_POLYGON_OFFSET_FILL:
		return (c->offset_states & TGL_OFFSET_FILL) != 0;
	case GL_SCISSOR_TEST:
		return c->scissor_enabled;
	default:
		if (cap >= GL_LIGHT0 && cap < GL_LIGHT0 + MAX_LIGHTS)
			return c->lights[cap - GL_LIGHT0].enabled;
		return GL_FALSE;
	}
}
