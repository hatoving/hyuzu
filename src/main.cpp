#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>

#include <raylib.h>

#include "pak/pak.h"
#include "song.h"

int main() {
  InitWindow(1280, 720, "raylib [core] example - basic window");

  HYUZU_UE_Pak* pak = HYUZU_Pak_Load("fake.pak");
  HYUZU_Song* song = new HYUZU_Song;

  song = HYUZU_Song_Load(pak);
  Texture2D tex = LoadTextureFromImage(song->cover);

  SetTargetFPS(60);

  while (!WindowShouldClose())
  {
      BeginDrawing();
      ClearBackground(RAYWHITE);   

      DrawTexture(tex, 30, 30, WHITE);

      DrawText(song->title.c_str(), 562, 30, 30, BLACK);
      DrawText(song->artist.c_str(), 562, 60, 20, BLACK);
      DrawText(TextFormat("BPM: %i", song->bpm), 562, 85, 20, BLACK);
      DrawText(TextFormat("Key: %s %s", HYUZU_Song_ParseKeyToString(song->key).c_str(), HYUZU_Song_ParseModeToString(song->mode).c_str()), 562, 110, 20, BLACK);

      DrawText(TextFormat("Creator: %s", song->creator.c_str()), 562, 170, 20, BLACK);

      EndDrawing();
  }

  CloseWindow();
  
  delete pak;
  delete song;

  return 0;
}