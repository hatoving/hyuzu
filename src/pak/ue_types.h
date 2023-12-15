#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

struct DataBuffer {
  std::vector<uint8_t> data;
  int pos;
  int offset;

  void init(std::vector<uint8_t> d, int p, int o) {
    data = d;
    pos = p;
    offset = o;
  }

  void serialize(void *buffer) {
    std::memcpy(&buffer, data.data(), pos + sizeof(buffer));
    pos += sizeof(buffer);
  }

  void serialize(void *buffer, size_t size) {
    std::memcpy(buffer, data.data() + pos, size);
    pos += size;
  }
};

struct HYUZU_UE_Mip {
  uint64_t entry_identifier;
	uint32_t flags;
	uint32_t len_1;
	uint32_t len_2;
	uint64_t offset;
	std::vector<uint8_t> data;
	uint32_t width;
	uint32_t height;

  void serialize(DataBuffer& buf) {
    buf.serialize(&entry_identifier);
    buf.serialize(&flags);
    buf.serialize(&len_1);
    buf.serialize(&len_2);
    buf.serialize(&offset);
    buf.serialize(&data, len_1);
    buf.serialize(&width);
    buf.serialize(&height);
  }
};

struct HYUZU_UE_Texture2D {
  std::vector<uint8_t> header;
	std::vector<uint8_t> footer;
	HYUZU_UE_Mip mip;
  uint64_t bad_file_switch_1;
	uint64_t bad_file_switch_2;
	uint32_t header_size = 355;
	uint32_t footer_size = 0x10;
	uint8_t mip_count = 10;

  void serialize(std::vector<uint8_t>& data) {
    std::ofstream fout;

    fout.open("cover.tex", std::ios::binary | std::ios::out);
    fout.write(reinterpret_cast<const char*>(data.data()), data.size());

    fout.close();
  }
};