#include "api.h"

// Motion handling function by assembly name
int ucncUpdateMotionByName(const char *assemblyName, float value) {
  ucncAssembly *assembly = findAssemblyByName(globalScene, assemblyName);

  if (!assembly) {
    fprintf(stderr, "Assembly '%s' not found.\n", assemblyName);
    return -1;
  }

  if (strcmp(assembly->motionType, MOTION_TYPE_NONE) == 0) {
    fprintf(stderr, "Assembly '%s' has no motion defined.\n", assemblyName);
    return -1;
  }

  return ucncUpdateMotion(assembly, value);
}

// Update motion for a given assembly
int ucncUpdateMotion(ucncAssembly *assembly, float value) {
  if (!assembly || strcmp(assembly->motionType, MOTION_TYPE_NONE) == 0) {
    return -1;
  }

  if (assembly->limitTriggered) {
    return -1;
  }

  if (assembly->invertMotion) {
    value = -value;
  }

  if (strcmp(assembly->motionType, MOTION_TYPE_ROTATIONAL) == 0) {
    switch (assembly->motionAxis) {
    case AXIS_X: {
      float newVal = assembly->rotationX + value;
      if (newVal < assembly->minRot - 1.0f ||
          newVal > assembly->maxRot + 1.0f) {
        assembly->limitTriggered = 1;
        return -1;
      }
      assembly->rotationX = newVal;
    } break;
    case AXIS_Y: {
      float newVal = assembly->rotationY + value;
      if (newVal < assembly->minRot - 1.0f ||
          newVal > assembly->maxRot + 1.0f) {
        assembly->limitTriggered = 1;
        return -1;
      }
      assembly->rotationY = newVal;
    } break;
    case AXIS_Z: {
      float newVal = assembly->rotationZ + value;
      if (newVal < assembly->minRot - 1.0f ||
          newVal > assembly->maxRot + 1.0f) {
        assembly->limitTriggered = 1;
        return -1;
      }
      assembly->rotationZ = newVal;
    } break;
    default:
      fprintf(stderr, "Invalid motion axis '%c' for rotational motion.\n",
              assembly->motionAxis);
      break;
    }
  } else if (strcmp(assembly->motionType, MOTION_TYPE_LINEAR) == 0) {
    switch (assembly->motionAxis) {
    case AXIS_X: {
      float newVal = assembly->positionX + value;
      if (newVal < assembly->minPos - 1.0f ||
          newVal > assembly->maxPos + 1.0f) {
        assembly->limitTriggered = 1;
        return -1;
      }
      assembly->positionX = newVal;
    } break;
    case AXIS_Y: {
      float newVal = assembly->positionY + value;
      if (newVal < assembly->minPos - 1.0f ||
          newVal > assembly->maxPos + 1.0f) {
        assembly->limitTriggered = 1;
        return -1;
      }
      assembly->positionY = newVal;
    } break;
    case AXIS_Z: {
      float newVal = assembly->positionZ + value;
      if (newVal < assembly->minPos - 1.0f ||
          newVal > assembly->maxPos + 1.0f) {
        assembly->limitTriggered = 1;
        return -1;
      }
      assembly->positionZ = newVal;
    } break;
    default:
      fprintf(stderr, "Invalid motion axis '%c' for linear motion.\n",
              assembly->motionAxis);
      break;
    }
  }

  return 0;
}

int ucncClearLimitWarning(const char *assemblyName) {
  ucncAssembly *assembly = findAssemblyByName(globalScene, assemblyName);
  if (!assembly)
    return -1;
  assembly->limitTriggered = 0;
  return 0;
}

void cncvis_handle_mouse_motion(int dx, int dy) {
  // TODO: Implement mouse motion handling for CNC visualization
  (void)dx;
  (void)dy;
}

