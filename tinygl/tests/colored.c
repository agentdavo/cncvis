#include "../include/zbuffer.h"
#include "../src/gl_init.h"
#include "../src/gl_state.h"
#include "../src/gl_utils.h"
#include "../src/gl_vertex.h"

int main(void) {
  ZBuffer *zb = ZB_open(32, 32, ZB_MODE_RGBA, 0);
  if (!zb)
    return 1;
  glInit(zb);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_TRIANGLES);
  glColor3f(1.f, 0.f, 0.f);
  glVertex3f(-1.f, -1.f, 0.f);
  glColor3f(0.f, 1.f, 0.f);
  glVertex3f(1.f, -1.f, 0.f);
  glColor3f(0.f, 0.f, 1.f);
  glVertex3f(0.f, 1.f, 0.f);
  glEnd();

  glFlush();
  GLenum err = glGetError();
  glClose();
  ZB_close(zb);
  return err != GL_NO_ERROR;
}
