export module serde.test.json;

import std;

import nlohmann.json;

import serde;

export namespace serde::test {

class JsonSerializer {
public:
  JsonSerializer(nlohmann::json &json) : value(json) {}

  JsonSerializer serialize_map() {
    value = nlohmann::json::object();
    return JsonSerializer{value};
  }

  JsonSerializer add_field(std::string_view field) {
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

  void serialize_bool(bool boolean) { value = boolean; }

private:
  nlohmann::json &value;
};

class JsonDeserializer {
public:
  JsonDeserializer(const nlohmann::json &json) : value(json) {}

  std::optional<std::vector<std::string>> keys() {
    if (value.is_object()) {
      std::vector<std::string> result;
      for (const auto &entry : value.items()) {
        result.push_back(entry.key());
      }
      return result;
    }

    return std::nullopt;
  }

  std::optional<JsonDeserializer> read_field(std::string_view field) {
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

  std::optional<std::string_view> deserialize_string() {
    if (value.is_string()) {
      return value.get<std::string_view>();
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
    } else if (value.is_number_integer() && value.get<std::int64_t>() >= 0) {
      return value.get<std::int64_t>();
    }

    return std::nullopt;
  }

  std::optional<double> deserialize_float() {
    if (value.is_number()) {
      return value;
    }

    return std::nullopt;
  }

  std::optional<bool> deserialize_bool() {
    if (value.is_boolean()) {
      return value;
    }

    return std::nullopt;
  }

private:
  const nlohmann::json &value;

  std::size_t array_index = 0;
};

static_assert(serde::serialize::Serializer<JsonSerializer>);
static_assert(serde::deserialize::Deserializer<JsonDeserializer>);

/* Convenience helpers for the test cases. */

template <typename T> nlohmann::json to_json(const T &value) {
  nlohmann::json output;
  JsonSerializer serializer(output);
  serde::convert::serialize(serializer, value);
  return output;
}

template <typename T> void from_json(const nlohmann::json &json, T &value) {
  JsonDeserializer deserializer(json);
  serde::convert::deserialize(deserializer, value);
}

template <typename T> T from_json(const nlohmann::json &json) {
  T value{};
  from_json(json, value);
  return value;
}

/* Serialize a value and read it straight back into a fresh instance. */
template <typename T> T round_trip(const T &value) {
  return from_json<T>(to_json(value));
}

} // namespace serde::test