void cncvis_handle_mouse_wheel(int wheel_delta) {
  if (!globalCamera)
    return;
  // Use the proper CAD camera zoom function
  ucncCameraZoom(globalCamera, wheel_delta * 2.0f);
  // Update camera matrix
  update_camera_matrix(globalCamera);
}

// Set all assemblies to their home position
void ucncSetAllAssembliesToHome(ucncAssembly *assembly) {
  if (!assembly)
    return;

  // Check if motionType is not null before comparing
  if (assembly->motionType &&
      strcmp(assembly->motionType, MOTION_TYPE_NONE) != 0) {
    // Set assembly's position and rotation to its home position
    assembly->positionX = assembly->homePositionX;
    assembly->positionY = assembly->homePositionY;
    assembly->positionZ = assembly->homePositionZ;
    assembly->rotationX = assembly->homeRotationX;
    assembly->rotationY = assembly->homeRotationY;
    assembly->rotationZ = assembly->homeRotationZ;

    printf("Assembly '%s' set to home position (%.2f, %.2f, %.2f) and rotation "
           "(%.2f, %.2f, %.2f).\n",
           assembly->name, assembly->homePositionX, assembly->homePositionY,
           assembly->homePositionZ, assembly->homeRotationX,
           assembly->homeRotationY, assembly->homeRotationZ);
  }

  // Recursively set all child assemblies to their home positions
  for (int i = 0; i < assembly->assemblyCount; i++) {
    ucncSetAllAssembliesToHome(assembly->assemblies[i]);
  }
}

// Set the dimensions of the TinyGL Z-buffer and return width/height
void ucncSetZBufferDimensions(int width, int height) {
  // Ensure width is a multiple of 4 (TinyGL requirement)
  if (width % 4 != 0) {
    int new_width = ((width + 3) / 4) * 4;
    fprintf(stderr,
            "Warning: Framebuffer width %d is not a multiple of 4, adjusting "
            "to %d\n",
            width, new_width);
    width = new_width;
  }

  if (globalFramebuffer) {
    ZB_close(globalFramebuffer);
  }
  globalFramebuffer = ZB_open(width, height, ZB_MODE_RGBA, 0);
  if (!globalFramebuffer) {
    fprintf(stderr, "Failed to initialize Z-buffer with dimensions %d x %d.\n",
            width, height);
    return;
  }
  printf(
      "ucncSetZBufferDimensions: Initialized framebuffer with size: %d x %d\n",
      globalFramebuffer->xsize, globalFramebuffer->ysize);
  if (globalFramebuffer->xsize != width || globalFramebuffer->ysize != height) {
    fprintf(stderr,
            "Framebuffer size mismatch: expected %d x %d, got %d x %d\n", width,
            height, globalFramebuffer->xsize, globalFramebuffer->ysize);
  }
}

// Expose Z-buffer output for external use
const float *ucncGetZBufferOutput(void) {
  if (!globalFramebuffer) {
    fprintf(stderr, "Framebuffer is not initialized.\n");
    return NULL;
  }
  return (const float *)globalFramebuffer->zbuf;
}

void ucncFrameReady(ZBuffer *framebuffer) {
  if (!framebuffer) {
    fprintf(stderr, "Framebuffer not provided for frame ready signal.\n");
    return;
  }
  printf("Frame is ready! Processing...\n");
  // Implement any processing, e.g., saving frame, signaling display.
}

