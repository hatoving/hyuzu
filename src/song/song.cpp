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

    std::vector<std::unordered_map<std::string, std::vector<std::string>>> metadata = HYUZU_Song_ReadMetadata(song, pak);

    song->title = HYUZU_Song_ReadValueFromMetadata(metadata, "Title")[0];
    song->artist = HYUZU_Song_ReadValueFromMetadata(metadata, "Artist")[0];
    song->year = std::stoi(HYUZU_Song_ReadValueFromMetadata(metadata, "Year")[0]);
    song->bpm = std::stoi(HYUZU_Song_ReadValueFromMetadata(metadata, "BPM")[0]);

    song->creator = "";

    song->key = HYUZU_Song_ParseStringToKey(HYUZU_Song_ReadValueFromMetadata(metadata, "Key")[0].replace(0, 6, ""));
    HYUZU_Song_TransposeKeys(song, 7);
    song->mode = HYUZU_Song_ParseStringToMode(HYUZU_Song_ReadValueFromMetadata(metadata, "Mode")[0].replace(0, 10, ""));

    song->beat.pickups = HYUZU_UE_ConvertStringsToFloats(HYUZU_Song_ReadValueFromMetadata(metadata, "PickupBeats", 1));
    song->bass.pickups = HYUZU_UE_ConvertStringsToFloats(HYUZU_Song_ReadValueFromMetadata(metadata, "PickupBeats", 2));
    song->loop.pickups = HYUZU_UE_ConvertStringsToFloats(HYUZU_Song_ReadValueFromMetadata(metadata, "PickupBeats", 3));
    song->lead.pickups = HYUZU_UE_ConvertStringsToFloats(HYUZU_Song_ReadValueFromMetadata(metadata, "PickupBeats", 4));

    std::string texts = "{ ";
    for (int i = 0; i < song->beat.pickups.size(); i++) {
        if (i != song->beat.pickups.size() - 1) texts += std::to_string(song->beat.pickups[i]) + ", ";
        else texts += std::to_string(song->beat.pickups[i]);
    }
    texts += " }";

    printf("%s\n", texts.c_str());

    song->genre = HYUZU_Song_ParseStringToGenre((HYUZU_Song_ReadValueFromMetadata(metadata, "Genre")[0].replace(0, 8, "")));
}

void HYUZU_Song_SetupClip(HYUZU_Song_Clip& clip, std::unordered_map<std::string, std::vector<std::string>> metadata) {
    if (metadata.size() != 0) {
        clip.pickups = HYUZU_UE_ConvertStringsToFloats(metadata["PickupBeats"]);
    }
}

#pragma region Reading Data From Metadatas

std::vector<std::unordered_map<std::string, std::vector<std::string>>> HYUZU_Song_ReadMetadata(HYUZU_Song* song, HYUZU_UE_Pak* pak) {
    std::string path = "DLC/Songs/" + song->short_name + "/Meta_" + song->short_name;
    std::unordered_map<std::string, std::vector<std::string>> metadata = HYUZU_UE_GetMetadataFromFile(pak->entries[path + ".uasset"]->data,
                                                                                                      pak->entries[path + ".uexp"]->data);

    path = "Audio/Songs/" + song->short_name + "/" + song->short_name + "bs/Meta_" + song->short_name + "bs";
    std::unordered_map<std::string, std::vector<std::string>> bass_metadata = HYUZU_UE_GetMetadataFromFile(pak->entries[path + ".uasset"]->data,
                                                                                                           pak->entries[path + ".uexp"]->data);
    
    path = "Audio/Songs/" + song->short_name + "/" + song->short_name + "bt/Meta_" + song->short_name + "bt";
    std::unordered_map<std::string, std::vector<std::string>> beat_metadata = HYUZU_UE_GetMetadataFromFile(pak->entries[path + ".uasset"]->data,
                                                                                                           pak->entries[path + ".uexp"]->data);

    path = "Audio/Songs/" + song->short_name + "/" + song->short_name + "lp/Meta_" + song->short_name + "lp";
    std::unordered_map<std::string, std::vector<std::string>> loop_metadata = HYUZU_UE_GetMetadataFromFile(pak->entries[path + ".uasset"]->data,
                                                                                                           pak->entries[path + ".uexp"]->data);

    path = "Audio/Songs/" + song->short_name + "/" + song->short_name + "ld/Meta_" + song->short_name + "ld";
    std::unordered_map<std::string, std::vector<std::string>> lead_metadata = HYUZU_UE_GetMetadataFromFile(pak->entries[path + ".uasset"]->data,
                                                                                                           pak->entries[path + ".uexp"]->data);

    return {metadata, beat_metadata, bass_metadata, loop_metadata, lead_metadata};
}

std::vector<std::string> HYUZU_Song_ReadValueFromMetadata(std::vector<std::unordered_map<std::string, std::vector<std::string>>> metadata, std::string index) {
    std::vector<std::string> vector;
    for (int i = 0; i < metadata.size(); i++) {
        if (metadata[i].count(index) > 0) {
            vector = (metadata[i])[index];
        }
    }

    return vector;
}

