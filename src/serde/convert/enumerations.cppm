export module serde:convert.enumerations;

import std;

import :serialize;
import :deserialize;

import :convert.base;

namespace serde::convert {

template <serde::serialize::Serializer S, typename T>
  requires(std::is_enum_v<T>)
class serialize_type<S, T> {
public:
  void operator()(S &serializer, const T &value) {
    static constexpr auto enumerators =
        std::define_static_array(std::meta::enumerators_of(^^T));
    template for (constexpr auto e : enumerators) {
      if (value == [:e:]) {
        serializer.serialize_string(std::meta::identifier_of(e));
        return;
      }
    }
  }
};

template <serde::deserialize::Deserializer D, typename T>
  requires(std::is_enum_v<T>)
class deserialize_type<D, T> {
public:
  void operator()(D &deserializer, T &value) {
    std::optional<std::string_view> string_value =
        deserializer.deserialize_string();
    if (!string_value) {
      return;
    }

    static constexpr auto enumerators =
        std::define_static_array(std::meta::enumerators_of(^^T));
    template for (constexpr auto e : enumerators) {
      if (string_value == std::meta::identifier_of(e)) {
        value = [:e:];
        return;
      }
    }
  }
};

} // namespace serde::convert
