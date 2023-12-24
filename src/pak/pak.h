#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

namespace Hyuzu {
    namespace Pak {
      struct PakFileEntry {
      uint64_t null1;

      int64_t size;
      int64_t uncompressed_size;
      int32_t compression_method_index;
      uint8_t sha_hash[20];

      uint32_t null;
      std::vector<uint8_t> data;

      void serialize(std::ifstream& file) {
        std::streampos pos = file.tellg();
        file.seekg(info.offset, std::ios::beg);

        uint64_t null1;
        file.read(reinterpret_cast<char*>(&null1), sizeof(uint64_t));
        
        file.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));
        file.read(reinterpret_cast<char*>(&uncompressed_size), sizeof(uint64_t));
        file.read(reinterpret_cast<char*>(&compression_method_index), sizeof(uint32_t));
        file.read(reinterpret_cast<char*>(&sha_hash), sizeof(uint8_t) * 20);

        file.read(reinterpret_cast<char*>(&null), 5);

        std::vector<uint8_t> data_vector(info.size);
        file.read(reinterpret_cast<char*>(data_vector.data()), info.size);

        data = data_vector;
        file.seekg(pos, std::ios::beg);
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

        void serialize(std::ifstream& file) {
          file.read(reinterpret_cast<char*>(&name_size), sizeof(name_size));
          if (name_size < 0) name_size = name_size * -2;

          char* name_c = (char*)malloc(name_size - 1);
          file.read(name_c, name_size - 1);

          name = std::string(name_c);
          name.erase(name.size() - (name.size() - (name_size - 1)), (name.size() - (name_size - 1)));

          file.read(reinterpret_cast<char*>(&null), 1);
          file.read(reinterpret_cast<char*>(&offset), sizeof(uint64_t));

          file.read(reinterpret_cast<char*>(&compressed_size), sizeof(uint64_t));
          file.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));

          file.read(reinterpret_cast<char*>(&compression_type), sizeof(uint32_t));

          file.read(reinterpret_cast<char*>(&unk), sizeof(uint8_t) * 20);
          file.read(reinterpret_cast<char*>(&unk2), sizeof(uint32_t));
          file.read(reinterpret_cast<char*>(&unk3), 1);
        }
      };
      Info info;
    };

    struct Directory {
      uint32_t dir_name_size;
      std::string dir_name;

      uint8_t null;
      uint32_t amount_of_files;

      void serialize(std::ifstream& file, int64_t offset) {
          file.seekg((int)offset, std::ios::beg);

          file.read(reinterpret_cast<char*>(&dir_name_size), sizeof(dir_name_size));

          char* name_buffer = new char[dir_name_size - 1];

          file.read(name_buffer, dir_name_size - 1);
          dir_name = std::string(name_buffer);

          file.read(reinterpret_cast<char*>(&null), 1);
          file.read(reinterpret_cast<char*>(&amount_of_files), sizeof(amount_of_files));
        }
      };

      struct PakFile {
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
        
        void serialize(std::ifstream& file) {
          file.seekg(0, std::ios::end);
          long file_size = file.tellg();
          file.seekg(0, std::ios::beg);

          file.seekg((int)file_size - (int)offset, std::ios::beg);

          file.read(guid, sizeof(guid));
          file.read(reinterpret_cast<char*>(&is_encrypted), sizeof(is_encrypted));
          file.read(reinterpret_cast<char*>(&magic), sizeof(magic));

          if (magic != 0x5A6F12E1) {
            printf("[HYUZU] Error. Magic invalid.\n");
          }

          file.read(reinterpret_cast<char*>(&version), sizeof(version));

          file.read(reinterpret_cast<char*>(&index_offset), sizeof(index_offset));
          file.read(reinterpret_cast<char*>(&index_size), sizeof(index_size));

          file.read(reinterpret_cast<char*>(&sha_hash), sizeof(sha_hash));
          
          //if the pak version is frozen
          if (version == 9) {
            file.read(reinterpret_cast<char*>(&is_frozen), sizeof(is_frozen));
          }

          file.read(compressed_name, sizeof(compressed_name));
        }
        Directory directory;
        std::unordered_map<std::string, PakFileEntry*> entries;

        void serialize_entries(std::ifstream& file) {
          for (int i = 0; i < (int)directory.amount_of_files; i++)
          {
            PakFileEntry* entry = new PakFileEntry;

            entry->info.serialize(file);
            entry->serialize(file);

            entries[entry->info.name] = entry;
          }
        }
      };

      PakFile* LoadPak(std::string path);
    }
}