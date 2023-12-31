#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>

#include <raylib.h>

#include "pak/pak.h"
#include "song/song.h"

#include "game/audio.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

int main() {
  //SetTraceLogLevel(LOG_WARNING | LOG_ERROR);
  InitWindow(1280, 720, "raylib [core] example - basic window");
  InitAudioDevice();

  Shader cover_shader = LoadShader(0, "data/shaders/shd_album_cover.fs");

  float radius = 0.25f;
  SetShaderValue(cover_shader, GetShaderLocation(cover_shader, "corner_scale"), &radius, SHADER_UNIFORM_FLOAT);

  Hyuzu::Pak::PakFile* pak = Hyuzu::Pak::LoadPak("paks/fake.pak");
  Hyuzu::Song::HYUZU_Song* song = new Hyuzu::Song::HYUZU_Song;

  song = Hyuzu::Song::Load(pak);
  Sound lead = LoadSound("data/LEAD.ogg");

  Texture2D tex = LoadTextureFromImage(song->cover);
  Texture2D mask = LoadTexture("data/cover_mask.png");

  SetTargetFPS(60);
  PlayAudio(lead);

  while (!WindowShouldClose())
  {
        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginShaderMode(cover_shader);
                DrawTexture(tex, 30, 30, WHITE);
                DrawTexture(mask, 30, 30, WHITE);
            EndShaderMode();

            DrawText(song->title.c_str(), 562, 50, 30, BLACK);
            DrawText(song->artist.c_str(), 562, 90, 20, BLACK);

            DrawText(TextFormat("BPM: %i", song->bpm), 562, 115, 20, BLACK);
            DrawText(TextFormat("Key: %s %s", Hyuzu::Song::ParseKeyToString(song->key).c_str(), Hyuzu::Song::ParseModeToString(song->mode).c_str()), 562, 140, 20, BLACK);
            DrawText(TextFormat("Genre: %s\n\nharharharharhahra", Hyuzu::Song::ParseGenreToString(song->genre).c_str()), 562, 190, 20, BLACK);

            if (IsKeyPressed(KEY_UP)) { 
                UpdatePitchAndFormantByValue(lead, 1.0f);
                printf("pitch up\n");
            }
            if (IsKeyPressed(KEY_DOWN)) {
                UpdatePitchAndFormantByValue(lead, -1.0f);
                printf("pitch down\n");
            }

            DrawFPS(20, 20);

        EndDrawing();
  }

  UnloadShader(cover_shader);
  StopSound(lead);
  UnloadSound(lead);

  CloseAudioDevice();
  CloseWindow();
  
  delete pak;
  delete song;

  return 0;
}