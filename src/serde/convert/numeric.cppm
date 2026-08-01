export module serde:convert.numeric;

import :serialize;
import :deserialize;

import :convert.base;

namespace serde::convert {
template <serde::serialize::Serializer S, std::integral T>
  requires(std::numeric_limits<T>::is_signed &&
           std::numeric_limits<T>::min() >=
               std::numeric_limits<std::int64_t>::min() &&
           std::numeric_limits<T>::max() <=
               std::numeric_limits<std::int64_t>::max())
class serialize_type<S, T> {
public:
  void operator()(S &serializer, const T &value) {
    serializer.serialize_signed(value);
  }
}; // namespace serde::template

template <serde::serialize::Serializer S, std::integral T>
  requires(std::numeric_limits<T>::is_unsigned &&
           std::numeric_limits<T>::min() >=
               std::numeric_limits<std::uint64_t>::min() &&
           std::numeric_limits<T>::max() <=
               std::numeric_limits<std::uint64_t>::max())
class serialize_type<S, T> {
public:
  void operator()(S &serializer, const T &value) {
    serializer.serialize_unsigned(value);
  }
};

template <serde::serialize::Serializer S> class serialize_type<S, float> {
public:
  void operator()(S &serializer, const float &value) {
    serializer.serialize_float(value);
  }
};

template <serde::serialize::Serializer S> class serialize_type<S, double> {
public:
  void operator()(S &serializer, const double &value) {
    serializer.serialize_float(value);
  }
};

template <serde::deserialize::Deserializer D, std::integral T>
  requires(std::numeric_limits<T>::is_signed)
class deserialize_type<D, T> {
public:
  void operator()(D &deserializer, T &value) {
    if (auto number = deserializer.deserialize_signed()) {
      value = *number;
    }
  }
};

template <serde::deserialize::Deserializer D, std::integral T>
  requires(std::numeric_limits<T>::is_unsigned)
class deserialize_type<D, T> {
public:
  void operator()(D &deserializer, T &value) {
    if (auto number = deserializer.deserialize_unsigned()) {
      value = *number;
    }
  }
};

template <serde::deserialize::Deserializer D> class deserialize_type<D, float> {
public:
  void operator()(D &deserializer, const float &value) {
    if (auto float_value = deserializer.deserialize_float()) {
      value = *float_value;
    }
  }
};

template <serde::deserialize::Deserializer D> class serialize_type<D, double> {
public:
  void operator()(D &deserializer, const double &value) {
    if (auto float_value = deserializer.deserialize_float()) {
      value = *float_value;
    }
  }
};

} // namespace serde::convert
