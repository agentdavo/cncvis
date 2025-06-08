// cncvis.h
#ifndef CNCVIS_H
#define CNCVIS_H

#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define MOTION_TYPE_ROTATIONAL "rotational"
#define MOTION_TYPE_LINEAR "linear"
#define MOTION_TYPE_NONE "none"
#define AXIS_X 'X'
#define AXIS_Y 'Y'
#define AXIS_Z 'Z'

#include "tinygl/include/GL/gl.h"
#include "tinygl/include/GL/glu.h"
#include "tinygl/include/zbuffer.h"
#define CHAD_API_IMPL
#include "chad_math.h"
#include "tinygl/src/font8x8_basic.h"

#ifndef M_PI
#define M_PI 3.14159265
#endif

#endif // CNCVIS_H
