#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include "actor.h"
#include "cncvis.h"

#define MAX_NAME_LENGTH 64

extern void drawAxis(float);

typedef struct ucncAssembly {
  char name[MAX_NAME_LENGTH];
  char parentName[MAX_NAME_LENGTH];
  float originX, originY, originZ;
  float positionX, positionY, positionZ;
  float rotationX, rotationY, rotationZ;
  float homePositionX, homePositionY, homePositionZ;
  float homeRotationX, homeRotationY, homeRotationZ;
  float colorR, colorG, colorB;
  float minPos, maxPos;
  float minRot, maxRot;
  int limitTriggered;
  char motionType[MAX_NAME_LENGTH];
  char motionAxis;
  int invertMotion;
  struct ucncActor **actors;
  int actorCount;
  struct ucncAssembly **assemblies;
  int assemblyCount;
} ucncAssembly;

ucncAssembly *
ucncAssemblyNew(const char *name, const char *parentName, float originX,
                float originY, float originZ, float positionX, float positionY,
                float positionZ, float rotationX, float rotationY,
                float rotationZ, float homePositionX, float homePositionY,
                float homePositionZ, float homeRotationX, float homeRotationY,
                float homeRotationZ, float colorR, float colorG, float colorB,
                const char *motionType, char motionAxis, int invertMotion,
                float minPos, float maxPos, float minRot, float maxRot);

int ucncAssemblyAddActor(ucncAssembly *assembly, ucncActor *actor);
int ucncAssemblyAddAssembly(ucncAssembly *parent, ucncAssembly *child);
void ucncAssemblyRender(const ucncAssembly *assembly);
void ucncAssemblyFree(ucncAssembly *assembly);
ucncAssembly *findAssemblyByName(ucncAssembly *rootAssembly, const char *name);

void cleanupAssemblies(ucncAssembly **assemblies, int assemblyCount);

#endif // ASSEMBLY_H
