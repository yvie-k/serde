export module serde:serialize;

import std;

export namespace serde::serialize {

template <typename T>
concept Serializer =
    requires(T &s, std::string_view string, std::int64_t signed_integer,
             std::uint64_t unsigned_integer, double floating_point,
             bool boolean) {
      { s.serialize_map() } -> std::convertible_to<T>;
      { s.add_field(string) } -> std::convertible_to<T>;
      { s.serialize_array() } -> std::convertible_to<T>;
      { s.add_entry() } -> std::convertible_to<T>;
      { s.serialize_string(string) } -> std::same_as<void>;
      { s.serialize_signed(signed_integer) } -> std::same_as<void>;
      { s.serialize_unsigned(unsigned_integer) } -> std::same_as<void>;
      { s.serialize_float(floating_point) } -> std::same_as<void>;
      { s.serialize_bool(boolean) } -> std::same_as<void>;
    };

} // namespace serde::serialize
