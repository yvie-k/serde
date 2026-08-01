export module serde:convert.base;

import :serialize;
import :deserialize;

export namespace serde::convert {
template <serde::serialize::Serializer S, typename T> class serialize_type {};

template <serde::serialize::Serializer S, typename T>
void serialize(S &serializer, const T &value) {
  serialize_type<S, T>{}(serializer, value);
}

template <serde::serialize::Serializer S, typename T>
class serialize_type<S, const T *> {
public:
  void operator()(S &serializer, const T *&value) {
    serialize(serializer, *value);
  }
};

template <serde::deserialize::Deserializer D, typename T>
class deserialize_type {};

template <serde::deserialize::Deserializer D, typename T>
void deserialize(D &deserializer, T &value) {
  deserialize_type<D, T>{}(deserializer, value);
}
} // namespace serde::convert
