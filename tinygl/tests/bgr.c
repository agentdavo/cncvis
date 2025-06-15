#include "../include/zbuffer.h"
#include "../src/gl_init.h"
#include "../src/gl_texture.h"
#include "../src/gl_vertex.h"

int main(void) {
  ZBuffer *zb = ZB_open(16, 16, ZB_MODE_RGBA, 0);
  if (!zb)
    return 1;
  glInit(zb);
  glEnable(GL_TEXTURE_2D);
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  unsigned char pix[12] = {0, 0, 255, 0, 255, 0, 255, 0, 0, 0, 0, 0};
  glTexImage2D(GL_TEXTURE_2D, 0, 3, 2, 2, 0, GL_BGR, GL_UNSIGNED_BYTE, pix);
  GLenum err = glGetError();
  glDeleteTextures(1, &tex);
  glClose();
  ZB_close(zb);
  return err != GL_NO_ERROR;
}