int cncvis_init(const char *configFile) {
  // === [1] Extract config directory ===
  char configDir[1024];
  getDirectoryFromPath(configFile, configDir);

  // === [2] Set framebuffer size and initialize TinyGL Z-buffer ===
  ucncSetZBufferDimensions(ZGL_FB_WIDTH, ZGL_FB_HEIGHT);
  if (!globalFramebuffer) {
    fprintf(stderr, "Failed to initialize framebuffer.\n");
    return EXIT_FAILURE;
  }

  // === [3] Load scene and light configuration ===
  if (!loadConfiguration(configFile, &globalScene, &globalLights,
                         &globalLightCount)) {
    fprintf(stderr, "Failed to load configuration from '%s'.\n", configFile);
    return EXIT_FAILURE;
  }

  ucncSetAllAssembliesToHome(globalScene);
  printAssemblyHierarchy(globalScene, 0);
  printLightHierarchy(globalLights, globalLightCount, 0);

  // === [4] Initialize TinyGL with the framebuffer ===
  glInit(globalFramebuffer);

  // === [5] Initialize camera ===
  globalCamera = ucncCameraNew(800.0f, 800.0f, 400.0f, 0.0f, 0.0f, 1.0f);
  if (!globalCamera) {
    fprintf(stderr, "Failed to initialize camera.\n");
    return EXIT_FAILURE;
  }

  globalCamera->targetX = 0.0f;
  globalCamera->targetY = 0.0f;
  globalCamera->targetZ = 0.0f;
  globalCamera->fov = 45.0f;
  globalCamera->distance =
      sqrtf(800.0f * 800.0f + 800.0f * 800.0f + 400.0f * 400.0f);
  globalCamera->orthoMode = false;
  globalCamera->orthoScale = 1.0f;
  printCameraDetails(globalCamera);

  // === [6] Set OpenGL render state ===
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  // === [7] Set initial 3D projection and view ===
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  GLfloat aspectRatio =
      (GLfloat)globalFramebuffer->xsize / (GLfloat)globalFramebuffer->ysize;
  gluPerspective(60.0f, aspectRatio, 1.0f, 5000.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt_custom(globalCamera->positionX, globalCamera->positionY,
                   globalCamera->positionZ, globalCamera->targetX,
                   globalCamera->targetY, globalCamera->targetZ,
                   globalCamera->upX, globalCamera->upY, globalCamera->upZ);

  // === [8] Render initial background gradient ===
  float topColor[3] = {0.529f, 0.808f, 0.980f};    // Light Sky Blue
  float bottomColor[3] = {0.000f, 0.000f, 0.545f}; // Dark Blue
  setBackgroundGradient(topColor, bottomColor);

  // === [9] Initialize OSD system ===
  osdInit(globalFramebuffer);
  osdSetDefaultStyle(1.0f, 1.0f, 0.0f, 1.5f, 1); // Yellow text, scale 1.5x

  // === [10] Draw reference axis initially ===
  drawAxis(500.0f);

  // Ensure all GL commands are executed
  glFlush();

  return EXIT_SUCCESS;
}

// Function to reset the scene and load a new configuration
int ucncLoadNewConfiguration(const char *configFile) {
  // Free the existing scene if it's already loaded
  if (globalScene) {
    ucncAssemblyFree(globalScene);
    globalScene = NULL;
  }

  // Reset camera, if needed
  if (globalCamera) {
    ucncCameraFree(globalCamera);
    globalCamera = NULL;
  }

  // Load the new configuration from the provided XML file
  if (!loadConfiguration(configFile, &globalScene, &globalLights,
                         &globalLightCount)) {
    fprintf(stderr, "Failed to load configuration from '%s'.\n", configFile);
    return EXIT_FAILURE;
  }

  // Set all assemblies to their home positions
  ucncSetAllAssembliesToHome(globalScene);

  // Re-initialize the camera (assuming root assembly at origin)
  globalCamera = ucncCameraNew(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f); // Z is up
  if (!globalCamera) {
    fprintf(stderr, "Failed to create camera.\n");
    return EXIT_FAILURE;
  }

  printf("Successfully loaded new configuration from '%s'.\n", configFile);
  return EXIT_SUCCESS;
}

void cncvis_render(void) {
  // === [1] Clear color and depth buffers ===
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // === [2] Render background gradient ===
  {
    float topColor[3] = {0.529f, 0.808f, 0.980f};    // Light Sky Blue
    float bottomColor[3] = {0.000f, 0.000f, 0.545f}; // Dark Blue
    setBackgroundGradient(topColor, bottomColor);
  }

  // === [3] Set up 3D projection ===
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  GLfloat aspect =
      (GLfloat)globalFramebuffer->xsize / (GLfloat)globalFramebuffer->ysize;
  gluPerspective(60.0f, aspect, 1.0f, 5000.0f);

  // === [4] Set up camera (modelview matrix) ===
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt_custom(globalCamera->positionX, globalCamera->positionY,
                   globalCamera->positionZ, globalCamera->targetX,
                   globalCamera->targetY, globalCamera->targetZ,
                   globalCamera->upX, globalCamera->upY, globalCamera->upZ);

  // === [5] Ensure proper OpenGL state for 3D rendering ===
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);

  // === [6] Render 3D scene ===
  ucncAssemblyRender(globalScene);
  drawAxis(500.0f); // Optional reference axis

  // === [7] Calculate frame timing ===
  float fps = calculateFPS();

  // === [8] Determine machine/tool position ===
  float machine_x = 0.0f, machine_y = 0.0f, machine_z = 0.0f;
  ucncAssembly *tool = findAssemblyByName(globalScene, "tool");
  if (tool) {
    machine_x = tool->positionX;
    machine_y = tool->positionY;
    machine_z = tool->positionZ;
  } else {
    ucncAssembly *endEffector = findAssemblyByName(globalScene, "end_effector");
    if (endEffector) {
      machine_x = endEffector->positionX;
      machine_y = endEffector->positionY;
      machine_z = endEffector->positionZ;
    }
  }

  // === [9] Render OSD (on-screen display) ===
  {
    // Set up 2D orthographic projection for OSD
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, globalFramebuffer->xsize, 0.0, globalFramebuffer->ysize, -1.0,
            1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Disable depth test and lighting for OSD
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // Draw machine coordinates
    osdDrawTextf(10, 10, OSD_ALIGN_LEFT, "X: %.3f Y: %.3f Z: %.3f", machine_x,
                 machine_y, machine_z);

    // Prepare status text
    char statusText[64];
    snprintf(statusText, sizeof(statusText), "RUNNING\n%.1f FPS", fps);

    // Calculate bounding box for status panel
    OSDStyle style = {0.0f, 1.0f, 0.0f, 1.2f, 1}; // Green, 1.2x scale
    int textW = calculateTextWidth("RUNNING", style.scale, style.spacing);
    int fpsW = calculateTextWidth("100.0 FPS", style.scale, style.spacing);
    int textWidth = (fpsW > textW ? fpsW : textW);
    int textHeight = 8 * 2 * style.scale + 4;

    int statusX = globalFramebuffer->xsize - 10;
    int statusY = 10;

    // Draw background panel
    osdDrawRect(statusX - textWidth - 6, statusY - 2, textWidth + 12,
                textHeight + 4, 0.0f, 0.0f, 0.0f, 0.7f // Semi-transparent black
    );

    // Draw text overlay
    osdDrawTextStyled(statusText, statusX, statusY, OSD_ALIGN_RIGHT, &style);

    // Draw bottom help text
    osdDrawTextf(globalFramebuffer->xsize / 2, globalFramebuffer->ysize - 20,
                 OSD_ALIGN_CENTER, "F1-F5: Views | Space: Toggle Projection");

    // Restore matrices and states
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // Restore 3D rendering states
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
  }

  // === [10] Ensure all GL commands are executed ===
  glFlush();
}

void cncvis_cleanup() {

  // Free assemblies and actors
  ucncAssemblyFree(globalScene);
  globalScene = NULL;

  // Free lights
  freeAllLights(&globalLights, globalLightCount);
  globalLights = NULL;
  globalLightCount = 0;

  // Free camera
  ucncCameraFree(globalCamera);
  globalCamera = NULL;

  // Close TinyGL context
  glClose();
  ZB_close(globalFramebuffer);
  globalFramebuffer = NULL;
}