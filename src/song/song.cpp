#include "song.h"

Hyuzu::Song::HYUZU_Song* Hyuzu::Song::Load(Hyuzu::Pak::PakFile* pak) {
  HYUZU_Song* song = new HYUZU_Song;
  GetMetadata(pak, song);

  return song;
}

void Hyuzu::Song::GetMetadata(Hyuzu::Pak::PakFile* pak, Hyuzu::Song::HYUZU_Song* song) {
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

    std::vector<std::unordered_map<std::string, std::vector<std::string>>> metadata = Hyuzu::Song::ReadMetadata(song, pak);

    song->title = Hyuzu::Song::ReadValueFromMetadata(metadata, "Title")[0];
    song->artist = Hyuzu::Song::ReadValueFromMetadata(metadata, "Artist")[0];
    song->year = std::stoi(Hyuzu::Song::ReadValueFromMetadata(metadata, "Year")[0]);
    song->bpm = std::stoi(Hyuzu::Song::ReadValueFromMetadata(metadata, "BPM")[0]);

    song->creator = "";

    song->key = Hyuzu::Song::ParseStringToKey(Hyuzu::Song::ReadValueFromMetadata(metadata, "Key")[0].replace(0, 6, ""));
    Hyuzu::Song::TransposeKeys(song, 7);
    song->mode = Hyuzu::Song::ParseStringToMode(Hyuzu::Song::ReadValueFromMetadata(metadata, "Mode")[0].replace(0, 10, ""));

    song->beat.pickups = Hyuzu::UE::UAsset::ConvertStringsToFloats(Hyuzu::Song::ReadValueFromMetadata(metadata, "PickupBeats", 1));
    song->bass.pickups = Hyuzu::UE::UAsset::ConvertStringsToFloats(Hyuzu::Song::ReadValueFromMetadata(metadata, "PickupBeats", 2));
    song->loop.pickups = Hyuzu::UE::UAsset::ConvertStringsToFloats(Hyuzu::Song::ReadValueFromMetadata(metadata, "PickupBeats", 3));
    song->lead.pickups = Hyuzu::UE::UAsset::ConvertStringsToFloats(Hyuzu::Song::ReadValueFromMetadata(metadata, "PickupBeats", 4));

    std::string texts = "{ ";
    for (int i = 0; i < song->beat.pickups.size(); i++) {
        if (i != song->beat.pickups.size() - 1) texts += std::to_string(song->beat.pickups[i]) + ", ";
        else texts += std::to_string(song->beat.pickups[i]);
    }
    texts += " }";

    printf("%s\n", texts.c_str());

    song->genre = Hyuzu::Song::ParseStringToGenre((Hyuzu::Song::ReadValueFromMetadata(metadata, "Genre")[0].replace(0, 8, "")));
}

void Hyuzu::Song::SetupClip(Hyuzu::Song::Clip& clip, std::unordered_map<std::string, std::vector<std::string>> metadata) {
    if (metadata.size() != 0) {
        clip.pickups = Hyuzu::UE::UAsset::ConvertStringsToFloats(metadata["PickupBeats"]);
    }
}

#pragma region Reading Data From Metadatas

std::vector<std::unordered_map<std::string, std::vector<std::string>>> Hyuzu::Song::ReadMetadata(Hyuzu::Song::HYUZU_Song* song, Hyuzu::Pak::PakFile* pak) {
    std::string path = "DLC/Songs/" + song->short_name + "/Meta_" + song->short_name;
    std::unordered_map<std::string, std::vector<std::string>> metadata = Hyuzu::UE::GetMetadataFromFile(pak->entries[path + ".uasset"]->data,
                                                                                                      pak->entries[path + ".uexp"]->data);

    path = "Audio/Songs/" + song->short_name + "/" + song->short_name + "bs/Meta_" + song->short_name + "bs";
    std::unordered_map<std::string, std::vector<std::string>> bass_metadata = Hyuzu::UE::GetMetadataFromFile(pak->entries[path + ".uasset"]->data,
                                                                                                           pak->entries[path + ".uexp"]->data);
    
    path = "Audio/Songs/" + song->short_name + "/" + song->short_name + "bt/Meta_" + song->short_name + "bt";
    std::unordered_map<std::string, std::vector<std::string>> beat_metadata = Hyuzu::UE::GetMetadataFromFile(pak->entries[path + ".uasset"]->data,
                                                                                                           pak->entries[path + ".uexp"]->data);

    path = "Audio/Songs/" + song->short_name + "/" + song->short_name + "lp/Meta_" + song->short_name + "lp";
    std::unordered_map<std::string, std::vector<std::string>> loop_metadata = Hyuzu::UE::GetMetadataFromFile(pak->entries[path + ".uasset"]->data,
                                                                                                           pak->entries[path + ".uexp"]->data);

    path = "Audio/Songs/" + song->short_name + "/" + song->short_name + "ld/Meta_" + song->short_name + "ld";
    std::unordered_map<std::string, std::vector<std::string>> lead_metadata = Hyuzu::UE::GetMetadataFromFile(pak->entries[path + ".uasset"]->data,
                                                                                                           pak->entries[path + ".uexp"]->data);

    return {metadata, beat_metadata, bass_metadata, loop_metadata, lead_metadata};
}

std::vector<std::string> Hyuzu::Song::ReadValueFromMetadata(std::vector<std::unordered_map<std::string, std::vector<std::string>>> metadata, std::string index) {
    std::vector<std::string> vector;
    for (int i = 0; i < metadata.size(); i++) {
        if (metadata[i].count(index) > 0) {
            vector = (metadata[i])[index];
        }
    }

    return vector;
}

