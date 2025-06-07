/* utils.c */

#include "utils.h"
#include "tinygl/include/GL/gl.h"
#include "tinygl/include/GL/glu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

// allow utils.c to see the global TinyGL framebuffer
extern ZBuffer *globalFramebuffer;

// Implementations of utility functions

double getCurrentTimeInMs() {
#ifdef _WIN32
    // Windows implementation
    LARGE_INTEGER frequency;
    LARGE_INTEGER currentTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&currentTime);
    return (double)(currentTime.QuadPart * 1000) / frequency.QuadPart;
#else
    // POSIX implementation
    struct timeval time;
    gettimeofday(&time, NULL);
    return (double)(time.tv_sec) * 1000.0 + (double)(time.tv_usec) / 1000.0;
#endif
}

void setupProjection(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glRotatef(90.0f, 1, 0, 0);
    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    gluPerspective(60.0f, aspect, 1.0f, 5000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void initProfilingStats(ProfilingStats *stats) {
    memset(stats, 0, sizeof(ProfilingStats));
    stats->minCameraSetup = 1e9;
    stats->maxCameraSetup = 0.0;
    stats->minSceneRender = 1e9;
    stats->maxSceneRender = 0.0;
    stats->minImageSave = 1e9;
    stats->maxImageSave = 0.0;
    stats->minFrame = 1e9;
    stats->maxFrame = 0.0;
}

void updateProfilingStats(ProfilingStats *stats, FrameTiming *frameTiming) {
    stats->totalCameraSetup += frameTiming->cameraSetupTime;
    stats->totalSceneRender += frameTiming->sceneRenderTime;
    stats->totalImageSave += frameTiming->imageSaveTime;
    stats->totalFrame += frameTiming->totalFrameTime;

    if (frameTiming->cameraSetupTime < stats->minCameraSetup)
        stats->minCameraSetup = frameTiming->cameraSetupTime;
    if (frameTiming->cameraSetupTime > stats->maxCameraSetup)
        stats->maxCameraSetup = frameTiming->cameraSetupTime;

    if (frameTiming->sceneRenderTime < stats->minSceneRender)
        stats->minSceneRender = frameTiming->sceneRenderTime;
    if (frameTiming->sceneRenderTime > stats->maxSceneRender)
        stats->maxSceneRender = frameTiming->sceneRenderTime;

    if (frameTiming->imageSaveTime < stats->minImageSave)
        stats->minImageSave = frameTiming->imageSaveTime;
    if (frameTiming->imageSaveTime > stats->maxImageSave)
        stats->maxImageSave = frameTiming->imageSaveTime;

    if (frameTiming->totalFrameTime < stats->minFrame)
        stats->minFrame = frameTiming->totalFrameTime;
    if (frameTiming->totalFrameTime > stats->maxFrame)
        stats->maxFrame = frameTiming->totalFrameTime;
}

void printProfilingStats(ProfilingStats *stats, int totalFrames) {
    printf("\n=== Performance Profiling Summary ===\n");
    printf("Total Frames Rendered: %d\n\n", totalFrames);

    printf("Camera Setup Time (ms):\n");
    printf("  Total: %.2f\n", stats->totalCameraSetup);
    printf("  Average: %.2f\n", stats->totalCameraSetup / totalFrames);
    printf("  Min: %.2f\n", stats->minCameraSetup);
    printf("  Max: %.2f\n\n", stats->maxCameraSetup);

    printf("Scene Render Time (ms):\n");
    printf("  Total: %.2f\n", stats->totalSceneRender);
    printf("  Average: %.2f\n", stats->totalSceneRender / totalFrames);
    printf("  Min: %.2f\n", stats->minSceneRender);
    printf("  Max: %.2f\n\n", stats->maxSceneRender);

    printf("Image Save Time (ms):\n");
    printf("  Total: %.2f\n", stats->totalImageSave);
    printf("  Average: %.2f\n", stats->totalImageSave / totalFrames);
    printf("  Min: %.2f\n", stats->minImageSave);
    printf("  Max: %.2f\n\n", stats->maxImageSave);

    printf("Total Frame Time (ms):\n");
    printf("  Total: %.2f\n", stats->totalFrame);
    printf("  Average: %.2f\n", stats->totalFrame / totalFrames);
    printf("  Min: %.2f\n", stats->minFrame);
    printf("  Max: %.2f\n", stats->maxFrame);
    printf("====================================\n");
}

void drawArrowHead(float size, float x, float y, float z) {
    float arrowSize = size * 0.1f;
    glBegin(GL_TRIANGLES);
        if (x != 0.0f) {
            glColor3f(1,0,0);
            glVertex3f(x,0,0);
            glVertex3f(x - arrowSize, arrowSize, 0);
            glVertex3f(x - arrowSize, -arrowSize,0);
        }
        if (y != 0.0f) {
            glColor3f(0,1,0);
            glVertex3f(0,y,0);
            glVertex3f(-arrowSize, y - arrowSize,0);
            glVertex3f(arrowSize,  y - arrowSize,0);
        }
        if (z != 0.0f) {
            glColor3f(0,0,1);
            glVertex3f(0,0,z);
            glVertex3f(0, arrowSize, z - arrowSize);
            glVertex3f(0,-arrowSize, z - arrowSize);
        }
    glEnd();
}

void drawAxis(float size) {
    if (size <= 0.0f) size = 1.0f;
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glBegin(GL_LINES);
        glColor3f(1,0,0);
        glVertex3f(0,0,0); glVertex3f(size,0,0);
        glColor3f(0,1,0);
        glVertex3f(0,0,0); glVertex3f(0,size,0);
        glColor3f(0,0,1);
        glVertex3f(0,0,0); glVertex3f(0,0,size);
    glEnd();
    drawArrowHead(size,size,0,0);
    drawArrowHead(size,0,size,0);
    drawArrowHead(size,0,0,size);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void setBackgroundGradient(float topColor[3], float bottomColor[3]) {
    // Debug: Print framebuffer dimensions
    printf("Rendering background with framebuffer size: %d x %d\n", 
           globalFramebuffer->xsize, globalFramebuffer->ysize);

    // Ensure full viewport
    glViewport(0, 0, globalFramebuffer->xsize, globalFramebuffer->ysize);

    // Disable depth testing/writing to ensure background is behind the scene
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // Disable lighting for pure color gradient
    glDisable(GL_LIGHTING);

    // Preserve modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity(); // Reset modelview to avoid scene transformations

    // Render large quad at far z to cover the view frustum
    glBegin(GL_QUADS);
        glColor3fv(bottomColor);
        glVertex3f(-1500.0f, -1500.0f, -1500.0f); // Bottom-left
        glColor3fv(topColor);
        glVertex3f(-1500.0f, 1500.0f, -1500.0f);  // Top-left
        glColor3fv(topColor);
        glVertex3f(1500.0f, 1500.0f, -1500.0f);   // Top-right
        glColor3fv(bottomColor);
        glVertex3f(1500.0f, -1500.0f, -1500.0f);  // Bottom-right
    glEnd();

    // Restore modelview matrix
    glPopMatrix();

    // Restore OpenGL states
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
}



void CreateGround(float sizeX, float sizeY) {
    glPushMatrix();
    glColor3f(0.25f,0.25f,0.25f);
    glBegin(GL_QUADS);
      glNormal3f(0,0,1);
      glVertex3f(-sizeX,-sizeY,-100.0f);
      glVertex3f( sizeX,-sizeY,-100.0f);
      glVertex3f( sizeX, sizeY,-100.0f);
      glVertex3f(-sizeX, sizeY,-100.0f);
    glEnd();
    glPopMatrix();
}

void saveFramebufferAsImage(ZBuffer *framebuffer,
                            const char *filename,
                            int width,
                            int height) {
    if (!framebuffer || !filename) return;
    PIXEL *imbuf = calloc(width*height, sizeof(PIXEL));
    if (!imbuf) return;
    unsigned char *pbuf = malloc(3*width*height);
    if (!pbuf) { free(imbuf); return; }

    ZB_copyFrameBuffer(framebuffer, imbuf, width * sizeof(PIXEL));
    for (int i = 0; i < width*height; i++) {
        pbuf[3*i+0] = GET_RED(imbuf[i]);
        pbuf[3*i+1] = GET_GREEN(imbuf[i]);
        pbuf[3*i+2] = GET_BLUE(imbuf[i]);
    }
    if (!stbi_write_png(filename, width, height, 3, pbuf, width*3)) {
        fprintf(stderr, "Failed to write image to %s\n", filename);
    }
    free(imbuf);
    free(pbuf);
}

void printAssemblyHierarchy(ucncAssembly *assembly, int level) {
    if (!assembly) return;
    for (int i=0; i<level; i++) printf("  ");
    printf("Assembly '%s':\n", assembly->name);
    for (int i=0; i<level+1; i++) printf("  ");
    printf("Origin: (%.2f, %.2f, %.2f)\n",
           assembly->originX, assembly->originY, assembly->originZ);
    for (int i=0; i<level+1; i++) printf("  ");
    printf("Position: (%.2f, %.2f, %.2f)\n",
           assembly->positionX, assembly->positionY, assembly->positionZ);
    for (int i=0; i<level+1; i++) printf("  ");
    printf("Rotation: (%.2f, %.2f, %.2f)\n",
           assembly->rotationX, assembly->rotationY, assembly->rotationZ);
    for (int i=0; i<level+1; i++) printf("  ");
    printf("Home Pos: (%.2f, %.2f, %.2f)\n",
           assembly->homePositionX, assembly->homePositionY, assembly->homePositionZ);
    for (int i=0; i<level+1; i++) printf("  ");
    printf("Home Rot: (%.2f, %.2f, %.2f)\n",
           assembly->homeRotationX, assembly->homeRotationY, assembly->homeRotationZ);
    for (int i=0; i<level+1; i++) printf("  ");
    printf("Color: (R:%.2f, G:%.2f, B:%.2f)\n",
           assembly->colorR, assembly->colorG, assembly->colorB);
    for (int i=0; i<level+1; i++) printf("  ");
    printf("Motion: Type '%s', Axis '%c', Invert: %s\n",
           assembly->motionType,
           assembly->motionAxis,
           assembly->invertMotion ? "yes" : "no");
    for (int i=0; i<assembly->assemblyCount; i++) {
        printAssemblyHierarchy(assembly->assemblies[i], level+1);
    }
}

void getDirectoryFromPath(const char *filePath, char *dirPath) {
    const char *lastSlash = strrchr(filePath, '/');
    if (!lastSlash) lastSlash = strrchr(filePath, '\\');
    if (lastSlash) {
        size_t len = lastSlash - filePath;
        strncpy(dirPath, filePath, len);
        dirPath[len] = '\0';
    } else {
        strcpy(dirPath, ".");
    }
}

void scanAssembly(const ucncAssembly *assembly,
                  int *totalAssemblies,
                  int *totalActors) {
    if (!assembly) return;
    printf("Assembly: %s\n", assembly->name);
    (*totalAssemblies)++;
    for (int i=0; i<assembly->actorCount; i++) {
        ucncActor *actor = assembly->actors[i];
        if (actor && actor->stlObject) {
            printf("  Actor: %s, Triangles: %lu\n",
                   actor->name, actor->triangleCount);
            (*totalActors)++;
        } else {
            printf("  Actor: %s has no STL data.\n", actor->name);
        }
    }
    for (int i=0; i<assembly->assemblyCount; i++) {
        scanAssembly(assembly->assemblies[i],
                     totalAssemblies,
                     totalActors);
    }
}

void scanGlobalScene(const ucncAssembly *assembly) {
    if (!assembly) {
        printf("No assemblies found in the global scene.\n");
        return;
    }
    int totalA=0, totalAct=0;
    printf("Scanning Global Scene...\n");
    scanAssembly(assembly, &totalA, &totalAct);
    printf("Total Assemblies: %d\n", totalA);
    printf("Total Actors: %d\n", totalAct);
}

static double previousTime = 0.0;
static double currentTime  = 0.0;
static int frameCount      = 0;
static float fps           = 0.0f;

float calculateFPS(void) {
    struct timeval time;
    gettimeofday(&time, NULL);
    currentTime = time.tv_sec*1000.0 + time.tv_usec/1000.0;
    frameCount++;
    double delta = currentTime - previousTime;
    if (delta >= 1000.0) {
        fps = frameCount / (delta/1000.0);
        previousTime = currentTime;
        frameCount = 0;
    }
    return fps;
}

void renderFPSData(int frameNumber, float fps) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
      glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
      glLoadIdentity();

      glTextSize(GL_TEXT_SIZE16x16);
      unsigned int color = 0x00FFFFFF;
      char buf[64];
      snprintf(buf, sizeof(buf), "FRM: %d", frameNumber);
      glDrawText((unsigned char*)buf, 10, 10, color);
      snprintf(buf, sizeof(buf), "FPS: %.1f", fps);
      glDrawText((unsigned char*)buf, 10, 30, color);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
