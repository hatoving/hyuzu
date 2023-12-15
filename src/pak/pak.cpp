#include "pak.h"

HYUZU_UE_Pak* HYUZU_Pak_Load(std::string path) {
  std::ifstream pak_file(path, std::ios::binary);
  HYUZU_UE_Pak* pak = new HYUZU_UE_Pak;
      
  if (!pak_file.is_open()) {
      perror("[HYUZU] Error. Failed to load PAK File.");
  }

  pak->serialize(pak_file);
  pak->directory.serialize(pak_file, pak->index_offset);
  pak->serialize_entries(pak_file);

  pak_file.close();
  return pak;
}