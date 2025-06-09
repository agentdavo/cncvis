#include "../include/zbuffer.h"
#include "../src/gl_init.h"
#include "../src/gl_utils.h"
#include <stdio.h>

int main(void) {
  ZBuffer *zb = ZB_open(16, 16, ZB_MODE_RGBA, 0);
  if (!zb)
    return 1;
  glInit(zb);
  glBegin(GL_TRIANGLES);
  glVertex3f(0.f, 0.f, 0.f);
  glVertex3f(0.f, 1.f, 0.f);
  glVertex3f(1.f, 0.f, 0.f);
  glEnd();
  GLenum err = glGetError();
  glClose();
  ZB_close(zb);
  return err != GL_NO_ERROR;
}
