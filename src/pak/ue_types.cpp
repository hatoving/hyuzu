#include "ue_types.h"

std::unordered_map<std::string, std::vector<std::string>> HYUZU_UE_GetMetadataFromFile(std::vector<uint8_t> uasset, std::vector<uint8_t> uexp) {
  std::unordered_map<std::string, std::vector<std::string>> metadata;

  size_t pos_uasset = 0;
  size_t pos_uexp = 0;
  
  UEAssetInfo info = HYUZU_UE_ExtractUAsset(uasset, pos_uasset);
  HYUZU_UE_ExtractUExp(uexp, info, metadata, pos_uexp);

  return metadata;
}