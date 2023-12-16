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
          printf("It's small! %s\n", e->info.name.c_str());
          std::string name;

          for (size_t i = pos + 10; i != '\0'; i++)
          {
              if (e->info.name[i] == '/') {
                break;
              }

              name += e->info.name[i];
          }
          if (e->info.name == ("DLC/Songs/" + name + "/Meta_" + name + ".uasset")) {
            printf("Name matches!\n");
            song->info.short_name = name;
          }
      }

      pos = e->info.name.find("UI/AlbumArt");

      if (pos != std::string::npos) {
        if (e->info.name.compare(e->info.name.length() - 5, 5, ".uexp") == 0) {
          pos = e->info.name.find("_small");

          if (pos != std::string::npos) {
              song->info.cover->serialize(e->data);

              song->info.cover_img = {
                  0,
                  (int)song->info.cover->mips[0].width,
                  (int)song->info.cover->mips[0].height,
                  1,
                  PIXELFORMAT_COMPRESSED_DXT1_RGB
              };

              // FUCK YEAAHHHH!!!
              song->info.cover_img.data = RL_MALLOC(song->info.cover->mips[0].data.size());
              memcpy(song->info.cover_img.data, song->info.cover->mips[0].data.data(), (unsigned int)song->info.cover->mips[0].data.size());
              //ExportImage(rawImage, "tex_exported.png");
          }
        }
      }
  }
}