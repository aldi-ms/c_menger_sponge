#include "include/clock_gettime.h"
#include "include/raylib.h"
#include <profileapi.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_ELEMENTS 161000

int PushNewCube(Vector3 pos, Model model, Vector3 positions[], Model models[], size_t *idx);
int GetNextGenCubes(Vector3 **positions, Model **models, size_t arrayLength, float nextCubeSize);

const float RotationAngle = 0.5f;
const Vector3 RotationAxis = {0.5f, 0.5f, 0.5f};

int main(void)
{
  setvbuf(stdout, NULL, _IOFBF, 0);
  setvbuf(stdin, NULL, _IOFBF, 0);
  SetConfigFlags(FLAG_MSAA_4X_HINT);

  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 1024;
  const int screenHeight = 768;

  InitWindow(screenWidth, screenHeight, "Menger sponge fractal");

  // Update
  //----------------------------------------------------------------------------------
  // TODO: Update your variables here
  //----------------------------------------------------------------------------------

  const Vector3 vectorOne = (Vector3){1.0f, 1.0f, 1.0f};
  const Vector3 vectorZero = (Vector3){0.0f, 0.0f, 0.0f};
  const Vector3 cubePosition = vectorZero;
  Camera3D cam = {0};
  cam.position = (Vector3){10.0f, 10.0f, 10.0f};
  cam.target = vectorZero;
  cam.up = (Vector3){0.0f, 1.0f, 0.0f};
  cam.fovy = 35.0f;
  cam.projection = CAMERA_PERSPECTIVE;

  short level = 0;
  short lvlStrLength = 9;
  float cubeSide = 1.0f;
  char *levelInfo = "Level: 0";
  size_t idx = 0;
  Vector3 *positions = malloc(ARRAY_ELEMENTS * sizeof(Vector3));
  Model *models = malloc(ARRAY_ELEMENTS * sizeof(Model));

  Model cubeModel = LoadModelFromMesh(GenMeshCube(cubeSide, cubeSide, cubeSide));
  PushNewCube(cubePosition, cubeModel, positions, models, &idx);

  DisableCursor();

  SetTargetFPS(60);
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    UpdateCamera(&cam, CAMERA_FREE);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      level++;
      levelInfo = (char *)malloc(lvlStrLength * sizeof(char));
      lvlStrLength = snprintf(levelInfo, lvlStrLength + 1, "Level: %d", level);

      cubeSide = cubeSide / 3.0f;

      double begin;
      PROFILE_BEGIN(begin);
      // struct timespec tic = {0};
      // clock_gettime(CLOCK_MONOTONIC, &tic);

      idx = GetNextGenCubes(&positions, &models, idx, cubeSide);

      PROFILE_END(begin, "GetNextGenCubes");
      // struct timespec toc = {0};
      // clock_gettime(CLOCK_MONOTONIC, &toc);

      // struct timespec time_diff = {0};
      // timespec_diff(&tic, &toc, &time_diff);
      // printf("Elapsed ns=%ld\n", time_diff.tv_nsec);
      // printf("Elapsed s=%ld\n", time_diff.tv_sec);
    }

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(SKYBLUE);

    BeginMode3D(cam);

    for (size_t i = 0; i < idx; i++) {
      DrawModelEx(models[i], positions[i], RotationAxis, RotationAngle, vectorOne, YELLOW);
      DrawModelWiresEx(models[i], positions[i], RotationAxis, RotationAngle, vectorOne, BLACK);
    }

    DrawGrid(20, 1.0f);

    EndMode3D();

    DrawFPS(10, 10);

    DrawText(levelInfo, 20, 30, 14, BLACK);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------

  for (size_t i = 0; i < idx; i++) {
    UnloadModel(models[i]);
  }

  free(levelInfo);
  free(positions);
  free(models);

  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

int PushNewCube(Vector3 pos, Model model, Vector3 positions[], Model models[], size_t *idx)
{
  if ((*idx) + 1 >= ARRAY_ELEMENTS) {
    return -1;
  }

  positions[*idx] = pos;
  models[*idx] = model;
  (*idx)++;

  return 0;
}

int GetNextGenCubes(Vector3 **positions, Model **models, size_t arrayLength, float nextCubeSize)
{
  Vector3 *nextPositions = malloc(ARRAY_ELEMENTS * sizeof(Vector3));
  Model *nextModels = malloc(ARRAY_ELEMENTS * sizeof(Model));
  if (!nextPositions || !nextModels) {
    free(nextPositions);
    free(nextModels);
    return -1;
  }

  size_t idx = 0;
  Model cube = LoadModelFromMesh(GenMeshCube(nextCubeSize, nextCubeSize, nextCubeSize));

  for (int i = 0; i < arrayLength; i++) {
    Vector3 basePos = (*positions)[i];

    for (int x = 0; x < 3; x++) {
      for (int y = 0; y < 3; y++) {
        for (int z = 0; z < 3; z++) {
          if ((x == 1 && y == 1) || (x == 1 && z == 1) || (y == 1 && z == 1)) {
            continue;
          }

          Vector3 nextPos = {basePos.x + (x - 1) * nextCubeSize, basePos.y + (y - 1) * nextCubeSize,
                             basePos.z + (z - 1) * nextCubeSize};
          // printf("Next position for [%d,%d,%d] cube = [%f,%f,%f].\n", x, y, z, nextPos.x, nextPos.y, nextPos.z);

          if (PushNewCube(nextPos, cube, nextPositions, nextModels, &idx)) {
            // most probably ARRAY_ELEMENTS overflow
            return -1;
          }
        }
      }
    }
  }

  free(*positions);
  *positions = NULL;
  free(*models);
  *models = NULL;

  *positions = nextPositions;
  *models = nextModels;

  return (int)idx;
}
