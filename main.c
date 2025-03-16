#include "include/raylib.h"
#include <stdatomic.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_ELEMENTS 10000

int PushNewCube(Vector3 pos, Model model, Vector3 positions[], Model models[],                size_t *idx);
int GetNextGenCubes(Vector3 **positions, Model **models, size_t *arrayLength,
                    float nextCubeSize);

int main(void) {
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
  cam.fovy = 45.0f;
  cam.projection = CAMERA_PERSPECTIVE;

  short level = 0;
  short lvlStrLength = 9;
  float cubeSide = 1.0f;
  char *levelInfo = "Level: 0";

  size_t idx = 0;

  Vector3 *positions = malloc(ARRAY_ELEMENTS * sizeof(Vector3));
  Model *models = malloc(ARRAY_ELEMENTS * sizeof(Model));

  Model cubeModel =
      LoadModelFromMesh(GenMeshCube(cubeSide, cubeSide, cubeSide));
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

      idx = GetNextGenCubes(&positions, &models, &idx, cubeSide);
    }

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(SKYBLUE);

    BeginMode3D(cam);

    for (int i = 0; i < idx; i++) {
      // printf("pos %d: %f:%f:%f \n", i, positions[i].x, positions[i].y,
      // positions[i].z);
      DrawModelEx(models[i], positions[i], vectorZero, 0.0f, vectorOne, YELLOW);
      DrawModelWiresEx(models[i], positions[i], vectorZero, 0.0f, vectorOne,
                       BLACK);
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

  for (int i = 0; i < idx; i++) {
    UnloadModel(models[i]);
  }

  free(levelInfo);
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

int PushNewCube(Vector3 pos, Model model, Vector3 positions[], Model models[],
                size_t *idx) {
  if ((*idx) + 1 >= ARRAY_ELEMENTS) {
    return 1;
  }

  positions[*idx] = pos;
  models[*idx] = model;
  (*idx)++;

  return 0;
}

int GetNextGenCubes(Vector3 **positions, Model **models, size_t *arrayLength,
                    float nextCubeSize) {

  size_t idx = 0;

  Vector3 *nextPositions = malloc(ARRAY_ELEMENTS * sizeof(Vector3));
  Model *nextModels = malloc(ARRAY_ELEMENTS * sizeof(Model));

  for (int i = 0; i < *arrayLength; i++) {

    for (int x = 0; x < 3; x++) {
      for (int y = 0; y < 3; y++) {
        for (int z = 0; z < 3; z++) {
          if ((x == 1 && y == 1) || (x == 1 && z == 1) || (y == 1 && z == 1)) {
            continue;
          }

          Model cube = LoadModelFromMesh(
              GenMeshCube(nextCubeSize, nextCubeSize, nextCubeSize));

          Vector3 nextPos = (Vector3){(*positions)[i].x, (*positions)[i].y,
                                      (*positions)[i].z};
          if (x < 1) {
            nextPos.x = nextPos.x - nextCubeSize;
          } else if (x > 1) {
            nextPos.x = nextPos.x + nextCubeSize;
          }

          if (y < 1) {
            nextPos.y = nextPos.y - nextCubeSize;
          } else if (y > 1) {
            nextPos.y = nextPos.y + nextCubeSize;
          }

          if (z < 1) {
            nextPos.z = nextPos.z - nextCubeSize;
          } else if (z > 1) {
            nextPos.z = nextPos.z + nextCubeSize;
          }

          printf("Generated next position for [%d,%d,%d] cube = [%f,%f,%f].", x,
                 y, z, nextPos.x, nextPos.y, nextPos.z);
          PushNewCube(nextPos, cube, nextPositions, nextModels, &idx);
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

  return idx;
}
