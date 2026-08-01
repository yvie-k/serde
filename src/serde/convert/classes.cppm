export module serde:convert.classes;

import :serialize;
import :deserialize;
import :convert.base;
import :annotations;

export namespace serde::convert {

template <serde::serialize::Serializer S, typename T,
          std::enable_if_t<std::is_class_v<T>, bool> = true>
class serialize_class {
public:
  void operator()(S &serializer, const T &value) {
    static constexpr auto context = std::meta::access_context::unchecked();
    static constexpr auto members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, context));
    template for (constexpr auto property : members) {
      static constexpr auto flags =
          serde::annotations::compute_serialize_flags(property);

      if constexpr (!flags.skip) {
        auto map_serializer = serializer.serialize_map(flags.name);
        serialize(map_serializer, value.[:property:]);
      }
    }
  }
};

template <serde::serialize::Serializer S, typename T>
  requires(std::is_class_v<T>)
class serialize_type<S, T> {
public:
  void operator()(S &serializer, const T &value) {
    serialize_class<S, T>{}(serializer, value);
  }
};

/* Specializations for special classes */

template <serde::serialize::Serializer S, typename T>
class serialize_class<S, const std::unique_ptr<T>> {
public:
  void operator()(S &serializer, const std::unique_ptr<T> &value) {
    serialize(serializer, *value);
  }
};

template <serde::serialize::Serializer S, typename T>
class serialize_class<S, const std::shared_ptr<T>> {
public:
  void operator()(S &serializer, const std::shared_ptr<T> &value) {
    serialize(serializer, *value);
  }
};

template <serde::serialize::Serializer S,
          std::convertible_to<std::string_view> T>
class serialize_class<S, T> {
public:
  void operator()(S &serializer, const T &value) {
    serializer.serialize_string(std::string_view(value));
  }
};

template <serde::serialize::Serializer S, typename T>
class serialize_class<S, std::vector<T>> {
public:
  void operator()(S &serializer, const std::vector<T> &array) {
    auto array_serializer = serializer.serialize_array();
    for (const auto &value : array) {
      auto array_entry_serializer = array_serializer.add_entry();
      serialize(array_entry_serializer, value);
    }
  }
};

template <serde::serialize::Serializer S,
          std::convertible_to<std ::string_view> K, typename V>
class serialize_class<S, std::map<K, V>> {
public:
  void operator()(S &serializer, const std::map<K, V> &map) {
    for (const auto &[key, value] : map) {
      auto map_serializer = serializer.serialize_map(std::string_view(key));
      serialize(map_serializer, value);
    }
  }
};

template <serde::serialize::Serializer S,
          std::convertible_to<std ::string_view> K, typename V>
class serialize_class<S, std::unordered_map<K, V>> {
public:
  void operator()(S &serializer, const std::unordered_map<K, V> &map) {
    for (const auto &[key, value] : map) {
      auto map_serializer = serializer.serialize_map(std::string_view(key));
      serialize(map_serializer, value);
    }
  }
};

template <serde::deserialize::Deserializer D, typename T,
          std::enable_if_t<std::is_class_v<T>, bool> = true>
class deserialize_class {
public:
  void operator()(D &deserializer, T &value) {
    static constexpr auto context = std::meta::access_context::unchecked();
    static constexpr auto members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, context));
    template for (constexpr auto property : members) {
      static constexpr auto flags =
          serde::annotations::compute_deserialize_flags(property);

      if constexpr (!flags.skip) {
        auto map_deserializer = deserializer.deserialize_map(flags.name);
        if (map_deserializer) {
          deserialize(*map_deserializer, value.[:property:]);
        }
      }
    }
  }
};

template <serde::deserialize::Deserializer D, typename T>
  requires(std::is_class_v<T>)
class deserialize_type<D, T> {
public:
  void operator()(D &deserializer, T &value) {
    deserialize_class<D, T>{}(deserializer, value);
  }
};

template <serde::deserialize::Deserializer D, typename T>
class deserialize_class<D, std::unique_ptr<T>> {
public:
  void operator()(D &deserializer, std::unique_ptr<T> &value) {
    value = std::make_unique(T{});
    deserialize(deserializer, *value);
  }
};
template <serde::deserialize::Deserializer D, typename T>
class deserialize_type<D, std::shared_ptr<T>> {
public:
  void operator()(D &deserializer, std::shared_ptr<T> &value) {
    value = std::make_shared(T{});
    deserialize(deserializer, *value);
  }
};

template <serde::deserialize::Deserializer D>
class deserialize_class<D, std::string> {
public:
  void operator()(D &deserializer, std::string &value) {
    if (auto deserialized = deserializer.deserialize_string()) {
      value = *deserialized;
    }
  }
};

template <serde::deserialize::Deserializer D, typename T>
class deserialize_class<D, std::vector<T>> {
public:
  void operator()(D &deserializer, std::vector<T> &array) {
    auto array_deserializer = deserializer.deserialize_array();
    if (!array_deserializer) {
      return;
    }

    while (auto array_entry_deserializer = array_deserializer->next_entry()) {
      deserialize(*array_entry_deserializer, array.emplace_back());
    }
  }
};

template <serde::deserialize::Deserializer D, typename V>
class deserialize_class<D, std::map<std::string, V>> {
public:
  void operator()(D &deserializer,
                  std::map<std::string, V> &value) {
    if (auto map_keys = deserializer.keys()) {
      for (const auto &key : *map_keys) {
        if (auto map_deserializer =
                deserializer.deserialize_map(std::string_view(key))) {
          deserialize(*map_deserializer, value[key]);
        }
      }
    }
  }
};

template <serde::deserialize::Deserializer D, typename V>
class deserialize_class<D, std::unordered_map<std::string, V>> {
public:
  void operator()(D &deserializer,
                  std::unordered_map<std::string, V> &value) {
    if (auto map_keys = deserializer.keys()) {
      for (const auto &key : *map_keys) {
        if (auto map_deserializer =
                deserializer.deserialize_map(std::string_view(key))) {
          deserialize(*map_deserializer, value[key]);
        }
      }
    }
  }
};

} // namespace serde::convert
