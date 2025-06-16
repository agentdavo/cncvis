#ifndef API_H
#define API_H

#include "actor.h"
#include "assembly.h"
#include "camera.h"
#include "config.h"
#include "light.h"
#include "osd.h"
#include "utils.h"

#define ZGL_FB_WIDTH 640
#define ZGL_FB_HEIGHT 480

#define ORBIT_RADIUS 500.0f        // Distance from the origin
#define ORBIT_ELEVATION 250.0f     // Elevation above the XY plane
#define ORBIT_ROTATION_SPEED 20.0f // Speed in degrees per second

extern ZBuffer *globalFramebuffer;
extern ucncAssembly *globalScene;
extern ucncCamera *globalCamera;
extern ucncLight **globalLights;
extern int globalLightCount;

// Motion and scene control functions
int ucncUpdateMotionByName(const char *assemblyName, float value);
void ucncSetAllAssembliesToHome(ucncAssembly *assembly);
int ucncUpdateMotion(ucncAssembly *assembly, float value);
int ucncClearLimitWarning(const char *assemblyName);

// Z-buffer handling
void ucncSetZBufferDimensions(int width, int height);
const float *ucncGetZBufferOutput(void);
void ucncFrameReady(ZBuffer *framebuffer);

// load an xml config file
int ucncLoadNewConfiguration(const char *configFile);

void cncvis_handle_mouse_motion(int dx, int dy);
void cncvis_handle_mouse_wheel(int wheel_delta);

// Initialization and cleanup (if needed)
int cncvis_init(const char *configFile);
void cncvis_render(void);
void cncvis_cleanup();

#endif // API_H
