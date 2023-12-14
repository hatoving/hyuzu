#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>

#include "pak/pak.h"
#include "song.h"

int main() {
  HYUZU_UE_Pak* pak = HYUZU_Pak_Load("amadeus.pak");
  HYUZU_Song* song = new HYUZU_Song;

  song = HYUZU_Song_Load(pak);
  printf(song->info.short_name.c_str());
  
  delete pak;
  delete song;

  return 0;
}