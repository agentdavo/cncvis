#include "../include/GL/gl.h"
#include "../include/zbuffer.h"
#include "../src/gl_init.h"
#include "../src/gl_state.h"
#include "../src/gl_vertex.h"
#include "../src/zgl.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIN_X 64
#define WIN_Y 64
#define NUM_CUBES 1000
#define FRAMES 60

static double now_sec(void) {
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void draw_cube(void) {
  static const GLfloat verts[24][3] = {
      {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f},  {0.5f, 0.5f, -0.5f},
      {-0.5f, 0.5f, -0.5f},  {-0.5f, -0.5f, 0.5f},  {0.5f, -0.5f, 0.5f},
      {0.5f, 0.5f, 0.5f},    {-0.5f, 0.5f, 0.5f},   {-0.5f, -0.5f, -0.5f},
      {-0.5f, 0.5f, -0.5f},  {-0.5f, 0.5f, 0.5f},   {-0.5f, -0.5f, 0.5f},
      {0.5f, -0.5f, -0.5f},  {0.5f, 0.5f, -0.5f},   {0.5f, 0.5f, 0.5f},
      {0.5f, -0.5f, 0.5f},   {-0.5f, -0.5f, 0.5f},  {0.5f, -0.5f, 0.5f},
      {0.5f, -0.5f, -0.5f},  {-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, 0.5f},
      {0.5f, 0.5f, 0.5f},    {0.5f, 0.5f, -0.5f},   {-0.5f, 0.5f, -0.5f}};
  static const GLfloat tex[4][2] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  glBegin(GL_QUADS);
  for (int f = 0; f < 6; ++f) {
    for (int v = 0; v < 4; ++v) {
      glTexCoord2fv((GLfloat *)tex[v]);
      glVertex3fv((GLfloat *)verts[f * 4 + v]);
    }
  }
  glEnd();
}

int main(void) {
  srand(0);
  float pos[NUM_CUBES][3];
  float rot[NUM_CUBES];
  for (int i = 0; i < NUM_CUBES; ++i) {
    pos[i][0] = ((rand() & 0x7fff) / 16384.0f - 0.5f) * 10.0f;
    pos[i][1] = ((rand() & 0x7fff) / 16384.0f - 0.5f) * 10.0f;
    pos[i][2] = -5.0f - ((rand() & 0x7fff) / 16384.0f) * 15.0f;
    rot[i] = (rand() & 0x7fff) / 16384.0f * 360.0f;
  }

  ZBuffer *zb = ZB_open(WIN_X, WIN_Y, ZB_MODE_RGBA, 0);
  if (!zb)
    return 1;
  glInit(zb);
  glViewport(0, 0, WIN_X, WIN_Y);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  float fov = 45.f * (float)M_PI / 180.f;
  float t = tanf(fov / 2.f);
  glFrustum(-t, t, -t, t, 1.f, 50.f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.f, 0.f, -10.f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  unsigned char pixels[12] = {255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 0};
  glTexImage2D(GL_TEXTURE_2D, 0, 3, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  for (int mode = 1; mode >= 0; --mode) {
    tgl_threads_enabled = mode;
    double start = now_sec();
    for (int f = 0; f < FRAMES; ++f) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      for (int i = 0; i < NUM_CUBES; ++i) {
        glPushMatrix();
        glTranslatef(pos[i][0], pos[i][1], pos[i][2]);
        glRotatef(rot[i] + f, 1.f, 1.f, 1.f);
        draw_cube();
        glPopMatrix();
      }
      glFinish();
    }
    double fps = FRAMES / (now_sec() - start);
    printf("Threads %s: %.1f fps\n", mode ? "on" : "off", fps);
  }

  glDeleteTextures(1, &tex);
  glClose();
  ZB_close(zb);
  return 0;
}
