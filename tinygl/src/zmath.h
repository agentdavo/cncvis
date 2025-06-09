#ifndef __ZMATH__
#define __ZMATH__
#include "../include/GL/gl.h"
#include "../include/zfeatures.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
/* Matrix & Vertex */

typedef struct {
	TGL_ALIGN GLfloat m[4][4];
} M4;

typedef struct {
	TGL_ALIGN GLfloat m[3][3];
} M3;

typedef struct {
	TGL_ALIGN GLfloat m[3][4];
} M34;

#define X v[0]
#define Y v[1]
#define Z v[2]
#define W v[3]

typedef struct {
	TGL_ALIGN GLfloat v[3];
} V3;

typedef struct {
	TGL_ALIGN GLfloat v[4];
} V4;

void gl_M4_Id(M4* a);
GLint gl_M4_IsId(M4* a);
void gl_M4_Move(M4* a, M4* b);
void gl_MoveV3(V3* a, V3* b);
void gl_MulM4V3(V3* a, M4* b, V3* c);
void gl_MulM3V3(V3* a, M4* b, V3* c);

void gl_M4_MulV4(V4* a, M4* b, V4* c);
void gl_M4_InvOrtho(M4* a, M4 b);
void gl_M4_Inv(M4* a, M4* b);
void gl_M4_Mul(M4* restrict c, const M4* restrict a, const M4* restrict b);
void gl_M4_MulLeft(M4* restrict c, const M4* restrict a);
void gl_M4_Transpose(M4* a, M4* b);
void gl_M4_Rotate(M4* c, GLfloat t, GLint u);

V3 gl_V3_New(GLfloat x, GLfloat y, GLfloat z);
V4 gl_V4_New(GLfloat x, GLfloat y, GLfloat z, GLfloat w);

GLint gl_Matrix_Inv(GLfloat* r, GLfloat* m, GLint n);

#if TGL_FEATURE_FISR == 1
static GLfloat fastInvSqrt(GLfloat x) {
	GLint i;
	GLfloat x2;
	memcpy(&i, &x, 4);
	i = 0x5F1FFFF9 - (i >> 1);
	memcpy(&x2, &i, 4);
	x2 *= 0.703952253f * (2.38924456f - x * x2 * x2);
	return x2;
}
#endif

static int gl_V3_Norm_Fast(V3* a) {
	GLfloat n;
#if TGL_FEATURE_FISR == 1
	n = fastInvSqrt(a->X * a->X + a->Y * a->Y + a->Z * a->Z);
	if (n > 1E+3)
		return 1;
#else
	n = sqrtf(a->X * a->X + a->Y * a->Y + a->Z * a->Z);
	if (n == 0)
		return 1;
	n = 1.0 / n;
#endif
	a->X *= n;
	a->Y *= n;
	a->Z *= n;
	return 0;
}

/* ------------------------------------------------------------------ */
/* SIMD-friendly helpers previously in chad_math.h */

typedef float f32;
typedef uint32_t u32;

typedef struct {
	alignas(16) f32 d[3];
} vec3;
typedef struct {
	alignas(16) int d[3];
} ivec3;
typedef struct {
	alignas(16) f32 d[4];
} vec4;
typedef struct {
	alignas(16) f32 d[16];
} mat4;

typedef struct {
	vec4 c;
	vec3 e;
} aabb;
typedef aabb colshape;

