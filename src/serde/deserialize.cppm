export module serde:deserialize;

import std;

export namespace serde::deserialize {
template <typename T>
concept Deserializer = requires(T &s, std::string_view string) {
  { s.read_field(string) } -> std::convertible_to<std::optional<T>>;
  { s.keys() } -> std::convertible_to<std::optional<std::vector<std::string>>>;
  { s.deserialize_array() } -> std::convertible_to<std::optional<T>>;
  { s.next_entry() } -> std::convertible_to<std::optional<T>>;
  {
    s.deserialize_string()
  } -> std::convertible_to<std::optional<std::string_view>>;
  {
    s.deserialize_signed()
  } -> std::convertible_to<std::optional<std::int64_t>>;
  {
    s.deserialize_unsigned()
  } -> std::convertible_to<std::optional<std::uint64_t>>;
  { s.deserialize_float() } -> std::convertible_to<std::optional<double>>;
};
} // namespace serde::deserialize
