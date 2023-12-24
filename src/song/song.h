#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "../pak/pak.h"
#include "../pak/ue_types.h"

#include "mogg.h"
#include <raylib.h>

namespace Hyuzu {
    namespace Song {
        enum class Keys {
            C = 0,
            Db = 1,
            D = 2,
            Eb = 3,
            E = 4,
            F = 5,
            Gb = 6,
            G = 7,
            Ab = 8,
            A = 9,
            Bb = 10,
            B = 11,
        };

        enum class Modes {
            Major,
            Minor
        };

        enum class Genres {
            Classical,
            Country,
            Rock,
            Pop,
            RnB,
            HipHop,
            Dance,
            International,
            AlternativeIndie,
            Soundtrack, //used for games, movies, etc.
            Misc
        };

        enum class KeymapPreset {
            Major,
            Minor,
            Shared,
            Custom
        };

        enum class Instruments {
            None,
            Guitar,
            Drums,
            Vocal,
            Synth,
            Sampler,
            Horns,
            Strings
        };

        struct Keyzone {
            std::string label;
            int index;
            bool unpitched;
            KeymapPreset preset;
        };

        struct Clip {
            std::vector<Hyuzu::MOGG::MOGG> moggs;

            std::vector<Keyzone> keyzones_clips;
            std::vector<Keyzone> keyzones_risers;

            std::vector<float> pickups;
            Instruments instrument = Instruments::None;
        };

        struct HYUZU_Song {
            std::string short_name;
            std::string creator;

            std::string title;
            std::string artist;

            Genres genre;
            std::string sub_genre;

            int year;
            int bpm;

            Keys key;
            std::vector<int> transposes;
            Modes mode;

            Clip beat;
            Clip bass;
            Clip loop;
            Clip lead;

            Image cover;
            Hyuzu::UE::Texture2D* ue_texture = new Hyuzu::UE::Texture2D;

            ~HYUZU_Song() {
                delete ue_texture;
            }
        };

        HYUZU_Song* Load(Hyuzu::Pak::PakFile* pak);

        void GetMetadata(Hyuzu::Pak::PakFile* pak, HYUZU_Song* song);
        std::vector<std::unordered_map<std::string, std::vector<std::string>>> ReadMetadata(HYUZU_Song* song, Hyuzu::Pak::PakFile* pak);

        std::vector<std::string> ReadValueFromMetadata(std::vector<std::unordered_map<std::string, std::vector<std::string>>> metadata, std::string index);
        std::vector<std::string> ReadValueFromMetadata(std::vector<std::unordered_map<std::string, std::vector<std::string>>> metadata, std::string index, int index2);

        Keys ParseStringToKey(std::string key);
        std::string ParseKeyToString(Keys key);

        Modes ParseStringToMode(std::string mode);
        std::string ParseModeToString(Modes mode);

        Genres ParseStringToGenre(std::string genre);
        std::string ParseGenreToString(Genres genre);

        void TransposeKeys(HYUZU_Song* song, int key);
        void SetupClip(Hyuzu::Song::Clip& clip, std::unordered_map<std::string, std::vector<std::string>> metadata);
    }
}