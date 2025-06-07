#include "../include/GL/gl.h"
#include "../include/zbuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static double now_sec(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec * 1e-9;
}

typedef void (*bench_fn)(int loops);
static FILE *log_file;

static void run_bench(const char *name, bench_fn fn, int loops) {
  double start = now_sec();
  fn(loops);
  double end = now_sec();
  fprintf(log_file, "%-20s %6d calls in %8.3f ms (%.0f/s)\n", name, loops,
          (end - start) * 1000.0, loops / (end - start));
  printf("%-20s %6d calls in %8.3f ms (%.0f/s)\n", name, loops,
         (end - start) * 1000.0, loops / (end - start));
}

static void bench_clear(int loops) {
  for (int i = 0; i < loops; ++i)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void bench_triangles(int loops) {
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-0.5f, -0.5f, 0.f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.5f, -0.5f, 0.f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.5f, 0.f);
    glEnd();
  }
}

static PIXEL *pixel_buf;
static GLubyte *texbuf;
static GLuint tex;

/* Icosahedron geometry for a more complex benchmark */
static const float PHI = 1.6180339887498948482f;
static const GLfloat ico_vertices[12][3] = {
    {-1, PHI, 0}, {1, PHI, 0}, {-1, -PHI, 0}, {1, -PHI, 0},
    {0, -1, PHI}, {0, 1, PHI}, {0, -1, -PHI}, {0, 1, -PHI},
    {PHI, 0, -1}, {PHI, 0, 1}, {-PHI, 0, -1}, {-PHI, 0, 1},
};
static const GLushort ico_indices[60] = {
    0, 11, 5,  0, 5,  1, 0, 1, 7, 0, 7,  10, 0, 10, 11, 1, 5, 9, 5, 11,
    4, 11, 10, 2, 10, 7, 6, 7, 1, 8, 3,  9,  4, 3,  4,  2, 3, 2, 6, 3,
    6, 8,  3,  8, 9,  4, 9, 5, 2, 4, 11, 6,  2, 10, 8,  6, 7, 9, 8, 1,
};

static void bench_tex_triangles(int loops) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, tex);
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.f, 0.f);
    glVertex3f(-0.5f, -0.5f, 0.f);
    glTexCoord2f(1.f, 0.f);
    glVertex3f(0.5f, -0.5f, 0.f);
    glTexCoord2f(0.5f, 1.f);
    glVertex3f(0.f, 0.5f, 0.f);
    glEnd();
  }
  glDisable(GL_TEXTURE_2D);
}

static void bench_drawpixels(int loops) {
  for (int i = 0; i < loops; ++i) {
    glRasterPos2f(-1.f, 1.f);
    glDrawPixels(256, 256, GL_RGB,
#if TGL_FEATURE_RENDER_BITS == 32
                 GL_UNSIGNED_INT,
#else
                 GL_UNSIGNED_SHORT,
#endif
                 pixel_buf);
  }
}

static void bench_teximage(int loops) {
  for (int i = 0; i < loops; ++i) {
    glTexImage2D(GL_TEXTURE_2D, 0, 3, TGL_FEATURE_TEXTURE_DIM,
                 TGL_FEATURE_TEXTURE_DIM, 0, GL_RGB, GL_UNSIGNED_BYTE, texbuf);
  }
}

static void bench_bindtex(int loops) {
  GLuint tex2;
  glGenTextures(1, &tex2);
  for (int i = 0; i < loops; ++i) {
    glBindTexture(GL_TEXTURE_2D, tex);
    glBindTexture(GL_TEXTURE_2D, tex2);
  }
  glDeleteTextures(1, &tex2);
}

static void bench_blendfunc(int loops) {
  for (int i = 0; i < loops; ++i) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
}

static void bench_enable(int loops) {
  for (int i = 0; i < loops; ++i) {
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_2D);
  }
}

static void bench_viewport(int loops) {
  for (int i = 0; i < loops; ++i) {
    glViewport(0, 0, 256, 256);
  }
}

static void bench_copytex(int loops) {
  for (int i = 0; i < loops; ++i) {
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, TGL_FEATURE_TEXTURE_DIM,
                     TGL_FEATURE_TEXTURE_DIM, 0);
  }
}

static void bench_icosahedron(int loops) {
  for (int i = 0; i < loops; ++i) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glRotatef((float)i, 0.3f, 1.f, 0.2f);
    glBegin(GL_TRIANGLES);
    for (int f = 0; f < 60; f++) {
      const GLfloat *v = ico_vertices[ico_indices[f]];
      glVertex3fv((GLfloat *)v);
    }
    glEnd();
    glPopMatrix();
  }
}

int main(int argc, char **argv) {
  int loops = 1000;
  int winX = 256;
  int winY = 256;
  if (argc > 1)
    loops = atoi(argv[1]);
  log_file = fopen("benchmark.log", "w");
  ZBuffer *zb = ZB_open(winX, winY,
#if TGL_FEATURE_RENDER_BITS == 32
                        ZB_MODE_RGBA,
#else
                        ZB_MODE_5R6G5B,
#endif
                        0);
  if (!zb) {
    fprintf(stderr, "ZB_open failed\n");
    return 1;
  }
  glInit(zb);
  glViewport(0, 0, winX, winY);
  glClearColor(0, 0, 0, 1);

  pixel_buf = malloc(winX * winY * sizeof(PIXEL));
  for (int i = 0; i < winX * winY; ++i)
    pixel_buf[i] = 0xffffffffu;
  texbuf = malloc(TGL_FEATURE_TEXTURE_DIM * TGL_FEATURE_TEXTURE_DIM * 3);
  memset(texbuf, 0xff, TGL_FEATURE_TEXTURE_DIM * TGL_FEATURE_TEXTURE_DIM * 3);
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, TGL_FEATURE_TEXTURE_DIM,
               TGL_FEATURE_TEXTURE_DIM, 0, GL_RGB, GL_UNSIGNED_BYTE, texbuf);

  run_bench("glClear", bench_clear, loops);
  run_bench("Triangles", bench_triangles, loops);
  run_bench("TexturedTriangles", bench_tex_triangles, loops);
  run_bench("glDrawPixels", bench_drawpixels, loops);
  run_bench("glCopyTexImage2D", bench_copytex, loops);
  run_bench("glTexImage2D", bench_teximage, loops);
  run_bench("glBindTexture", bench_bindtex, loops);
  run_bench("glBlendFunc", bench_blendfunc, loops);
  run_bench("glEnable/Disable", bench_enable, loops);
  run_bench("glViewport", bench_viewport, loops);
  run_bench("Icosahedron", bench_icosahedron, loops);

  free(pixel_buf);
  glDeleteTextures(1, &tex);
  free(texbuf);

  fclose(log_file);

  glClose();
  ZB_close(zb);
  return 0;
}
