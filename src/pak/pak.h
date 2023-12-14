#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <vector>

struct HYUZU_UE_PakEntry {
  int64_t size;
  int64_t uncompressed_size;
  int32_t compression_method_index;
  uint8_t sha_hash[20];

  uint32_t null;
  uint8_t data;

  void serialize(FILE* file) {
    long pos = ftell(file);
    fseek(file, info.offset, SEEK_SET);
    
    fread(&size, sizeof(int64_t), 1, file);
    fread(&uncompressed_size, sizeof(int64_t), 1, file);
    fread(&compression_method_index, sizeof(int32_t), 1, file);
    fread(&sha_hash, sizeof(uint8_t[20]), 1, file);

    fread(&null, 5, 1, file);
    fread(&data, size, 1, file);

    fseek(file, pos, SEEK_SET);
  }

  struct Info {
    uint32_t name_size;

    std::string name;
    uint8_t null;

    uint64_t offset;

    uint64_t compressed_size;
    uint64_t size;
    uint32_t compression_type;

    uint8_t unk[20];
    uint32_t unk2;
    uint8_t unk3;

    void serialize(FILE* file) {
      fread(&name_size, sizeof(uint32_t), 1, file);
      if (name_size < 0) name_size = name_size * -2;

      char* name_c = (char*)malloc(name_size);
      fread(name_c, name_size - 1, 1, file);
      name = std::string(name_c);

      fread(&null, 1, 1, file);
      fread(&offset, sizeof(uint64_t), 1, file);

      fread(&compressed_size, sizeof(uint64_t), 1, file);
      fread(&size, sizeof(uint64_t), 1, file);

      fread(&compression_type, sizeof(uint32_t), 1, file);

      fread(&unk, 20, 1, file);
      fread(&unk2, 4, 1, file);
      fread(&unk3, 1, 1, file);
    }
  };
  Info info;
};

struct HYUZU_UE_Directory {
  uint32_t dir_name_size;
  std::string dir_name;

  uint8_t null;
  uint32_t amount_of_files;

  void serialize(FILE* file, int64_t offset) {
    fseek(file, offset, SEEK_SET);

    fread(&dir_name_size, sizeof(uint32_t), 1, file);

    char* dir_name_c = (char*)malloc(dir_name_size);
    fread(dir_name_c, dir_name_size - 1, 1, file);
    dir_name = std::string(dir_name_c);

    fread(&null, 1, 1, file);
    fread(&amount_of_files, sizeof(uint32_t), 1, file);
  }
};

struct HYUZU_UE_Pak {
  static const uint32_t offset = 221;

  char guid[16];
  bool is_encrypted;
  uint32_t magic;
  int version;

  int64_t index_offset;
  int64_t index_size;

  uint8_t sha_hash[20];
  bool is_frozen = false;
  char compressed_name[32];
  
  void serialize(FILE* file) {
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, file_size - (int)offset, SEEK_SET);

    fread(&guid, sizeof(char[16]), 1, file);
    fread(&is_encrypted, sizeof(bool), 1, file);
    fread(&magic, sizeof(uint32_t), 1, file);

    if (magic != 0x5A6F12E1) {
      printf("[HYUZU] Error. Magic invalid.\n");
    }

    fread(&version, sizeof(int), 1, file);

    fread(&index_offset, sizeof(int64_t), 1, file);
    fread(&index_size, sizeof(int64_t), 1, file);

    fread(&sha_hash, sizeof(uint8_t[20]), 1, file);
    
    //if the pak version is frozen
    if (version == 9) {
      fread(&is_frozen, sizeof(bool), 1, file);
    }

    fread(&compressed_name, sizeof(char[30]), 1, file);
  }
  HYUZU_UE_Directory directory;
  std::vector<HYUZU_UE_PakEntry*> entries;

  void serialize_entries(FILE* file) {
    for (int i = 0; i < (int)directory.amount_of_files; i++)
    {
      HYUZU_UE_PakEntry* entry = (HYUZU_UE_PakEntry*)malloc(sizeof(HYUZU_UE_PakEntry));

      entry->info.serialize(file);
      entry->serialize(file);

      entries.push_back(entry);
    }
  }
};

HYUZU_UE_Pak* HYUZU_Pak_Load(char* path);