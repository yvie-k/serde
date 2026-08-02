export module serde:convert.classes;

import std;

import :serialize;
import :deserialize;
import :convert.base;
import :annotations;

namespace serde::convert {

template <typename T, std::enable_if_t<std::is_class_v<T>, bool> = true>
class structure {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const T &value) {
    auto map_serializer = serializer.serialize_map();

    static constexpr auto context = std::meta::access_context::unchecked();
    static constexpr auto members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, context));
    template for (constexpr auto property : members) {
      static constexpr auto flags =
          serde::annotations::compute_serialize_flags(property);

      if constexpr (!flags.skip) {
        auto field_serializer = map_serializer.add_field(flags.name);
        serde::convert::serialize(field_serializer, value.[:property:]);
      }
    }
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, T &value) {
    static constexpr auto context = std::meta::access_context::unchecked();
    static constexpr auto members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, context));
    template for (constexpr auto property : members) {
      static constexpr auto flags =
          serde::annotations::compute_deserialize_flags(property);

      if constexpr (!flags.skip) {
        auto map_deserializer = deserializer.read_field(flags.name);
        if (map_deserializer) {
          serde::convert::deserialize(*map_deserializer, value.[:property:]);
        }
      }
    }
  }
};

template <typename T>
  requires(std::is_class_v<T>)
class type<T> {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const T &value) {
    structure<T>::template serialize<S>(serializer, value);
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, T &value) {
    structure<T>::template deserialize<D>(deserializer, value);
  }
};

/* Specializations for special classes */

template <typename T> class structure<std::unique_ptr<T>> {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const std::unique_ptr<T> &value) {
    serde::convert::serialize(serializer, *value);
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, std::unique_ptr<T> &value) {
    value = std::make_unique<T>();
    serde::convert::deserialize(deserializer, *value);
  }
};

template <typename T> class structure<std::shared_ptr<T>> {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const std::shared_ptr<T> &value) {
    serde::convert::serialize(serializer, *value);
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, std::shared_ptr<T> &value) {
    value = std::make_shared<T>();
    serde::convert::deserialize(deserializer, *value);
  }
};

template <std::convertible_to<std::string_view> T> class structure<T> {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const T &value) {
    serializer.serialize_string(std::string_view(value));
  }

  template <serde::deserialize::Deserializer D>
    requires std::same_as<T, std::string>
  static void deserialize(D &deserializer, T &value) {
    if (auto deserialized = deserializer.deserialize_string()) {
      value = *deserialized;
    }
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, T &value) =
      delete ("Cannot deserialize std::string_view");
};

template <typename T> class structure<std::vector<T>> {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const std::vector<T> &array) {
    auto array_serializer = serializer.serialize_array();
    for (const auto &value : array) {
      auto array_entry_serializer = array_serializer.add_entry();
      serde::convert::serialize(array_entry_serializer, value);
    }
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, std::vector<T> &array) {
    auto array_deserializer = deserializer.deserialize_array();
    if (!array_deserializer) {
      return;
    }

    while (auto array_entry_deserializer = array_deserializer->next_entry()) {
      serde::convert::deserialize(*array_entry_deserializer,
                                  array.emplace_back());
    }
  }
};

template <std::convertible_to<std::string_view> K, typename V>
class structure<std::map<K, V>> {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const std::map<K, V> &map) {
    auto map_serializer = serializer.serialize_map();

    for (const auto &[key, value] : map) {
      auto field_serializer = map_serializer.add_field(std::string_view(key));
      serde::convert::serialize(field_serializer, value);
    }
  }

  template <serde::deserialize::Deserializer D>
    requires(std::same_as<K, std::string>)
  static void deserialize(D &deserializer, std::map<std::string, V> &value) {
    if (auto map_keys = deserializer.keys()) {
      for (const auto &key : *map_keys) {
        if (auto map_deserializer =
                deserializer.read_field(std::string_view(key))) {
          serde::convert::deserialize(*map_deserializer, value[key]);
        }
      }
    }
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, std::map<K, V> &value) =
      delete ("Cannot deserialize std::string_view");
};

template <std::convertible_to<std ::string_view> K, typename V>
class structure<std::unordered_map<K, V>> {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const std::unordered_map<K, V> &map) {
    auto map_serializer = serializer.serialize_map();

    for (const auto &[key, value] : map) {
      auto field_serializer = map_serializer.add_field(std::string_view(key));
      serde::convert::serialize(field_serializer, value);
    }
  }

  template <serde::deserialize::Deserializer D>
    requires(std::same_as<K, std::string>)
  static void deserialize(D &deserializer,
                          std::unordered_map<std::string, V> &value) {
    if (auto map_keys = deserializer.keys()) {
      for (const auto &key : *map_keys) {
        if (auto map_deserializer =
                deserializer.read_field(std::string_view(key))) {
          serde::convert::deserialize(*map_deserializer, value[key]);
        }
      }
    }
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, std::unordered_map<K, V> &value) =
      delete ("Cannot deserialize std::string_view");
};

} // namespace serde::convert
