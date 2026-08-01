export module serde:convert.base;

import :serialize;
import :deserialize;

export namespace serde::convert {
template <serde::serialize::Serializer S, typename T> class serialize_type {
public:
  void operator()(S &serializer,
                  const T &value) = delete ("No serializer for type");
};

template <serde::serialize::Serializer S, typename T>
void serialize(S &serializer, const T &value) {
  serialize_type<S, T>{}(serializer, value);
}

template <serde::serialize::Serializer S, typename T>
class serialize_type<S, const T *> {
public:
  void operator()(S &serializer, const T *const &value) {
    serialize(serializer, *value);
  }
};

template <serde::deserialize::Deserializer D, typename T>
class deserialize_type {
public:
  void operator()(D &deserializer,
                  T &value) = delete ("No deserializer for type");
};

template <serde::deserialize::Deserializer D, typename T>
void deserialize(D &deserializer, T &value) {
  deserialize_type<D, T>{}(deserializer, value);
}
} // namespace serde::convert
