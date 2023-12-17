#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

#include "ue_types.h"

struct NameInfo {
    uint32_t size;
    std::string string;
    uint32_t flags;
};

struct ImportInfo {
  uint64_t parent_name_id;
  uint64_t class_id;
  uint32_t parent_import_id;
  uint32_t name_id;
  uint32_t unk_id;
};

struct UEAssetInfo {
  std::vector<NameInfo> names;
  std::vector<ImportInfo> imports;
};

UEAssetInfo HYUZU_UE_ExtractUAsset(std::vector<uint8_t> data, size_t& offset_ua);
void HYUZU_UE_ExtractUExp(std::vector<uint8_t> data, UEAssetInfo info, std::unordered_map<std::string, std::vector<std::string>>& metadata, size_t& offset_ue);

std::vector<float> HYUZU_UE_ConvertStringsToFloats(std::vector<std::string> strings);