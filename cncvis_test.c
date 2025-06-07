#include "api.h"
#include "assembly.h"
#include "utils.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>

#define CHAD_MATH_IMPL
#include "3dMath.h"
#define CHAD_API_IMPL
#include "tinygl/include/zbuffer.h"

ZBuffer *globalFramebuffer = NULL;
ucncAssembly *globalScene = NULL;
ucncCamera *globalCamera = NULL;
ucncLight **globalLights = NULL;
int globalLightCount = 0;

static void orbit_camera_z(float delta_deg) {
    static float angle = 0.0f;
    angle += delta_deg;
    float rad = angle * (float)M_PI / 180.0f;
    float radius = sqrtf((globalCamera->positionX - globalCamera->targetX) *
                         (globalCamera->positionX - globalCamera->targetX) +
                         (globalCamera->positionY - globalCamera->targetY) *
                         (globalCamera->positionY - globalCamera->targetY));
    globalCamera->positionX = globalCamera->targetX + radius * cosf(rad);
    globalCamera->positionY = globalCamera->targetY + radius * sinf(rad);
    globalCamera->yaw = angle;
    update_camera_matrix(globalCamera);
}

static void test_init_and_motion(void) {
    int rc = cncvis_init("machines/meca500/config.xml");
    assert(rc == 0);
    assert(globalScene != NULL);
    assert(globalCamera != NULL);

    ucncAssembly *link1 = findAssemblyByName(globalScene, "link1");
    assert(link1 != NULL);
    float prev_rot = link1->rotationZ;
    ucncUpdateMotionByName("link1", 5.0f);
    assert(link1->rotationZ == prev_rot + 5.0f);

    cncvis_render();

    /* export the frame using stb_image_write */
    saveFramebufferAsImage(globalFramebuffer, "frame.png",
                           globalFramebuffer->xsize, globalFramebuffer->ysize);

    const float *zbuf = ucncGetZBufferOutput();
    assert(zbuf != NULL);
    cncvis_cleanup();
}

static void test_reload_config(void) {
    int rc = cncvis_init("machines/meca500/config.xml");
    assert(rc == 0);
    rc = ucncLoadNewConfiguration("machines/meca500/config.xml");
    assert(rc == 0);
    cncvis_cleanup();
}

static void test_orbit_video(void) {
    int rc = cncvis_init("machines/meca500/config.xml");
    assert(rc == 0);

    ucncCameraSetTarget(globalCamera, 0.0f, 0.0f, 0.0f);
    mkdir("frames", 0755);

    // Debug: Print framebuffer resolution
    printf("Framebuffer resolution: %d x %d\n", globalFramebuffer->xsize, globalFramebuffer->ysize);

    for (int i = 0; i < 60; ++i) {
        cncvis_render();
        char fname[64];
        snprintf(fname, sizeof(fname), "frames/frame%03d.png", i);
        saveFramebufferAsImage(globalFramebuffer, fname, globalFramebuffer->xsize,
                               globalFramebuffer->ysize);
        orbit_camera_z(6.0f);
    }

    cncvis_cleanup();

    system("ffmpeg -y -framerate 30 -i frames/frame%03d.png -c:v libx264 -crf 15 -preset veryslow -pix_fmt yuv444p orbit.mp4");
}

static void test_benchmark(void) {
    // Initialize profiling stats
    ProfilingStats stats;
    initProfilingStats(&stats);

    // Number of frames to benchmark
    const int num_frames = 100;

    // Measure initialization time
    double init_start = getCurrentTimeInMs();
    int rc = cncvis_init("machines/meca500/config.xml");
    double init_time = getCurrentTimeInMs() - init_start;
    assert(rc == 0);
    assert(globalScene != NULL);
    assert(globalCamera != NULL);

    ucncCameraSetTarget(globalCamera, 0.0f, 0.0f, 0.0f);

    // Benchmark rendering loop
    for (int i = 0; i < num_frames; ++i) {
        FrameTiming timing = {0};

        // Measure camera setup
        double start = getCurrentTimeInMs();
        orbit_camera_z(6.0f); // Rotate camera
        timing.cameraSetupTime = getCurrentTimeInMs() - start;

        // Measure rendering
        start = getCurrentTimeInMs();
        cncvis_render();
        timing.sceneRenderTime = getCurrentTimeInMs() - start;

        // Measure frame saving (optional, can skip to reduce I/O impact)
        start = getCurrentTimeInMs();
        char fname[64];
        snprintf(fname, sizeof(fname), "bench_frames/frame%03d.png", i);
        mkdir("bench_frames", 0755);
        saveFramebufferAsImage(globalFramebuffer, fname, globalFramebuffer->xsize,
                               globalFramebuffer->ysize);
        timing.imageSaveTime = getCurrentTimeInMs() - start;

        // Total frame time
        timing.totalFrameTime = timing.cameraSetupTime + timing.sceneRenderTime + timing.imageSaveTime;

        // Update stats
        updateProfilingStats(&stats, &timing);
    }

    // Clean up
    cncvis_cleanup();

    // Print results
    printf("\nBenchmark Results (%d frames):\n", num_frames);
    printf("Initialization Time: %.2f ms\n", init_time);
    printProfilingStats(&stats, num_frames);
}

int main(void) {
    test_init_and_motion();
    test_reload_config();
    test_orbit_video();
    test_benchmark();
    return 0;
}