static inline vec3 vec3_add(vec3 a, vec3 b) { return (vec3){.d = {a.d[0] + b.d[0], a.d[1] + b.d[1], a.d[2] + b.d[2]}}; }
static inline vec3 vec3_sub(vec3 a, vec3 b) { return (vec3){.d = {a.d[0] - b.d[0], a.d[1] - b.d[1], a.d[2] - b.d[2]}}; }
static inline vec3 vec3_scale(f32 s, vec3 v) { return (vec3){.d = {s * v.d[0], s * v.d[1], s * v.d[2]}}; }
static inline vec3 vec3_mul(vec3 a, vec3 b) { return (vec3){.d = {a.d[0] * b.d[0], a.d[1] * b.d[1], a.d[2] * b.d[2]}}; }
static inline f32 vec3_dot(vec3 a, vec3 b) { return a.d[0] * b.d[0] + a.d[1] * b.d[1] + a.d[2] * b.d[2]; }
static inline f32 vec3_length(vec3 a) { return sqrtf(vec3_dot(a, a)); }
static inline vec3 vec3_cross(vec3 a, vec3 b) {
	return (vec3){.d = {a.d[1] * b.d[2] - a.d[2] * b.d[1], a.d[2] * b.d[0] - a.d[0] * b.d[2], a.d[0] * b.d[1] - a.d[1] * b.d[0]}};
}
static inline vec3 vec3_clamp(vec3 a, vec3 mn, vec3 mx) {
	return (vec3){.d = {fminf(fmaxf(a.d[0], mn.d[0]), mx.d[0]), fminf(fmaxf(a.d[1], mn.d[1]), mx.d[1]), fminf(fmaxf(a.d[2], mn.d[2]), mx.d[2])}};
}
static inline vec3 vec3_normalize(vec3 a) {
	f32 len = vec3_length(a);
	return (len == 0) ? (vec3){.d = {0, 0, 1}} : vec3_scale(1.0f / len, a);
}

static inline vec4 vec4_add(vec4 a, vec4 b) { return (vec4){.d = {a.d[0] + b.d[0], a.d[1] + b.d[1], a.d[2] + b.d[2], a.d[3] + b.d[3]}}; }
static inline vec4 vec4_sub(vec4 a, vec4 b) { return (vec4){.d = {a.d[0] - b.d[0], a.d[1] - b.d[1], a.d[2] - b.d[2], a.d[3] - b.d[3]}}; }
static inline vec4 vec4_scale(f32 s, vec4 v) { return (vec4){.d = {s * v.d[0], s * v.d[1], s * v.d[2], s * v.d[3]}}; }
static inline vec4 vec4_mul(vec4 a, vec4 b) { return (vec4){.d = {a.d[0] * b.d[0], a.d[1] * b.d[1], a.d[2] * b.d[2], a.d[3] * b.d[3]}}; }
static inline f32 vec4_dot(vec4 a, vec4 b) { return a.d[0] * b.d[0] + a.d[1] * b.d[1] + a.d[2] * b.d[2] + a.d[3] * b.d[3]; }
static inline f32 vec4_length(vec4 a) { return sqrtf(vec4_dot(a, a)); }
static inline vec4 vec4_clamp(vec4 a, vec4 mn, vec4 mx) {
	return (vec4){.d = {fminf(fmaxf(a.d[0], mn.d[0]), mx.d[0]), fminf(fmaxf(a.d[1], mn.d[1]), mx.d[1]), fminf(fmaxf(a.d[2], mn.d[2]), mx.d[2]),
						fminf(fmaxf(a.d[3], mn.d[3]), mx.d[3])}};
}
static inline vec4 vec4_normalize(vec4 a) {
	f32 len = vec4_length(a);
	return (len == 0) ? (vec4){.d = {0, 0, 1, 0}} : vec4_scale(1.0f / len, a);
}
static inline vec4 vec4_from_vec3(vec3 v, f32 w) { return (vec4){.d = {v.d[0], v.d[1], v.d[2], w}}; }
static inline vec3 vec3_from_vec4(vec4 v) { return (vec3){.d = {v.d[0], v.d[1], v.d[2]}}; }

