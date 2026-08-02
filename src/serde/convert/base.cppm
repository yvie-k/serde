export module serde:convert.base;

import std;

import :serialize;
import :deserialize;

export namespace serde::convert {
template <typename T> class type {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer,
                        const T &value) = delete ("No serializer for type");

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer,
                          T &value) = delete ("No deserializer for type");
};

template <serde::serialize::Serializer S, typename T>
void serialize(S &serializer, const T &value) {
  type<std::remove_cvref_t<T>>::template serialize<S>(serializer, value);
}

template <serde::deserialize::Deserializer D, typename T>
void deserialize(D &deserializer, T &value) {
  type<std::remove_cvref_t<T>>::template deserialize<D>(deserializer, value);
}

template <typename T> class type<const T *> {
public:
  template <serde::serialize::Serializer S>
  static void serialize(S &serializer, const T *const &value) {
    serde::convert::serialize(serializer, *value);
  }

  template <serde::deserialize::Deserializer D>
  static void deserialize(D &deserializer, const T *&value) =
      delete ("Cannot deserialize pointer type");
};
} // namespace serde::convert
