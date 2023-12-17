#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>

#include <raylib.h>

#include "pak/pak.h"
#include "song.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

float size[2] = {1280, 720};
float radius = 128.0f;

int main() {
  InitWindow(1280, 720, "raylib [core] example - basic window");

  Shader shader = LoadShader(0, "data/shaders/shd_round_corners.fs");

  HYUZU_UE_Pak* pak = HYUZU_Pak_Load("fake.pak");
  HYUZU_Song* song = new HYUZU_Song;

  song = HYUZU_Song_Load(pak);
  Texture2D tex = LoadTextureFromImage(song->cover);

  SetTargetFPS(60);

  Camera camera = { 0 };
  camera.position = { 2.0f, 3.0f, 2.0f };
  camera.target = { 0.0f, 1.0f, 0.0f };
  camera.up = { 0.0f, 1.0f, 0.0f };
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  RenderTexture2D target = LoadRenderTexture(1280, 720);

  while (!WindowShouldClose())
  {
      UpdateCamera(&camera, CAMERA_ORBITAL);

      BeginTextureMode(target);
          ClearBackground(RAYWHITE);
              BeginMode3D(camera);
              DrawGrid(10, 1.0f);
          EndMode3D();
      EndTextureMode();

      BeginDrawing();
          ClearBackground(RAYWHITE);

          DrawTextureRec(target.texture, { 0, 0, (float)target.texture.width, (float)-target.texture.height }, { 0, 0 }, WHITE);

          BeginShaderMode(shader);
              SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), tex);
              DrawTexture(tex, 30, 30, WHITE);
          EndShaderMode();

          DrawText(song->title.c_str(), 562, 30, 30, BLACK);
          DrawText(song->artist.c_str(), 562, 60, 20, BLACK);
          DrawText(TextFormat("BPM: %i", song->bpm), 562, 85, 20, BLACK);
          DrawText(TextFormat("Key: %s %s", HYUZU_Song_ParseKeyToString(song->key).c_str(), HYUZU_Song_ParseModeToString(song->mode).c_str()), 562, 110, 20, BLACK);

          DrawText(TextFormat("Creator: %s", song->creator.c_str()), 562, 170, 20, BLACK);
      EndDrawing();
  }

  UnloadRenderTexture(target);
  UnloadShader(shader);
  UnloadShader(gray);
  CloseWindow();
  
  delete pak;
  delete song;

  return 0;
}