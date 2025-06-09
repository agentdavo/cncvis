#include "../include/GL/gl.h"
#include "../include/zbuffer.h"
#include "../src/zgl.h"
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

static void bench_points(int loops) {
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_POINTS);
    for (float x = -0.9f; x <= 0.9f; x += 0.025f)
      for (float y = -0.9f; y <= 0.9f; y += 0.025f)
        glVertex2f(x, y);
    glEnd();
  }
}

static void bench_lines(int loops) {
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_LINES);
    for (float x = -0.9f; x <= 0.9f; x += 0.1f) {
      glVertex2f(x, -0.9f);
      glVertex2f(x, 0.9f);
      glVertex2f(-0.9f, x);
      glVertex2f(0.9f, x);
    }
    glEnd();
  }
}

static void bench_wireframe(int loops) {
#ifdef GL_POLYGON_MODE
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_QUADS);
    glVertex2f(-1.f, -1.f);
    glVertex2f(1.f, -1.f);
    glVertex2f(1.f, 1.f);
    glVertex2f(-1.f, 1.f);
    glEnd();
  }
#ifdef GL_POLYGON_MODE
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}

static PIXEL *pixel_buf;
static GLubyte *texbuf, *texbuf_small;
static GLuint tex, tex2;

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

static void bench_triangle_strip(int loops) {
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-0.5f, -0.5f, 0.f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(-0.5f, 0.5f, 0.f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.5f, -0.5f, 0.f);
    glColor3f(1.f, 1.f, 0.f);
    glVertex3f(0.5f, 0.5f, 0.f);
    glEnd();
  }
}

static void bench_blended_triangles(int loops) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_TRIANGLES);
    glColor4f(1.f, 0.f, 0.f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.f);
    glColor4f(0.f, 1.f, 0.f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.f);
    glColor4f(0.f, 0.f, 1.f, 0.5f);
    glVertex3f(0.f, 0.5f, 0.f);
    glEnd();
  }
  glDisable(GL_BLEND);
}

static void bench_depth_triangles(int loops) {
  glEnable(GL_DEPTH_TEST);
  for (int i = 0; i < loops; ++i) {
    float z = -1.0f + (float)i * 2.0f / loops;
    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-0.5f, -0.5f, z);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.5f, -0.5f, z);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.5f, z);
    glEnd();
  }
  glDisable(GL_DEPTH_TEST);
}

static void bench_gouraud_fill(int loops) {
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_QUADS);
    glColor3f(1.f, 0.f, 0.f);
    glVertex2f(-1.f, -1.f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex2f(1.f, -1.f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex2f(1.f, 1.f);
    glColor3f(1.f, 1.f, 1.f);
    glVertex2f(-1.f, 1.f);
    glEnd();
  }
}

static void bench_tex_fill(int loops) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, tex);
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_QUADS);
    glColor3f(1.f, 1.f, 1.f);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(-1.f, -1.f);
    glTexCoord2f(1.f, 0.f);
    glVertex2f(1.f, -1.f);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(1.f, 1.f);
    glTexCoord2f(0.f, 1.f);
    glVertex2f(-1.f, 1.f);
    glEnd();
  }
  glDisable(GL_TEXTURE_2D);
}

static void bench_tex_fill_blend(int loops) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, tex);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_QUADS);
    glColor4f(1.f, 1.f, 1.f, 0.5f);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(-1.f, -1.f);
    glTexCoord2f(1.f, 0.f);
    glVertex2f(1.f, -1.f);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(1.f, 1.f);
    glTexCoord2f(0.f, 1.f);
    glVertex2f(-1.f, 1.f);
    glEnd();
  }
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}

static void bench_smalltex_fill(int loops) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, tex2);
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_QUADS);
    glColor3f(1.f, 1.f, 1.f);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(-1.f, -1.f);
    glTexCoord2f(1.f, 0.f);
    glVertex2f(1.f, -1.f);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(1.f, 1.f);
    glTexCoord2f(0.f, 1.f);
    glVertex2f(-1.f, 1.f);
    glEnd();
  }
  glDisable(GL_TEXTURE_2D);
}

static void bench_alpha_test(int loops) {
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.5f);
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_TRIANGLES);
    glColor4f(1.f, 0.f, 0.f, 0.4f);
    glVertex3f(-0.5f, -0.5f, 0.f);
    glColor4f(0.f, 1.f, 0.f, 0.6f);
    glVertex3f(0.5f, -0.5f, 0.f);
    glColor4f(0.f, 0.f, 1.f, 0.8f);
    glVertex3f(0.f, 0.5f, 0.f);
    glEnd();
  }
  glDisable(GL_ALPHA_TEST);
}

static void bench_scissor(int loops) {
  glEnable(GL_SCISSOR_TEST);
  glScissor(64, 64, 128, 128);
  for (int i = 0; i < loops; ++i) {
    glClear(GL_COLOR_BUFFER_BIT);
  }
  glDisable(GL_SCISSOR_TEST);
}