std::vector<std::string> HYUZU_Song_ReadValueFromMetadata(std::vector<std::unordered_map<std::string, std::vector<std::string>>> metadata, std::string index, int index2) {
    if (metadata[index2].count(index) > 0) {
        return (metadata[index2])[index];
    } else {
        return std::vector<std::string>();
    }
}

#pragma endregion

#pragma region Parsing Enums

HYUZU_Song_Keys HYUZU_Song_ParseStringToKey(std::string key) {
    if (key == "C") {
        return HYUZU_Song_Keys::C;
    } else if (key == "Db") {
        return HYUZU_Song_Keys::Db;
    } else if (key == "D") {
        return HYUZU_Song_Keys::D;
    } else if (key == "Eb") {
        return HYUZU_Song_Keys::Eb;
    } else if (key == "E") {
        return HYUZU_Song_Keys::E;
    } else if (key == "F") {
        return HYUZU_Song_Keys::F;
    } else if (key == "Gb") {
        return HYUZU_Song_Keys::Gb;
    } else if (key == "G") {
        return HYUZU_Song_Keys::G;
    } else if (key == "Ab") {
        return HYUZU_Song_Keys::Ab;
    } else if (key == "A") {
        return HYUZU_Song_Keys::A;
    } else if (key == "Bb") {
        return HYUZU_Song_Keys::Bb;
    } else if (key == "B") {
        return HYUZU_Song_Keys::B;
    } else {
        // Handle the case where the string doesn't match any enum value
        throw std::invalid_argument("Invalid key string");
    }
};

std::string HYUZU_Song_ParseKeyToString(HYUZU_Song_Keys key) {
    switch (key) {
        case HYUZU_Song_Keys::C:
            return "C";
        case HYUZU_Song_Keys::Db:
            return "Db";
        case HYUZU_Song_Keys::D:
            return "D";
        case HYUZU_Song_Keys::Eb:
            return "Eb";
        case HYUZU_Song_Keys::E:
            return "E";
        case HYUZU_Song_Keys::F:
            return "F";
        case HYUZU_Song_Keys::Gb:
            return "Gb";
        case HYUZU_Song_Keys::G:
            return "G";
        case HYUZU_Song_Keys::Ab:
            return "Ab";
        case HYUZU_Song_Keys::A:
            return "A";
        case HYUZU_Song_Keys::Bb:
            return "Bb";
        case HYUZU_Song_Keys::B:
            return "B";
        default:
            throw std::invalid_argument("Invalid HYUZU_Song_Keys enum value");
    }
}

HYUZU_Song_Modes HYUZU_Song_ParseStringToMode(std::string mode) {
    if (mode == "Major") {
        return HYUZU_Song_Modes::Major;
    } else if (mode == "Minor") {
        return HYUZU_Song_Modes::Minor;
    } else {
        // Handle the case where the string doesn't match any enum value
        throw std::invalid_argument("Invalid mode string");
    }
};

std::string HYUZU_Song_ParseModeToString(HYUZU_Song_Modes mode) {
    switch (mode) {
        case HYUZU_Song_Modes::Major:
            return "Major";
        case HYUZU_Song_Modes::Minor:
            return "Minor";
        default:
            throw std::invalid_argument("Invalid HYUZU_Song_Modes enum value");
    }
}

HYUZU_Song_Genres HYUZU_Song_ParseStringToGenre(std::string genre) {
    if (genre == "Classical") {
        return HYUZU_Song_Genres::Classical;
    } else if (genre == "Country") {
        return HYUZU_Song_Genres::Country;
    } else if (genre == "Rock") {
        return HYUZU_Song_Genres::Rock;
    } else if (genre == "RnB") {
        return HYUZU_Song_Genres::RnB;
    } else if (genre == "HipHop") {
        return HYUZU_Song_Genres::HipHop;
    } else if (genre == "LatinAndCarribean") {
        return HYUZU_Song_Genres::International;
    } else if (genre == "Dance") {
        return HYUZU_Song_Genres::Dance;
    } else if (genre == "Pop") {
        return HYUZU_Song_Genres::Pop;
    }      else {
        // Handle the case where the string doesn't match any enum value
        throw std::invalid_argument("Invalid genre string");
    }
};

std::string HYUZU_Song_ParseGenreToString(HYUZU_Song_Genres mode) {
    switch (mode) {
        case HYUZU_Song_Genres::Classical:
            return "Classical";
        case HYUZU_Song_Genres::Country:
            return "Country";
        case HYUZU_Song_Genres::Rock:
            return "Rock";
        case HYUZU_Song_Genres::RnB:
            return "RnB";
        case HYUZU_Song_Genres::HipHop:
            return "HipHop";
        case HYUZU_Song_Genres::International:
            return "International";
        case HYUZU_Song_Genres::Dance:
            return "Dance";
        case HYUZU_Song_Genres::Pop:
            return "Pop";
        default:
            throw std::invalid_argument("Invalid HYUZU_Song_Genres enum value");
    }
}

#pragma endregion

void HYUZU_Song_TransposeKeys(HYUZU_Song* song, int key) {
    song->transposes.clear();

    for (int i = 0; i < 12; i++) {
        int offset = i - key;

        if (offset < -6) {
            offset += 12;
        } else if (offset > 6) {
            offset -= 6;
        }

        song->transposes.push_back(offset);
    }
}