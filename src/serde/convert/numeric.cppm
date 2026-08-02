export module serde:convert.numeric;

import std;

import :serialize;
import :deserialize;

import :convert.base;

namespace serde::convert {
template <std::integral T>
  requires(std::numeric_limits<T>::is_signed && !std::is_same_v<T, bool> &&
           std::numeric_limits<T>::min() >=
               std::numeric_limits<std::int64_t>::min() &&
           std::numeric_limits<T>::max() <=
               std::numeric_limits<std::int64_t>::max())
class type<T> {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const T &value) {
    serializer.serialize_signed(value);
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, T &value) {
    if (auto number = deserializer.deserialize_signed()) {
      value = *number;
    }
  }
};

template <std::integral T>
  requires(!std::numeric_limits<T>::is_signed && !std::is_same_v<T, bool> &&
           std::numeric_limits<T>::min() >=
               std::numeric_limits<std::uint64_t>::min() &&
           std::numeric_limits<T>::max() <=
               std::numeric_limits<std::uint64_t>::max())
class type<T> {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const T &value) {
    serializer.serialize_unsigned(value);
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, T &value) {
    if (auto number = deserializer.deserialize_unsigned()) {
      value = *number;
    }
  }
};

template <> class type<bool> {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const bool &value) {
    serializer.serialize_bool(value);
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, bool &value) {
    if (auto bool_value = deserializer.deserialize_bool()) {
      value = *bool_value;
    }
  }
};

template <> class type<float> {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const float &value) {
    serializer.serialize_float(value);
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, float &value) {
    if (auto float_value = deserializer.deserialize_float()) {
      value = *float_value;
    }
  }
};

template <> class type<double> {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const double &value) {
    serializer.serialize_float(value);
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, double &value) {
    if (auto float_value = deserializer.deserialize_float()) {
      value = *float_value;
    }
  }
};

} // namespace serde::convert
