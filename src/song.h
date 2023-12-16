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
    std::string short_name;
    std::string creator;

    std::string song_name;
    std::string artist;

    enum class HYUZU_Song_Genres {
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
    } genre;

    std::string sub_genre;

    int year;
    int bpm;

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
    } key;

    enum class Modes {
        Major,
        Minor
    } mode;

    Image cover;
    HYUZU_UE_Texture2D* ue_texture = new HYUZU_UE_Texture2D;

  ~HYUZU_Song() {
    delete ue_texture;
  }
};

HYUZU_Song* HYUZU_Song_Load(HYUZU_UE_Pak* pak);
void HYUZU_Song_GetMetadata(HYUZU_UE_Pak* pak, HYUZU_Song* song);