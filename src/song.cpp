#include "song.h"

HYUZU_Song* HYUZU_Song_Load(HYUZU_UE_Pak* pak) {
  HYUZU_Song* song = new HYUZU_Song;
  HYUZU_Song_GetMetadata(pak, song);

  return song;
}

void HYUZU_Song_GetMetadata(HYUZU_UE_Pak* pak, HYUZU_Song* song) {
  for (auto &&e : pak->entries)
  {
      auto pos = e->info.name.find("DLC/Songs/");

      if (pos != std::string::npos) {
          std::string name;

          for (size_t i = pos + 10; i != '\0'; i++)
          {
              if (e->info.name[i] == '/') {
                break;
              }

              name += e->info.name[i];
          }

          if (e->info.name == ("DLC/Songs/" + name + "/Meta_" + name + ".uasset")) {
            song->info.short_name = name;
          }
      }

      pos = e->info.name.find("UI/AlbumArt");

      if (pos != std::string::npos) {
        if (e->info.name.compare(e->info.name.length() - 5, 5, ".uexp") == 0) {
          
        }
      }
  }
}