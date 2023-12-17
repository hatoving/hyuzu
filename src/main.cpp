#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>

#include <raylib.h>

#include "pak/pak.h"
#include "song/song.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

int main() {
  SetTraceLogLevel(LOG_WARNING | LOG_ERROR);
  InitWindow(1280, 720, "raylib [core] example - basic window");

  Shader cover_shader = LoadShader(0, "data/shaders/shd_album_cover.fs");

  HYUZU_UE_Pak* pak = HYUZU_Pak_Load("oops.pak");
  HYUZU_Song* song = new HYUZU_Song;

  song = HYUZU_Song_Load(pak);

  Texture2D tex = LoadTextureFromImage(song->cover);
  Texture2D mask = LoadTexture("data/cover_mask.png");

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
              DrawGrid(100, 1.0f);
          EndMode3D();
      EndTextureMode();

      BeginDrawing();
          ClearBackground(RAYWHITE);

          DrawTextureRec(target.texture, { 0, 0, (float)target.texture.width, (float)-target.texture.height }, { 0, 0 }, WHITE);

          BeginShaderMode(cover_shader);
              SetShaderValueTexture(cover_shader, GetShaderLocation(cover_shader, "texture0"), tex);
              DrawTexture(tex, 30, 30, WHITE);
              DrawTexture(mask, 30, 30, WHITE);
          EndShaderMode();

          DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, RED);

          DrawText(song->title.c_str(), 562, 50, 30, BLACK);
          DrawText(song->artist.c_str(), 562, 90, 20, BLACK);
          DrawText(TextFormat("BPM: %i", song->bpm), 562, 115, 20, BLACK);
          DrawText(TextFormat("Key: %s %s", HYUZU_Song_ParseKeyToString(song->key).c_str(), HYUZU_Song_ParseModeToString(song->mode).c_str()), 562, 140, 20, BLACK);
          DrawText(TextFormat("Genre: %s\n\nharharharharhahra", HYUZU_Song_ParseGenreToString(song->genre).c_str()), 562, 190, 20, BLACK);
      EndDrawing();
  }

  UnloadRenderTexture(target);
  UnloadShader(cover_shader);
  CloseWindow();
  
  delete pak;
  delete song;

  return 0;
}