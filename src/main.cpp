#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>

#include <raylib.h>

#include "pak/pak.h"
#include "song.h"

int main() {
  InitWindow(1280, 720, "raylib [core] example - basic window");

  HYUZU_UE_Pak* pak = HYUZU_Pak_Load("amadeus.pak");
  HYUZU_Song* song = new HYUZU_Song;

  song = HYUZU_Song_Load(pak);
  Texture2D tex = LoadTextureFromImage(song->cover);

  SetTargetFPS(60);

  while (!WindowShouldClose())
  {
      BeginDrawing();
      ClearBackground(RAYWHITE);    
      DrawTexture(tex, 10, 10, WHITE);
      EndDrawing();
  }

  CloseWindow();
  
  delete pak;
  delete song;

  return 0;
}