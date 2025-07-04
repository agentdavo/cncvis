#include "../include/zbuffer.h"
#include "../src/gl_init.h"
#include "../src/gl_state.h"
#include "../src/gl_texture.h"
#include "../src/gl_utils.h"
#include "../src/gl_vertex.h"

int main(void) {
  ZBuffer *zb = ZB_open(32, 32, ZB_MODE_RGBA, 0);
  if (!zb)
    return 1;
  glInit(zb);
  glEnable(GL_TEXTURE_2D);
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  unsigned char pixels[12] = {
      0xff, 0xff, 0xff, // white
      0xff, 0x00, 0x00, // red
      0x00, 0xff, 0x00, // green
      0x00, 0x00, 0xff  // blue
  };
  glTexImage2D(GL_TEXTURE_2D, 0, 3, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  glBegin(GL_TRIANGLES);
  glTexCoord2f(0.f, 0.f);
  glVertex3f(-1.f, -1.f, 0.f);
  glTexCoord2f(1.f, 0.f);
  glVertex3f(1.f, -1.f, 0.f);
  glTexCoord2f(0.5f, 1.f);
  glVertex3f(0.f, 1.f, 0.f);
  glEnd();

  glFlush();
  GLenum err = glGetError();
  glDeleteTextures(1, &tex);
  glClose();
  ZB_close(zb);
  return err != GL_NO_ERROR;
}
