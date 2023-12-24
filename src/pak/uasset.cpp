#include "uasset.h"

Hyuzu::UE::UAsset::AssetInfo Hyuzu::UE::UAsset::ExtractUAsset(std::vector<uint8_t> data, size_t& offset_ua) {
  uint8_t* signature = Hyuzu::UE::ReadBytesFromVector(data, offset_ua, 4); // C1 83 2A 9E
  uint32_t version = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4) ^ 0xFFFFFFFF;

  offset_ua += 16;

  uint32_t dir_offset = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t unk1 = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t package_name = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);

  offset_ua += 4;

  uint8_t unk2 = Hyuzu::UE::ReadValueFromVector<uint8_t>(data, offset_ua, 1);

  uint32_t names_amount = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t offs_amount = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);

  offset_ua += 8;

  uint32_t export_amount = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t export_off = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t import_amount = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t import_off = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);

  offset_ua += 20;

  uint8_t* guid_hash = Hyuzu::UE::ReadBytesFromVector(data, offset_ua, 16);

  uint32_t unk3 = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t unk4 = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t unk5 = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4); // names_amount again

  offset_ua += 36;
  uint32_t unk6 = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  offset_ua += 4;

  uint32_t padding_offset = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t file_length = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);

  offset_ua += 12;

  uint32_t unk7 = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);
  uint32_t data_offset = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);

  std::vector<Hyuzu::UE::UAsset::NameInfo> names;

  for (int i = 0; i < names_amount; i++) {
      Hyuzu::UE::UAsset::NameInfo info;

      info.size = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);

      info.string = std::string(((char*)Hyuzu::UE::ReadBytesFromVector(data, offset_ua, (int)info.size - 1)));
      info.string.erase(info.string.size() - (info.string.size() - (info.size - 1)), (info.string.size() - (info.size - 1)));

      offset_ua += 1;

      info.flags = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);
      names.push_back(info);
  }

  std::vector<Hyuzu::UE::UAsset::ImportInfo> imports;

  for (int i = 0; i < import_amount; i++) {
      Hyuzu::UE::UAsset::ImportInfo info;

      info.parent_name_id = Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ua, 8);
      info.class_id = Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ua, 8);
      info.parent_import_id = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ua, 4);
      info.name_id = Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ua, 4);
      info.unk_id = Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ua, 4);

      imports.push_back(info);
  }

  for (int i = 0; i < export_amount; i++) {
      offset_ua += 100;
  }

  offset_ua += 4;

  return {names, imports};
}

