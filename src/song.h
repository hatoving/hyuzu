#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "pak/pak.h"
#include "pak/ue_types.h"

#include <raylib.h>

struct HYUZU_Song {
  struct BasicInfo {
    std::string short_name;

    HYUZU_UE_Texture2D* cover = new HYUZU_UE_Texture2D;
    Image cover_img;
  };
  BasicInfo info;

  ~HYUZU_Song() {
    delete info.cover;
  }
};

HYUZU_Song* HYUZU_Song_Load(HYUZU_UE_Pak* pak);
void HYUZU_Song_GetMetadata(HYUZU_UE_Pak* pak, HYUZU_Song* song);