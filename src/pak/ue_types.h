#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

template <typename T>
T HYUZU_UE_ReadValueFromVector(const std::vector<uint8_t>& data, size_t& offset, size_t size) {
    if (offset + size <= data.size()) {
        const T* valuePtr = reinterpret_cast<const T*>(&data[offset]);
        offset += size;
        return *valuePtr;
    } else {
        throw std::runtime_error("Not enough data in the vector");
    }
}

inline std::vector<uint8_t> HYUZU_UE_SplitVector(const std::vector<uint8_t>& originalVector, size_t& offset, size_t length) {
    if (offset >= originalVector.size()) {
        std::cerr << "Error: Offset is out of bounds." << std::endl;
        return std::vector<uint8_t>();
    }

    size_t endPos = offset + length;
    if (endPos > originalVector.size()) {
        std::cerr << "Warning: Length extends beyond the vector's end. Adjusting length." << std::endl;
        endPos = originalVector.size();
    }
    
    std::vector<uint8_t> subVector(originalVector.begin() + offset, originalVector.begin() + endPos);
    offset += (int)length;
    return subVector;
}


struct HYUZU_UE_Texture2D {
  size_t pos = 0;

  struct HYUZU_UE_Mip {
    uint64_t entry_identifier;
    uint32_t flags;
    uint32_t len_1;
    uint32_t len_2;
    uint64_t offset;
    std::vector<uint8_t> data;
    uint32_t width;
    uint32_t height;

    void serialize(std::vector<uint8_t>& d, size_t& pos) {
      entry_identifier = HYUZU_UE_ReadValueFromVector<uint32_t>(d, pos, 8);
      flags = HYUZU_UE_ReadValueFromVector<uint32_t>(d, pos, 4);

      len_1 = HYUZU_UE_ReadValueFromVector<uint32_t>(d, pos, 4);
      len_2 = HYUZU_UE_ReadValueFromVector<uint32_t>(d, pos, 4);

      offset = HYUZU_UE_ReadValueFromVector<uint64_t>(d, pos, 8);

      data = HYUZU_UE_SplitVector(d, pos, len_1);

      width = HYUZU_UE_ReadValueFromVector<uint32_t>(d, pos, 4);
      height = HYUZU_UE_ReadValueFromVector<uint32_t>(d, pos, 4);
    }
  };

  std::vector<uint8_t> header;
	std::vector<uint8_t> footer;
  std::vector<HYUZU_UE_Mip> mips;
  uint64_t bad_file_switch_1;
	uint64_t bad_file_switch_2;
	uint32_t header_size = 355;
	uint32_t footer_size = 0x10;
	uint8_t mip_count = 10;

  void serialize(std::vector<uint8_t> data) {
    bad_file_switch_1 = HYUZU_UE_ReadValueFromVector<uint32_t>(data, pos, 8);
    bad_file_switch_2 = HYUZU_UE_ReadValueFromVector<uint32_t>(data, pos, 8);

    if (bad_file_switch_2 == 5) {
        header_size = 329;
        mip_count = 9;
    }

    header = HYUZU_UE_SplitVector(data, pos, header_size);

    for (int i = 0; i < mip_count - 1; i++) {
        HYUZU_UE_Mip mip;
        mip.serialize(data, pos);
        mips.push_back(mip);
    }
    
    footer = HYUZU_UE_SplitVector(data, pos, footer_size);
    //footer = std::vector<uint8_t>(ftr_);
  }
};