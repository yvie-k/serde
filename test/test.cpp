import std;

import nlohmann.json;

import serde;

enum Color {
  Red,
  Green,
  Blue,
  White,
};

struct Y {
  int value = 0;

  [[= serde::annotations::rename("colour")]] Color color = Color::White;

  [[= serde::annotations::skip()]] union {
  } _skip2;
};

struct X {
  std::string s;
  std::vector<Y> y;
  std::map<std::string, std::string> kv;
};

class JsonSerializer {
public:
  JsonSerializer(nlohmann::json &json) : value(json) {}

  JsonSerializer serialize_map(std::string_view field) {
    return JsonSerializer{value[field]};
  }

  JsonSerializer serialize_array() {
    value = nlohmann::json::array();
    return JsonSerializer{value};
  }

  JsonSerializer add_entry() { return JsonSerializer{value.emplace_back()}; }

  void serialize_string(std::string_view string) { value = string; }

  void serialize_signed(std::int64_t number) { value = number; }

  void serialize_unsigned(std::uint64_t number) { value = number; }

  void serialize_float(double number) { value = number; }

private:
  nlohmann::json &value;
};

class JsonDeserializer {
public:
  JsonDeserializer(const nlohmann::json &json) : value(json) {}

  std::optional<std::vector<std::string>> keys() {
    if (value.is_object()) {
      std::vector<std ::string> result;
      for (const auto &entry : value.items()) {
        result.push_back(entry.key());
      }
      return result;
    }

    return std::nullopt;
  }

  std::optional<JsonDeserializer> deserialize_map(std::string_view field) {
    if (value.is_object() && value.contains(field)) {
      return JsonDeserializer{value[field]};
    }

    return std::nullopt;
  }

  std::optional<JsonDeserializer> deserialize_array() {
    if (value.is_array()) {
      return JsonDeserializer{value};
    }

    return std::nullopt;
  }

  std::optional<JsonDeserializer> next_entry() {
    if (array_index < value.size()) {
      return JsonDeserializer{value[array_index++]};
    }

    return std::nullopt;
  }

  std::optional<std::string> deserialize_string() {
    if (value.is_string()) {
      return value;
    }

    return std::nullopt;
  }

  std::optional<std::int64_t> deserialize_signed() {
    if (value.is_number_integer()) {
      return value;
    }

    return std::nullopt;
  }

  std::optional<std::uint64_t> deserialize_unsigned() {
    if (value.is_number_unsigned()) {
      return value;
    }

    return std::nullopt;
  }

  std::optional<double> deserialize_float() {
    if (value.is_number()) {
      return value;
    }

    return std::nullopt;
  }

private:
  const nlohmann::json &value;

  std::size_t array_index = 0;
};

int main() {
  nlohmann::json output;
  JsonSerializer s(output);

  X input;
  input.s = "foo";
  input.y = {{.value = 42, .color = Color::Red},
             {.value = 43, .color = Color::Blue}};
  input.kv["foo"] = "bar";
  input.kv["baz"] = "qux";
  serde::convert::serialize(s, input);

  std::cout << output.dump(true) << std::endl;

  JsonDeserializer d(output);
  X converted;
  serde::convert::deserialize(d, converted);

  std::cout << "s = " << converted.s << ", y = {";
  bool first = true;
  for (const auto &entry : converted.y) {
    std::cout << (first ? "" : ", ") << entry.value << " / " << entry.color;
    first = false;
  }
  std::cout << "}, kv = {";
  first = true;
  for (const auto &[k, v] : converted.kv) {
    std::cout << (first ? "" : ", ") << k << " : " << v;
    first = false;
  }
  std::cout << "}" << std::endl;
}
