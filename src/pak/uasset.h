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

namespace Hyuzu {
    namespace UE {
        namespace UAsset {
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

            struct AssetInfo {
              std::vector<NameInfo> names;
              std::vector<ImportInfo> imports;
            };

            AssetInfo ExtractUAsset(std::vector<uint8_t> data, size_t& offset_ua);
            void ExtractUExp(std::vector<uint8_t> data, AssetInfo info, std::unordered_map<std::string, std::vector<std::string>>& metadata, size_t& offset_ue);

            std::vector<float> ConvertStringsToFloats(std::vector<std::string> strings);
        }
    }
}