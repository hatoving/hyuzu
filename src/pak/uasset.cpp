#include "uasset.h"

UEAssetInfo HYUZU_UE_ExtractUAsset(std::vector<uint8_t> data, size_t& offset_ua) {
  uint8_t* signature = HYUZU_UE_ReadBytesFromVector(data, offset_ua, 4); // C1 83 2A 9E
  uint32_t version = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4) ^ 0xFFFFFFFF;

  offset_ua += 16;

  uint32_t dir_offset = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t unk1 = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t package_name = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);

  offset_ua += 4;

  uint8_t unk2 = HYUZU_UE_ReadValueFromVector<uint8_t>(data, offset_ua, 1);

  uint32_t names_amount = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t offs_amount = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);

  offset_ua += 8;

  uint32_t export_amount = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t export_off = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t import_amount = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t import_off = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);

  offset_ua += 20;

  uint8_t* guid_hash = HYUZU_UE_ReadBytesFromVector(data, offset_ua, 16);

  uint32_t unk3 = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t unk4 = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t unk5 = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4); // names_amount again

  offset_ua += 36;
  uint32_t unk6 = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  offset_ua += 4;

  uint32_t padding_offset = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t file_length = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);

  offset_ua += 12;

  uint32_t unk7 = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t data_offset = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);

  std::vector<NameInfo> names;

  for (int i = 0; i < names_amount; i++) {
      NameInfo info;

      info.size = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);

      info.string = std::string(((char*)HYUZU_UE_ReadBytesFromVector(data, offset_ua, (int)info.size - 1)));
      info.string.erase(info.string.size() - (info.string.size() - (info.size - 1)), (info.string.size() - (info.size - 1)));

      offset_ua += 1;

      info.flags = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);
      names.push_back(info);
  }

  std::vector<ImportInfo> imports;

  for (int i = 0; i < import_amount; i++) {
      ImportInfo info;

      info.parent_name_id = HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ua, 8);
      info.class_id = HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ua, 8);
      info.parent_import_id = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ua, 4);
      info.name_id = HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ua, 4);
      info.unk_id = HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ua, 4);

      imports.push_back(info);
  }

  for (int i = 0; i < export_amount; i++) {
      offset_ua += 100;
  }

  offset_ua += 4;

  return {names, imports};
}