static inline mat4 mat4_identity(void) {
	mat4 m = {.d = {0}};
	m.d[0] = m.d[5] = m.d[10] = m.d[15] = 1.0f;
	return m;
}
static inline mat4 mat4_scale(vec4 s) {
	mat4 m = {.d = {0}};
	m.d[0] = s.d[0];
	m.d[5] = s.d[1];
	m.d[10] = s.d[2];
	m.d[15] = s.d[3];
	return m;
}
static inline mat4 mat4_translate(vec3 t) {
	mat4 m = mat4_identity();
	m.d[12] = t.d[0];
	m.d[13] = t.d[1];
	m.d[14] = t.d[2];
	return m;
}
static inline mat4 mat4_mult(mat4 a, mat4 b) {
	mat4 r;
	for (int i = 0; i < 4; ++i) {
		int i4 = i * 4;
		r.d[i4 + 0] = a.d[i4 + 0] * b.d[0] + a.d[i4 + 1] * b.d[4] + a.d[i4 + 2] * b.d[8] + a.d[i4 + 3] * b.d[12];
		r.d[i4 + 1] = a.d[i4 + 0] * b.d[1] + a.d[i4 + 1] * b.d[5] + a.d[i4 + 2] * b.d[9] + a.d[i4 + 3] * b.d[13];
		r.d[i4 + 2] = a.d[i4 + 0] * b.d[2] + a.d[i4 + 1] * b.d[6] + a.d[i4 + 2] * b.d[10] + a.d[i4 + 3] * b.d[14];
		r.d[i4 + 3] = a.d[i4 + 0] * b.d[3] + a.d[i4 + 1] * b.d[7] + a.d[i4 + 2] * b.d[11] + a.d[i4 + 3] * b.d[15];
	}
	return r;
}
static inline vec4 mat4_mul_vec4(mat4 m, vec4 v) {
	return (vec4){.d = {m.d[0] * v.d[0] + m.d[1] * v.d[1] + m.d[2] * v.d[2] + m.d[3] * v.d[3],
						m.d[4] * v.d[0] + m.d[5] * v.d[1] + m.d[6] * v.d[2] + m.d[7] * v.d[3],
						m.d[8] * v.d[0] + m.d[9] * v.d[1] + m.d[10] * v.d[2] + m.d[11] * v.d[3],
						m.d[12] * v.d[0] + m.d[13] * v.d[1] + m.d[14] * v.d[2] + m.d[15] * v.d[3]}};
}
static inline mat4 mat4_perspective(f32 fov_deg, f32 aspect, f32 near, f32 far) {
	f32 f = 1.0f / tanf(fov_deg * (3.14159265358979323846f / 180.0f) / 2.0f);
	mat4 m = {.d = {0}};
	m.d[0] = f / aspect;
	m.d[5] = f;
	m.d[10] = (far + near) / (near - far);
	m.d[11] = -1.0f;
	m.d[14] = (2 * far * near) / (near - far);
	return m;
}
static inline mat4 mat4_lookat(vec3 eye, vec3 at, vec3 up) {
	vec3 f = vec3_normalize(vec3_sub(at, eye));
	vec3 s = vec3_normalize(vec3_cross(f, up));
	vec3 u = vec3_cross(s, f);
	mat4 m = mat4_identity();
	m.d[0] = s.d[0];
	m.d[4] = s.d[1];
	m.d[8] = s.d[2];
	m.d[1] = u.d[0];
	m.d[5] = u.d[1];
	m.d[9] = u.d[2];
	m.d[2] = -f.d[0];
	m.d[6] = -f.d[1];
	m.d[10] = -f.d[2];
	m.d[12] = -vec3_dot(s, eye);
	m.d[13] = -vec3_dot(u, eye);
	m.d[14] = vec3_dot(f, eye);
	return m;
}
static inline vec3 vec3_reflect(vec3 v, vec3 n) { return vec3_sub(v, vec3_scale(2.0f * vec3_dot(n, v), n)); }
static inline vec4 sphere_vs_sphere(vec4 s1, vec4 s2) {
	vec3 d = vec3_sub(vec3_from_vec4(s2), vec3_from_vec4(s1));
	f32 len = vec3_length(d);
	f32 pen = (s1.d[3] + s2.d[3] - len);
	if (pen <= 0 || len == 0)
		return (vec4){.d = {0, 0, 0, 0}};
	return vec4_from_vec3(vec3_scale(pen / len, d), pen);
}
static inline int box_vs_box_bool(aabb b1, aabb b2) {
	vec3 se = vec3_add(b1.e, b2.e);
	vec3 c1 = vec3_from_vec4(b1.c), c2 = vec3_from_vec4(b2.c);
	return (fabsf(c1.d[0] - c2.d[0]) <= se.d[0] && fabsf(c1.d[1] - c2.d[1]) <= se.d[1] && fabsf(c1.d[2] - c2.d[2]) <= se.d[2]);
}
static inline vec3 closest_point_aabb(aabb box, vec3 p) {
	vec3 min = vec3_sub(vec3_from_vec4(box.c), box.e);
	vec3 max = vec3_add(vec3_from_vec4(box.c), box.e);
	return vec3_clamp(p, min, max);
}
static inline vec4 sphere_vs_aabb(vec4 sph, aabb box) {
	vec3 closest = closest_point_aabb(box, vec3_from_vec4(sph));
	vec3 diff = vec3_sub(closest, vec3_from_vec4(sph));
	f32 d2 = vec3_dot(diff, diff);
	if (d2 <= sph.d[3] * sph.d[3]) {
		f32 len = sqrtf(d2);
		f32 pen = sph.d[3] - len;
		if (len > 0)
			return vec4_from_vec3(vec3_scale(pen / len, diff), pen);
		aabb virt = {.c = sph, .e = {.d = {sph.d[3], sph.d[3], sph.d[3]}}};
		return (vec4){.d = {box_vs_box_bool(virt, box), 0, 0, 0}};
	}
	return (vec4){.d = {0, 0, 0, 0}};
}

