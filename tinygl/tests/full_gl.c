#include "../include/zbuffer.h"
#include "../include/zfeatures.h"
#include "../src/gl_init.h"
#include "../src/gl_lighting.h"
#include "../src/gl_state.h"
#include "../src/gl_texture.h"
#include "../src/gl_utils.h"
#include "../src/gl_vertex.h"
#include <stdio.h>

#define CHECK(call)                                                            \
  do {                                                                         \
    call;                                                                      \
    GLenum e = glGetError();                                                   \
    if (e != GL_NO_ERROR) {                                                    \
      printf("%s -> %d\n", #call, e);                                          \
      failures++;                                                              \
    }                                                                          \
  } while (0)

int main(void) {
  ZBuffer *zb = ZB_open(32, 32, ZB_MODE_RGBA, 0);
  if (!zb)
    return 1;
  glInit(zb);

  int failures = 0;

  (void)glGetString(GL_VERSION);
  CHECK(glGetError());

  GLint iv[4];
  CHECK(glGetIntegerv(GL_VIEWPORT, iv));
  GLfloat fv[16];
  CHECK(glGetFloatv(GL_MODELVIEW_MATRIX, fv));

  CHECK(glEnable(GL_DEPTH_TEST));
  CHECK(glDisable(GL_LIGHTING));
  CHECK(glCullFace(GL_BACK));
  CHECK(glFrontFace(GL_CCW));
  CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  CHECK(glDepthFunc(GL_LEQUAL));
  CHECK(glDepthMask(GL_TRUE));
  CHECK(glAlphaFunc(GL_ALWAYS, 0.f));
  CHECK(glShadeModel(GL_SMOOTH));
  CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
  CHECK(glPolygonOffset(1.f, 1.f));
  CHECK(glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST));

  CHECK(glMatrixMode(GL_MODELVIEW));
  CHECK(glLoadIdentity());
  CHECK(glPushMatrix());
  CHECK(glTranslatef(0.f, 0.f, -1.f));
  CHECK(glRotatef(45.f, 0.f, 1.f, 0.f));
  CHECK(glScalef(1.f, 1.f, 1.f));
  CHECK(glPopMatrix());
  GLfloat mat[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  CHECK(glLoadMatrixf(mat));
  CHECK(glMatrixMode(GL_PROJECTION));
  CHECK(glFrustum(-1, 1, -1, 1, 1, 10));
  CHECK(glOrtho(-1, 1, -1, 1, 1, 10));
  CHECK(glViewport(0, 0, 32, 32));

  GLfloat verts[9] = {0, 0, 0, 1, 0, 0, 0, 1, 0};
  GLfloat tex[6] = {0, 0, 1, 0, 0, 1};
  GLfloat cols[12] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1};
  GLfloat norms[9] = {0, 0, 1, 0, 0, 1, 0, 0, 1};

  CHECK(glEnableClientState(GL_VERTEX_ARRAY));
  CHECK(glVertexPointer(3, GL_FLOAT, 0, verts));
  CHECK(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
  CHECK(glTexCoordPointer(2, GL_FLOAT, 0, tex));
  CHECK(glEnableClientState(GL_COLOR_ARRAY));
  CHECK(glColorPointer(4, GL_FLOAT, 0, cols));
  CHECK(glEnableClientState(GL_NORMAL_ARRAY));
  CHECK(glNormalPointer(GL_FLOAT, 0, norms));

  CHECK(glBegin(GL_TRIANGLES));
  CHECK(glColor3f(1.f, 0.f, 0.f));
  CHECK(glNormal3f(0.f, 0.f, 1.f));
  CHECK(glTexCoord2f(0.f, 0.f));
  CHECK(glVertex3f(0.f, 0.f, 0.f));
  CHECK(glColor4f(0.f, 1.f, 0.f, 1.f));
  CHECK(glTexCoord2f(1.f, 0.f));
  CHECK(glVertex2f(1.f, 0.f));
  CHECK(glTexCoord2f(0.f, 1.f));
  CHECK(glVertex3f(0.f, 1.f, 0.f));
  CHECK(glEnd());

  CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));
  GLuint indices[3] = {0, 1, 2};
  CHECK(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, indices));

  GLuint texid;
  CHECK(glGenTextures(1, &texid));
  CHECK(glBindTexture(GL_TEXTURE_2D, texid));
  const int dim = TGL_FEATURE_TEXTURE_DIM;
  GLubyte *pixels = malloc(dim * dim * 3);
  if (!pixels) {
    return 1;
  }
  memset(pixels, 255, dim * dim * 3);
  CHECK(glTexImage2D(GL_TEXTURE_2D, 0, 3, dim, dim, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     pixels));
  CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dim, dim, GL_RGB,
                        GL_UNSIGNED_BYTE, pixels));
  CHECK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  CHECK(glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE));
  CHECK(glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE));
  CHECK(glDeleteTextures(1, &texid));
  free(pixels);

  CHECK(glClearColor(0.f, 0.f, 0.f, 1.f));
  CHECK(glClearDepth(1.f));
  CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  CHECK(glDrawBuffer(GL_FRONT));
  CHECK(glReadBuffer(GL_FRONT));
  GLuint px;
  CHECK(glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, &px));

  float light_v[4] = {0, 0, 1, 0};
  CHECK(glLightfv(GL_LIGHT0, GL_POSITION, light_v));
  CHECK(glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_v));
  CHECK(glMaterialfv(GL_FRONT, GL_AMBIENT, light_v));
  CHECK(glColorMaterial(GL_FRONT, GL_DIFFUSE));

  CHECK(glFogf(GL_FOG_DENSITY, 0.5f));
  CHECK(glFogi(GL_FOG_MODE, GL_EXP));
  CHECK(glFogfv(GL_FOG_COLOR, light_v));

  CHECK(glPointSize(1.f));
  CHECK(glLineWidth(1.f));
  CHECK(glFlush());
  CHECK(glFinish());

  glClose();
  ZB_close(zb);
  if (failures) {
    printf("Failures: %d\n", failures);
  }
  return failures ? 1 : 0;
}
