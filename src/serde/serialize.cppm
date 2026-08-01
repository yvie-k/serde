export module serde:serialize;

import std;

export namespace serde::serialize {

template <typename T>
concept Serializer =
    requires(T &s, std::string_view string, std::int64_t signed_integer,
             std::uint64_t unsigned_integer, double floating_point) {
      { s.serialize_map() } -> std::convertible_to<T>;
      { s.add_field(string) } -> std::convertible_to<T>;
      { s.serialize_array() } -> std::convertible_to<T>;
      { s.add_entry() } -> std::convertible_to<T>;
      { s.serialize_string(string) } -> std::convertible_to<void>;
      { s.serialize_signed(signed_integer) } -> std::convertible_to<void>;
      { s.serialize_unsigned(unsigned_integer) } -> std::convertible_to<void>;
      { s.serialize_float(floating_point) } -> std::convertible_to<void>;
    };

} // namespace serde::serialize
