#include "pak.h"

HYUZU_UE_Pak* HYUZU_Pak_Load(char* path) {
  FILE *pak_file = fopen(path, "rb");

  HYUZU_UE_Pak* pak = (HYUZU_UE_Pak*)malloc(sizeof(HYUZU_UE_Pak));
      
  if (pak_file == NULL) {
      perror("[HYUZU] Error. Failed to load PAK File.");
  }

  pak->serialize(pak_file);
  pak->directory.serialize(pak_file, pak->index_offset);
  pak->serialize_entries(pak_file);

  fclose(pak_file);

  return pak;
}