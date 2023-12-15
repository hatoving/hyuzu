#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <iostream>
#include <vector>

#include "pak/pak.h"
#include "pak/ue_types.h"

struct HYUZU_Song {
  struct BasicInfo {
    std::string short_name;
  };
  BasicInfo info;
};

HYUZU_Song* HYUZU_Song_Load(HYUZU_UE_Pak* pak);
void HYUZU_Song_GetMetadata(HYUZU_UE_Pak* pak, HYUZU_Song* song);