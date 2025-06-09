#include "gl_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Message handling helpers (from msghandling.c) */
void tgl_warning(const char* format, ...) {
#ifndef NO_DEBUG_OUTPUT
	va_list args;
	va_start(args, format);
	fprintf(stderr, "*WARNING* ");
	vfprintf(stderr, format, args);
	va_end(args);
#else
	(void)format;
#endif
}

void tgl_trace(const char* format, ...) {
#ifndef NO_DEBUG_OUTPUT
	va_list args;
	va_start(args, format);
	fprintf(stderr, "*DEBUG* ");
	vfprintf(stderr, format, args);
	va_end(args);
#else
	(void)format;
#endif
}

void tgl_fixme(const char* format, ...) {
#ifndef NO_DEBUG_OUTPUT
	va_list args;
	va_start(args, format);
	fprintf(stderr, "*FIXME* ");
	vfprintf(stderr, format, args);
	va_end(args);
#else
	(void)format;
#endif
}

void gl_fatal_error(char* format, ...) {
#ifndef NO_DEBUG_OUTPUT
	va_list ap;
	va_start(ap, format);
	fprintf(stderr, "TinyGL: fatal error: ");
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");
	va_end(ap);
#endif
	exit(1);
}

/* Utility functions from get.c and misc.c */

/* Strings identifying the implementation */
static const GLubyte* license_string = (const GLubyte*)""
													   "Copyright notice:\n"
													   "\n"
													   " (C) 1997-2021 Fabrice Bellard, Gek (DMHSW), C-Chads\n"
													   "\n"
													   " This software is provided 'as-is', without any express or implied\n"
													   "  warranty.  In no event will the authors be held liable for any damages\n"
													   "   arising from the use of this software.\n"
													   "\n"
													   " Permission is granted to anyone to use this software for any purpose,\n"
													   " including commercial applications, and to alter it and redistribute it\n"
													   " freely, subject to the following restrictions:\n"
													   "\n"
													   " 1. The origin of this software must not be misrepresented; you must not\n"
													   "    claim that you wrote the original software. If you use this software\n"
													   "    in a product, an acknowledgment in the product and its documentation \n"
													   "    *is* required.\n"
													   " 2. Altered source versions must be plainly marked as such, and must not be\n"
													   "    misrepresented as being the original software.\n"
													   " 3. This notice may not be removed or altered from any source distribution.\n"
													   "\n"
													   "If you redistribute modified sources, I would appreciate that you\n"
													   "include in the files history information documenting your changes.";

static const GLubyte* vendor_string = (const GLubyte*)"Fabrice Bellard, Gek, and the C-Chads";
static const GLubyte* renderer_string = (const GLubyte*)"TinyGL";
#define xstr(s) str(s)
#define str(s) #s
static const GLubyte* version_string = (const GLubyte*)"" xstr(TINYGL_VERSION) " TinyGLv" xstr(TINYGL_VERSION);
static const GLubyte* extensions_string = (const GLubyte*)"TGL_TEXTURE TGL_SMOOTHSHADING TGL_LIGHTING";

const GLubyte* glGetString(GLenum name) {
	switch (name) {
	case GL_VENDOR:
		return vendor_string;
	case GL_RENDERER:
		return renderer_string;
	case GL_VERSION:
		return version_string;
	case GL_EXTENSIONS:
		return extensions_string;
	case GL_LICENSE:
		return license_string;
	default:
		return NULL;
	}
}

GLenum glGetError(void) {
#if TGL_FEATURE_ERROR_CHECK == 1
	GLContext* c = gl_get_context();
	GLenum eflag = c->error_flag;
	if (eflag != GL_OUT_OF_MEMORY)
		c->error_flag = GL_NO_ERROR;
	return eflag;
#else
	return GL_NO_ERROR;
#endif
}

void glFlush(void) { /* no-op for software renderer */ }

void glDebug(GLint mode) {
	GLContext* c = gl_get_context();
#include "error_check.h"
	c->print_flag = mode;
}
