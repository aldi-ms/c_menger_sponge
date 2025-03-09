#include "include/raylib.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 1024;
  const int screenHeight = 768;

  InitWindow(screenWidth, screenHeight, "Menger sponge fractal");

  // Update
  //----------------------------------------------------------------------------------
  // TODO: Update your variables here
  //----------------------------------------------------------------------------------

  const Vector3 cubePosition = {0.0f, 0.0f, 0.0f};
  const Vector3 vectorOne = (Vector3){1.0f, 1.0f, 1.0f};
  const Vector3 vectorZero = (Vector3){0.0f, 0.0f, 0.0f};
  Camera3D cam = {0};
  cam.position = (Vector3){10.0f, 10.0f, 10.0f};
  cam.target = vectorZero;
  cam.up = (Vector3){0.0f, 1.0f, 0.0f};
  cam.fovy = 45.0f;
  cam.projection = CAMERA_PERSPECTIVE;

  short level = 0;
  short lvlStrLength = 9;

  float cubeSide = 2.0f;
  Mesh cubeMesh = GenMeshCube(cubeSide, cubeSide, cubeSide);
  Model cubeModel = LoadModelFromMesh(cubeMesh);
  char *levelInfo = "Level: 0";

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

      //cubeSide = cubeSide / 3.0f;
    }

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(SKYBLUE);

    BeginMode3D(cam);

    DrawModelEx(cubeModel, cubePosition, cubePosition, 0.0f, vectorOne, RED);
    DrawModelWiresEx(cubeModel, cubePosition, cubePosition, 0.0f, vectorOne,
                     BLACK);

    DrawGrid(20, 1.0f);

    EndMode3D();

    DrawFPS(10, 10);

    DrawText(levelInfo, 20, 30, 14, BLACK);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------

  UnloadModel(cubeModel);
  UnloadMesh(cubeMesh);
  free(levelInfo);
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