void Hyuzu::UE::UAsset::ExtractUExp(std::vector<uint8_t> data, Hyuzu::UE::UAsset::AssetInfo info, std::unordered_map<std::string, std::vector<std::string>>& metadata, size_t& offset_ue) {
    bool check = true;
    while (offset_ue <= data.size()) {
      std::string name_id = info.names[Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;

      if (name_id == "None") {
          if (offset_ue + 8 >= data.size()) {
            break;
          }
          else if (check) {
            name_id = info.names[Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;
            check = false;
          }
      }

      std::string class_id = info.names[Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;
      //printf("%s, %s\n", name_id.c_str(), class_id.c_str());
      uint64_t length = Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ue, 8);
      std::vector<std::string> vs;

      if (class_id == "NameProperty") {
          offset_ue += 1;

          std::string name = info.names[Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ue, 4)].string;
          uint32_t name_unknown = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ue, 4);

          vs.push_back(name);
          metadata[name_id] = vs;
          //printf("%s\n", name.c_str());
      } else if (class_id == "SoftObjectProperty") {
          offset_ue += 1;

          std::string name = info.names[Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ue, 4)].string;
          uint64_t value = Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ue, 8);

          vs.push_back(name);
          metadata[name_id] = vs;
          //printf("%s, %i\n", name.c_str(), value);
      } else if (class_id == "TextProperty") {
          offset_ue += 1;

          uint32_t flag = Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ue, 4);
          int8_t history_type = Hyuzu::UE::ReadValueFromVector<int8_t>(data, offset_ue, 1);

          std::vector<std::string> strings;

          if (history_type == -1) {
              int8_t strings_amount = Hyuzu::UE::ReadValueFromVector<int8_t>(data, offset_ue, 4);

              for (int i = 0; i < strings_amount; i++) {
                  int8_t string_length = Hyuzu::UE::ReadValueFromVector<int8_t>(data, offset_ue, 4);
                  
                  if (string_length < 0) {
                      string_length = string_length * -2;

                      std::string u16_string = std::string(((char*)Hyuzu::UE::ReadBytesFromVector(data, offset_ue, (int)string_length)));
                      strings.push_back(u16_string);
                  } else {
                      std::string string = std::string(((char*)Hyuzu::UE::ReadBytesFromVector(data, offset_ue, (int)string_length)));
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
              //printf("%s\n", texts.c_str());
          }
      } else if (class_id == "EnumProperty") {
          std::string enum_type = info.names[Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;
          offset_ue += 1;
          std::string enum_value = info.names[Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;

          vs.push_back(enum_value);
          metadata[name_id] = vs;
          //printf("%s\n", enum_value.c_str());
      } else if (class_id == "IntProperty") {
          offset_ue += 1;
          int value = Hyuzu::UE::ReadValueFromVector<int>(data, offset_ue, 4);

          vs.push_back(std::to_string(value));
          metadata[name_id] = vs;
          //printf("%i\n", value);
      } else if (class_id == "ArrayProperty") {
          std::string a_class = info.names[Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;

          offset_ue += 1;
          uint16_t amount_values = Hyuzu::UE::ReadValueFromVector<uint16_t>(data, offset_ue, 4);

          std::vector<std::any> values;
           
          if (a_class == "ObjectProperty") {
              for (int i = 0; i < amount_values; i++)
              {
                  Hyuzu::UE::UAsset::ImportInfo value = info.imports[Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ue, 4) ^ 0xFFFFFFFF];
                  values.push_back(value);
              }
          } else if (a_class == "FloatProperty") {
              for (int i = 0; i < amount_values; i++)
              {
                  float value = Hyuzu::UE::ReadValueFromVector<float>(data, offset_ue, 4);
                  values.push_back(value);
                  vs.push_back(std::to_string(value));
              }
          } else if (a_class == "SoftObjectProperty") {
              std::unordered_map<std::string, std::any> sobjectp_values;
              for (int i = 0; i < amount_values; i++)
              {
                  offset_ue += 1;

                  std::string name = info.names[Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ue, 4)].string;
                  uint64_t value = Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ue, 8);

                  sobjectp_values[name] = value;
                  vs.push_back(std::to_string(value));
                  values.push_back(sobjectp_values);
              }
              offset_ue -= 1;
          }
          
          metadata[name_id] = vs;
          //metadata[name_id] = values;
          //printf("%s, amount: %i\n", a_class.c_str(), amount_values);
      } else if (class_id == "ObjectProperty") {
          offset_ue += 1;
          Hyuzu::UE::UAsset::ImportInfo value = info.imports[Hyuzu::UE::ReadValueFromVector<uint32_t>(data, offset_ue, 4) ^ 0xFFFFFFFF];
          //metadata[name_id] = value;
          //printf("%s, %s\n", info.names[value.name_id].string.c_str(), info.names[value.class_id].string.c_str());
      } else if (class_id == "StructProperty") {
          size_t cur_offset = offset_ue;

          std::string struct_class = info.names[Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;
          std::vector<std::any> struct_values;

          if (struct_class == "Transposes") {
              offset_ue += 17;

              while (offset_ue <= cur_offset + length) {
                  std::string key = info.names[Hyuzu::UE::ReadValueFromVector<uint64_t>(data, offset_ue, 8)].string;
                  offset_ue += 8;
                  int value = Hyuzu::UE::ReadValueFromVector<int>(data, offset_ue, 4);
                  offset_ue += 9;
                  struct_values.push_back(value);
                  vs.push_back(std::to_string(value));
              }
          }

          metadata[name_id] = vs;
          //metadata[name_id] = struct_values;
          
          std::string values_text = "{ ";
          for (int i = 0; i < struct_values.size(); i++) {
              if (i != struct_values.size() - 1) values_text += std::to_string(std::any_cast<int>(struct_values[i])) + ", ";
              else values_text += std::to_string(std::any_cast<int>(struct_values[i]));
          }
          values_text += " }";
          //printf("%s\n", values_text.c_str());
      } else break;
    }
}

std::vector<float> Hyuzu::UE::UAsset::ConvertStringsToFloats(std::vector<std::string> strings) {
    std::vector<float> result;

    for (const auto& str : strings) {
        try {
            float value = std::stof(str);
            result.push_back(value);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error converting string to float: " << e.what() << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Error converting string to float: " << e.what() << std::endl;
        }
    }

    return result;
}