void HYUZU_UE_ExtractUExp(std::vector<uint8_t> data, UEAssetInfo info, std::unordered_map<std::string, std::any>& metadata, size_t& offset_ue) {
    bool check = true;
    while (offset_ue <= data.size()) {
      std::string name_id = info.names[HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;

      if (name_id == "None") {
          if (offset_ue + 8 >= data.size()) {
            break;
          }
          else if (check) {
            name_id = info.names[HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;
            check = false;
          }
      }

      std::string class_id = info.names[HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;
      printf("\n%s, %s\n", name_id.c_str(), class_id.c_str());
      uint64_t length = HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ue, 8);

      if (class_id == "NameProperty") {
          offset_ue += 1;

          std::string name = info.names[HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ue, 4)].string;
          uint32_t name_unknown = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ue, 4);

          metadata[name_id] = name;
          printf("%s\n", name.c_str());
      } else if (class_id == "SoftObjectProperty") {
          offset_ue += 1;

          std::string name = info.names[HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ue, 4)].string;
          uint64_t value = HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ue, 8);

          metadata[name_id] = value;
          printf("%s, %i\n", name.c_str(), value);
      } else if (class_id == "TextProperty") {
          offset_ue += 1;

          uint32_t flag = HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ue, 4);
          int8_t history_type = HYUZU_UE_ReadValueFromVector<int8_t>(data, offset_ue, 1);

          std::vector<std::string> strings;

          if (history_type == -1) {
              int8_t strings_amount = HYUZU_UE_ReadValueFromVector<int8_t>(data, offset_ue, 4);

              for (int i = 0; i < strings_amount; i++) {
                  int8_t string_length = HYUZU_UE_ReadValueFromVector<int8_t>(data, offset_ue, 4);
                  
                  if (string_length < 0) {
                      string_length = string_length * -2;

                      std::string u16_string = std::string(((char*)HYUZU_UE_ReadBytesFromVector(data, offset_ue, (int)string_length)));
                      strings.push_back(u16_string);
                  } else {
                      std::string string = std::string(((char*)HYUZU_UE_ReadBytesFromVector(data, offset_ue, (int)string_length)));
                      strings.push_back(string);
                  }
              }

              std::string texts = "{ ";
              for (int i = 0; i < strings.size(); i++) {
                  if (i != strings.size() - 1) texts += strings[i] + ",";
                  else texts += strings[i];
              }
              texts += " }";
              
              metadata[name_id] = strings;
              printf("%s\n", texts.c_str());
          }
      } else if (class_id == "EnumProperty") {
          std::string enum_type = info.names[HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;
          offset_ue += 1;
          std::string enum_value = info.names[HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;

          metadata[name_id] = enum_value;
          printf("%s\n", enum_value.c_str());
      } else if (class_id == "IntProperty") {
          offset_ue += 1;
          int value = HYUZU_UE_ReadValueFromVector<int>(data, offset_ue, 4);

          metadata[name_id] = value;
          printf("%i\n", value);
      } else if (class_id == "ArrayProperty") {
          std::string a_class = info.names[HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;

          offset_ue += 1;
          uint16_t amount_values = HYUZU_UE_ReadValueFromVector<uint16_t>(data, offset_ue, 4);

          std::vector<std::any> values;
           
          if (a_class == "ObjectProperty") {
              for (int i = 0; i < amount_values; i++)
              {
                  ImportInfo value = info.imports[HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ue, 4) ^ 0xFFFFFFFF];
                  values.push_back(value);
              }
          } else if (a_class == "FloatProperty") {
              for (int i = 0; i < amount_values; i++)
              {
                  float value = HYUZU_UE_ReadValueFromVector<float>(data, offset_ue, 4);
                  values.push_back(value);
              }
          } else if (a_class == "SoftObjectProperty") {
              std::unordered_map<std::string, std::any> sobjectp_values;
              for (int i = 0; i < amount_values; i++)
              {
                  offset_ue += 1;

                  std::string name = info.names[HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ue, 4)].string;
                  uint64_t value = HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ue, 8);

                  sobjectp_values[name] = value;
                  values.push_back(sobjectp_values);
              }
              offset_ue -= 1;
          }
          
          metadata[name_id] = values;
          printf("%s, amount: %i\n", a_class.c_str(), amount_values);
      } else if (class_id == "ObjectProperty") {
          offset_ue += 1;
          ImportInfo value = info.imports[HYUZU_UE_ReadValueFromVector<uint32_t>(data, offset_ue, 4) ^ 0xFFFFFFFF];
          metadata[name_id] = value;
          printf("%s, %s\n", info.names[value.name_id].string.c_str(), info.names[value.class_id].string.c_str());
      } else if (class_id == "StructProperty") {
          size_t cur_offset = offset_ue;

          std::string struct_class = info.names[HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;
          std::vector<std::any> struct_values;

          if (struct_class == "Transposes") {
              offset_ue += 17;

              while (offset_ue <= cur_offset + length) {
                  std::string key = info.names[HYUZU_UE_ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;
                  offset_ue += 8;
                  int value = HYUZU_UE_ReadValueFromVector<int>(data, offset_ue, 4);
                  offset_ue += 9;
                  struct_values.push_back(value);
              }
          }

          metadata[name_id] = struct_values;
          
          std::string values_text = "{ ";
          for (int i = 0; i < struct_values.size(); i++) {
              if (i != struct_values.size() - 1) values_text += std::to_string(std::any_cast<int>(struct_values[i])) + ", ";
              else values_text += std::to_string(std::any_cast<int>(struct_values[i]));
          }
          values_text += " }";
          printf("%s\n", values_text.c_str());
      } else break;
    }
}