static void bench_polygon_offset(int loops) {
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.f, 1.f);
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_QUADS);
    glVertex3f(-1.f, -1.f, 0.f);
    glVertex3f(1.f, -1.f, 0.f);
    glVertex3f(1.f, 1.f, 0.f);
    glVertex3f(-1.f, 1.f, 0.f);
    glEnd();
  }
  glDisable(GL_POLYGON_OFFSET_FILL);
}

static void bench_vertex_arrays(int loops) {
  GLfloat verts[9] = {-0.5f, -0.5f, 0.f, 0.5f, -0.5f, 0.f, 0.f, 0.5f, 0.f};
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, verts);
  for (int i = 0; i < loops; ++i) {
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }
  glDisableClientState(GL_VERTEX_ARRAY);
}

static void bench_stencil(int loops) {
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_QUADS);
    glVertex2f(-1.f, -1.f);
    glVertex2f(1.f, -1.f);
    glVertex2f(1.f, 1.f);
    glVertex2f(-1.f, 1.f);
    glEnd();
  }
  glDisable(GL_STENCIL_TEST);
}

static void bench_point_sprites(int loops) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, tex2);
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_POINTS);
    for (float x = -0.8f; x <= 0.8f; x += 0.08f)
      for (float y = -0.8f; y <= 0.8f; y += 0.08f) {
        glTexCoord2f((x + 1.0f) / 2.0f, (y + 1.0f) / 2.0f);
        glVertex2f(x, y);
      }
    glEnd();
  }
  glDisable(GL_TEXTURE_2D);
}

static void bench_multitex_sim(int loops) {
  glEnable(GL_TEXTURE_2D);
  for (int i = 0; i < loops; ++i) {
    glBindTexture(GL_TEXTURE_2D, tex);
    glBegin(GL_QUADS);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(-1.f, -1.f);
    glTexCoord2f(1.f, 0.f);
    glVertex2f(1.f, -1.f);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(1.f, 1.f);
    glTexCoord2f(0.f, 1.f);
    glVertex2f(-1.f, 1.f);
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glColor4f(1, 1, 1, 0.5f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(-1.f, -1.f);
    glTexCoord2f(1.f, 0.f);
    glVertex2f(1.f, -1.f);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(1.f, 1.f);
    glTexCoord2f(0.f, 1.f);
    glVertex2f(-1.f, 1.f);
    glEnd();
    glDisable(GL_BLEND);
  }
  glDisable(GL_TEXTURE_2D);
}

static void bench_fillrate_overdraw(int loops) {
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  int layers = 32;
  for (int i = 0; i < loops; ++i) {
    for (int l = 0; l < layers; ++l) {
      float z = -1.0f + 2.0f * (float)l / (float)(layers - 1);
      glBegin(GL_QUADS);
      glColor3f((float)l / layers, 1.0f - (float)l / layers, 0.5f);
      glVertex3f(-1.f, -1.f, z);
      glColor3f(1.0f, 0.2f, (float)l / layers);
      glVertex3f(1.f, -1.f, z);
      glColor3f(0.2f, (float)l / layers, 1.0f);
      glVertex3f(1.f, 1.f, z);
      glColor3f((float)l / layers, 0.7f, 1.0f - (float)l / layers);
      glVertex3f(-1.f, 1.f, z);
      glEnd();
    }
  }
}

static void bench_lighting(int loops) {
#if defined(GL_LIGHTING) && defined(GL_LIGHT0)
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  GLfloat lightpos[] = {0.f, 0.f, 1.f, 0.f};
  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
  GLfloat diffuse[] = {1.f, 0.6f, 0.2f, 1.f};
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_TRIANGLES);
    glNormal3f(0, 0, 1);
    glVertex3f(-0.5f, -0.5f, 0.f);
    glNormal3f(0, 0, 1);
    glVertex3f(0.5f, -0.5f, 0.f);
    glNormal3f(0, 0, 1);
    glVertex3f(0.f, 0.5f, 0.f);
    glEnd();
  }
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);
#else
  bench_triangles(loops);
#endif
}

static void bench_fog(int loops) {
#ifdef GL_FOG
  glEnable(GL_FOG);
  GLfloat fogColor[4] = {0.6f, 0.6f, 0.6f, 1.f};
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogf(GL_FOG_START, -1.f);
  glFogf(GL_FOG_END, 1.f);
  for (int i = 0; i < loops; ++i) {
    float z = -1.0f + (float)i * 2.0f / loops;
    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-0.5f, -0.5f, z);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.5f, -0.5f, z);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.5f, z);
    glEnd();
  }
  glDisable(GL_FOG);
#else
  bench_triangles(loops);
#endif
}

static void bench_logic_op(int loops) {
#ifdef GL_COLOR_LOGIC_OP
  glEnable(GL_COLOR_LOGIC_OP);
  glLogicOp(GL_XOR);
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_QUADS);
    glColor3f(1, 0, 0);
    glVertex2f(-1, -1);
    glColor3f(0, 1, 0);
    glVertex2f(1, -1);
    glColor3f(0, 0, 1);
    glVertex2f(1, 1);
    glColor3f(1, 1, 0);
    glVertex2f(-1, 1);
    glEnd();
  }
  glDisable(GL_COLOR_LOGIC_OP);
