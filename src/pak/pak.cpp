#include "pak.h"

Hyuzu::Pak::PakFile* Hyuzu::Pak::LoadPak(std::string path) {
  std::ifstream pak_file(path, std::ios::binary);
  PakFile* pak = new PakFile;
      
  if (!pak_file.is_open()) {
      perror("[HYUZU] Error. Failed to load PAK File.");
  }

  pak->serialize(pak_file);
  pak->directory.serialize(pak_file, pak->index_offset);
  pak->serialize_entries(pak_file);

  pak_file.close();
  return pak;
}