std::vector<std::string> Hyuzu::Song::ReadValueFromMetadata(std::vector<std::unordered_map<std::string, std::vector<std::string>>> metadata, std::string index, int index2) {
    if (metadata[index2].count(index) > 0) {
        return (metadata[index2])[index];
    } else {
        return std::vector<std::string>();
    }
}

#pragma endregion

#pragma region Parsing Enums

Hyuzu::Song::Keys Hyuzu::Song::ParseStringToKey(std::string key) {
    if (key == "C") {
        return Hyuzu::Song::Keys::C;
    } else if (key == "Db") {
        return Hyuzu::Song::Keys::Db;
    } else if (key == "D") {
        return Hyuzu::Song::Keys::D;
    } else if (key == "Eb") {
        return Hyuzu::Song::Keys::Eb;
    } else if (key == "E") {
        return Hyuzu::Song::Keys::E;
    } else if (key == "F") {
        return Hyuzu::Song::Keys::F;
    } else if (key == "Gb") {
        return Hyuzu::Song::Keys::Gb;
    } else if (key == "G") {
        return Hyuzu::Song::Keys::G;
    } else if (key == "Ab") {
        return Hyuzu::Song::Keys::Ab;
    } else if (key == "A") {
        return Hyuzu::Song::Keys::A;
    } else if (key == "Bb") {
        return Hyuzu::Song::Keys::Bb;
    } else if (key == "B") {
        return Hyuzu::Song::Keys::B;
    } else {
        // Handle the case where the string doesn't match any enum value
        throw std::invalid_argument("Invalid key string");
    }
};

std::string Hyuzu::Song::ParseKeyToString(Hyuzu::Song::Keys key) {
    switch (key) {
        case Hyuzu::Song::Keys::C:
            return "C";
        case Hyuzu::Song::Keys::Db:
            return "Db";
        case Hyuzu::Song::Keys::D:
            return "D";
        case Hyuzu::Song::Keys::Eb:
            return "Eb";
        case Hyuzu::Song::Keys::E:
            return "E";
        case Hyuzu::Song::Keys::F:
            return "F";
        case Hyuzu::Song::Keys::Gb:
            return "Gb";
        case Hyuzu::Song::Keys::G:
            return "G";
        case Hyuzu::Song::Keys::Ab:
            return "Ab";
        case Hyuzu::Song::Keys::A:
            return "A";
        case Hyuzu::Song::Keys::Bb:
            return "Bb";
        case Hyuzu::Song::Keys::B:
            return "B";
        default:
            throw std::invalid_argument("Invalid Hyuzu::Song::Keys enum value");
    }
}

Hyuzu::Song::Modes Hyuzu::Song::ParseStringToMode(std::string mode) {
    if (mode == "Major") {
        return Hyuzu::Song::Modes::Major;
    } else if (mode == "Minor") {
        return Hyuzu::Song::Modes::Minor;
    } else {
        // Handle the case where the string doesn't match any enum value
        throw std::invalid_argument("Invalid mode string");
    }
};

std::string Hyuzu::Song::ParseModeToString(Hyuzu::Song::Modes mode) {
    switch (mode) {
        case Hyuzu::Song::Modes::Major:
            return "Major";
        case Hyuzu::Song::Modes::Minor:
            return "Minor";
        default:
            throw std::invalid_argument("Invalid Hyuzu::Song::Modes enum value");
    }
}

Hyuzu::Song::Genres Hyuzu::Song::ParseStringToGenre(std::string genre) {
    if (genre == "Classical") {
        return Hyuzu::Song::Genres::Classical;
    } else if (genre == "Country") {
        return Hyuzu::Song::Genres::Country;
    } else if (genre == "Rock") {
        return Hyuzu::Song::Genres::Rock;
    } else if (genre == "RnB") {
        return Hyuzu::Song::Genres::RnB;
    } else if (genre == "HipHop") {
        return Hyuzu::Song::Genres::HipHop;
    } else if (genre == "LatinAndCarribean") {
        return Hyuzu::Song::Genres::International;
    } else if (genre == "Dance") {
        return Hyuzu::Song::Genres::Dance;
    } else if (genre == "Pop") {
        return Hyuzu::Song::Genres::Pop;
    }      else {
        // Handle the case where the string doesn't match any enum value
        throw std::invalid_argument("Invalid genre string");
    }
};

std::string Hyuzu::Song::ParseGenreToString(Hyuzu::Song::Genres mode) {
    switch (mode) {
        case Hyuzu::Song::Genres::Classical:
            return "Classical";
        case Hyuzu::Song::Genres::Country:
            return "Country";
        case Hyuzu::Song::Genres::Rock:
            return "Rock";
        case Hyuzu::Song::Genres::RnB:
            return "RnB";
        case Hyuzu::Song::Genres::HipHop:
            return "HipHop";
        case Hyuzu::Song::Genres::International:
            return "International";
        case Hyuzu::Song::Genres::Dance:
            return "Dance";
        case Hyuzu::Song::Genres::Pop:
            return "Pop";
        default:
            throw std::invalid_argument("Invalid Hyuzu::Song::Genres enum value");
    }
}

#pragma endregion

void Hyuzu::Song::TransposeKeys(Hyuzu::Song::HYUZU_Song* song, int key) {
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