#else
  bench_gouraud_fill(loops);
#endif
}

static void bench_grid_mesh(int loops) {
  int nx = 32, ny = 32;
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_QUADS);
    for (int x = 0; x < nx; ++x) {
      for (int y = 0; y < ny; ++y) {
        float fx = -1.f + 2.f * (float)x / nx;
        float fy = -1.f + 2.f * (float)y / ny;
        float fx1 = -1.f + 2.f * (float)(x + 1) / nx;
        float fy1 = -1.f + 2.f * (float)(y + 1) / ny;
        glColor3f(fx, fy, 1.f - fx);
        glVertex2f(fx, fy);
        glVertex2f(fx1, fy);
        glVertex2f(fx1, fy1);
        glVertex2f(fx, fy1);
      }
    }
    glEnd();
  }
}

static void bench_point_cloud(int loops) {
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_POINTS);
    for (int p = 0; p < 4096; ++p) {
      float x = ((rand() & 0x7FFF) / 16383.f) * 2.f - 1.f;
      float y = ((rand() & 0x7FFF) / 16383.f) * 2.f - 1.f;
      glColor3f(x, y, 1.f - x * y);
      glVertex2f(x, y);
    }
    glEnd();
  }
}

static void bench_driver_overhead(int loops) {
  for (int i = 0; i < loops; ++i) {
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(-0.5f, -0.5f);
    glColor3f(0, 1, 0);
    glVertex2f(0.5f, -0.5f);
    glColor3f(0, 0, 1);
    glVertex2f(0.0f, 0.5f);
    glEnd();
  }
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

static void run_all(int loops) {
  run_bench("glClear", bench_clear, loops);
  run_bench("Triangles", bench_triangles, loops);
  run_bench("TriangleStrip", bench_triangle_strip, loops);
  run_bench("TexturedTriangles", bench_tex_triangles, loops);
  run_bench("BlendedTriangles", bench_blended_triangles, loops);
  run_bench("DepthTestedTriangles", bench_depth_triangles, loops);
  run_bench("GouraudFill", bench_gouraud_fill, loops);
  run_bench("TexturedFill", bench_tex_fill, loops);
  run_bench("TexFill_Blend", bench_tex_fill_blend, loops);
  run_bench("SmallTexFill", bench_smalltex_fill, loops);
  run_bench("AlphaTest", bench_alpha_test, loops);
  run_bench("Scissor", bench_scissor, loops);
  run_bench("PolygonOffset", bench_polygon_offset, loops);
  run_bench("VertexArrays", bench_vertex_arrays, loops);
  run_bench("Stencil", bench_stencil, loops);
  run_bench("PointSprites", bench_point_sprites, loops);
  run_bench("MultiTexSim", bench_multitex_sim, loops);
  run_bench("OverdrawFillrate", bench_fillrate_overdraw, loops);
  run_bench("Lighting", bench_lighting, loops);
  run_bench("Fog", bench_fog, loops);
  run_bench("LogicOp", bench_logic_op, loops);
  run_bench("GridMesh", bench_grid_mesh, loops);
  run_bench("PointCloud", bench_point_cloud, loops);
  run_bench("glDrawPixels", bench_drawpixels, loops);
  run_bench("glCopyTexImage2D", bench_copytex, loops);
  run_bench("glTexImage2D", bench_teximage, loops);
  run_bench("glBindTexture", bench_bindtex, loops);
  run_bench("glBlendFunc", bench_blendfunc, loops);
  run_bench("glEnable/Disable", bench_enable, loops);
  run_bench("glViewport", bench_viewport, loops);
  run_bench("Icosahedron", bench_icosahedron, loops);
  run_bench("DriverOverhead", bench_driver_overhead, loops);
}

int main(int argc, char **argv) {
  int loops = 10000;
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
  texbuf_small = malloc(32 * 32 * 3);
  memset(texbuf, 0xff, TGL_FEATURE_TEXTURE_DIM * TGL_FEATURE_TEXTURE_DIM * 3);
  memset(texbuf_small, 0xaa, 32 * 32 * 3);
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, TGL_FEATURE_TEXTURE_DIM,
               TGL_FEATURE_TEXTURE_DIM, 0, GL_RGB, GL_UNSIGNED_BYTE, texbuf);

  glGenTextures(1, &tex2);
  glBindTexture(GL_TEXTURE_2D, tex2);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE,
               texbuf_small);

  printf("\n== Non-threaded ==\n");
  tgl_threads_enabled = 0;
  run_all(loops);
  printf("\n== Threaded ==\n");
  tgl_threads_enabled = 1;
  run_all(loops);

  free(pixel_buf);
  glDeleteTextures(1, &tex);
  glDeleteTextures(1, &tex2);
  free(texbuf);
  free(texbuf_small);

  fclose(log_file);

  glClose();
  ZB_close(zb);
  return 0;
}