#define addv3 vec3_add
#define subv3 vec3_sub
#define addv4 vec4_add
#define subv4 vec4_sub
#define scalev3 vec3_scale
#define scalev4 vec4_scale
#define multvec3 vec3_mul
#define multvec4 vec4_mul
#define dotv3 vec3_dot
#define dotv4 vec4_dot
#define lengthv3 vec3_length
#define lengthv4 vec4_length
#define clampvec3 vec3_clamp
#define clampvec4 vec4_clamp
#define normalizev3 vec3_normalize
#define normalizev4 vec4_normalize
#define crossv3 vec3_cross
#define reflect vec3_reflect
#define upv3(v, w) vec4_from_vec3((v), (w))
#define downv4 vec3_from_vec4
#define scalemat4 mat4_scale
static inline mat4 identitymat4(void) { return mat4_identity(); }
static inline mat4 translate(vec3 t) { return mat4_translate(t); }
static inline mat4 multm4(mat4 a, mat4 b) { return mat4_mult(a, b); }
static inline vec4 mat4xvec4(mat4 m, vec4 v) { return mat4_mul_vec4(m, v); }
static inline mat4 perspective(f32 fov, f32 aspect, f32 near, f32 far) { return mat4_perspective(fov, aspect, near, far); }
static inline mat4 lookAt(vec3 eye, vec3 at, vec3 up) { return mat4_lookat(eye, at, up); }
static inline vec3 rotatev3(vec3 in, vec3 axis, f32 ang) {
	vec3 t1 = vec3_scale(cosf(ang), in);
	vec3 t2 = vec3_scale(sinf(ang), vec3_cross(axis, in));
	vec3 t3 = vec3_scale((1 - cosf(ang)) * vec3_dot(axis, in), axis);
	return vec3_add(t1, vec3_add(t2, t3));
}
static inline vec4 boxvbox(aabb b1, aabb b2) {
	vec4 ret = {.d = {0, 0, 0, 0}};
	vec3 sumextents = vec3_add(b1.e, b2.e);
	vec3 b1c = vec3_from_vec4(b1.c);
	vec3 b2c = vec3_from_vec4(b2.c);
	if (!(fabsf(b1c.d[0] - b2c.d[0]) <= sumextents.d[0] && fabsf(b1c.d[1] - b2c.d[1]) <= sumextents.d[1] && fabsf(b1c.d[2] - b2c.d[2]) <= sumextents.d[2]))
		return ret;
	vec3 axispen[2];
	vec3 b1min = vec3_sub(b1c, b1.e);
	vec3 b2min = vec3_sub(b2c, b2.e);
	vec3 b1max = vec3_add(b1c, b1.e);
	vec3 b2max = vec3_add(b2c, b2.e);
	axispen[0] = vec3_sub(b1max, b2min);
	axispen[1] = vec3_sub(b1min, b2max);
	ret.d[3] = fabsf(axispen[0].d[0]);
	ret.d[0] = axispen[0].d[0];
	for (int i = 1; i < 6; i++) {
		if (fabsf(axispen[i / 3].d[i % 3]) < fabsf(ret.d[3])) {
			ret = (vec4){.d = {0, 0, 0, axispen[i / 3].d[i % 3]}};
			ret.d[i % 3] = ret.d[3];
			ret.d[3] = fabsf(ret.d[3]);
		}
	}
	return ret;
}
#define boxvboxbool box_vs_box_bool
#define spherevsphere sphere_vs_sphere
#define spherevaabb sphere_vs_aabb
#define closestpointAABB closest_point_aabb
#endif
