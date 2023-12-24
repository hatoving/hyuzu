#include "ue_types.h"

std::unordered_map<std::string, std::vector<std::string>> Hyuzu::UE::GetMetadataFromFile(std::vector<uint8_t> uasset, std::vector<uint8_t> uexp) {
  std::unordered_map<std::string, std::vector<std::string>> metadata;

  size_t pos_uasset = 0;
  size_t pos_uexp = 0;
  
  Hyuzu::UE::UAsset::AssetInfo info = Hyuzu::UE::UAsset::ExtractUAsset(uasset, pos_uasset);
  Hyuzu::UE::UAsset::ExtractUExp(uexp, info, metadata, pos_uexp);

  return metadata;
}