#include "api.h"
#include "assembly.h"
#include <assert.h>

ZBuffer *globalFramebuffer = NULL;
ucncAssembly *globalScene = NULL;
ucncCamera *globalCamera = NULL;
ucncLight **globalLights = NULL;
int globalLightCount = 0;

static void test_init_and_motion(void) {
  int rc = cncvis_init("../machines/meca500/config.xml");
  assert(rc == 0);
  assert(globalScene != NULL);
  assert(globalCamera != NULL);

  ucncAssembly *link1 = findAssemblyByName(globalScene, "link1");
  assert(link1 != NULL);
  float prev_rot = link1->rotationZ;
  ucncUpdateMotionByName("link1", 5.0f);
  assert(link1->rotationZ == prev_rot + 5.0f);

  cncvis_render();
  cncvis_cleanup();
}

int main(void) {
  test_init_and_motion();
  return 0;
}