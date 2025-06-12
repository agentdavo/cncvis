#include "../include/GL/gl.h"
#include "../include/zbuffer.h"
#include "../src/gl_init.h"
#include "../src/gl_state.h"
#include "../src/gl_vertex.h"

int main(void) {
  ZBuffer *zb = ZB_open(16, 16, ZB_MODE_RGBA, 0);
  if (!zb)
    return 1;
  glInit(zb);
  GLfloat verts[] = {0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f};
  GLushort idx[] = {0, 1, 2};
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, verts);
  glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_SHORT, idx);
  GLenum err = glGetError();
  glDisableClientState(GL_VERTEX_ARRAY);
  glClose();
  ZB_close(zb);
  return err != GL_NO_ERROR;
}
