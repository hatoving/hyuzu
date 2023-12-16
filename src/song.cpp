#include "song.h"

HYUZU_Song* HYUZU_Song_Load(HYUZU_UE_Pak* pak) {
  HYUZU_Song* song = new HYUZU_Song;
  HYUZU_Song_GetMetadata(pak, song);

  return song;
}

void HYUZU_Song_GetMetadata(HYUZU_UE_Pak* pak, HYUZU_Song* song) {
    for (const auto &e : pak->entries)
    {
          auto pos = e.second->info.name.find("DLC/Songs/");

          if (pos != std::string::npos) {
              std::string name;

              for (size_t i = pos + 10; i != '\0'; i++)
              {
                  if (e.second->info.name[i] == '/') {
                    break;
                  }

                  name += e.second->info.name[i];
              }
              if (e.second->info.name == ("DLC/Songs/" + name + "/Meta_" + name + ".uasset")) {
                song->short_name = name;
              }
          }

          pos = e.second->info.name.find("UI/AlbumArt");

          if (pos != std::string::npos) {
            if (e.second->info.name.compare(e.second->info.name.length() - 5, 5, ".uexp") == 0) {
              pos = e.second->info.name.find("_small");

                if (pos != std::string::npos) {
                    song->ue_texture->serialize(e.second->data);

                    song->cover = {
                        0,
                        (int)song->ue_texture->mips[0].width,
                        (int)song->ue_texture->mips[0].height,
                        1,
                        PIXELFORMAT_COMPRESSED_DXT1_RGB
                    };

                    // FUCK YEAAHHHH!!!
                    song->cover.data = RL_MALLOC(song->ue_texture->mips[0].data.size());
                    memcpy(song->cover.data, song->ue_texture->mips[0].data.data(), (unsigned int)song->ue_texture->mips[0].data.size());
                    //ExportImage(rawImage, "tex_exported.png");
                }
            }
        }
    }

    std::unordered_map<std::string, std::any> metadata = HYUZU_UE_GetMetadataFromFile(pak->entries["DLC/Songs/" + song->short_name + "/Meta_" + song->short_name + ".uasset"]->data,
                                                         pak->entries["DLC/Songs/" + song->short_name + "/Meta_" + song->short_name + ".uexp"]->data);
}