export module serde:annotations;

import std;

namespace serde::annotations {

export struct Annotation {
  enum Type { Skip, SkipSerialize, SkipDeserialize, Rename };

  Type type;
  const char *string_value = nullptr;
};

export constexpr Annotation skip() { return {.type = Annotation::Skip}; }

export constexpr Annotation skip_serialize() {
	return {.type = Annotation::SkipSerialize};
}

export constexpr Annotation skip_deserialize() {
  return {.type = Annotation::SkipDeserialize};
}

export consteval Annotation rename(std::string_view name) {
  return {.type = Annotation::Rename, .string_value = std::define_static_string(name)};
}

struct Flags {
  bool skip = false;
  std::string_view name;
};

consteval Flags compute_common_flags(const std::meta::info property) {
  auto annotations =
      std::meta::annotations_of_with_type(property, ^^Annotation);

  Flags flags;
  flags.name = std::define_static_string(std::meta::identifier_of(property));

  for (auto entry : annotations) {
    Annotation annotation = std::meta::extract<Annotation>(entry);
    if (annotation.type == Annotation::Type::Skip) {
      flags.skip = true;
    }
    if (annotation.type == Annotation::Type::Rename) {
      flags.name = annotation.string_value;
    }
  }

  return flags;
}

consteval Flags compute_serialize_flags(const std::meta::info property) {
  auto annotations =
      std::meta::annotations_of_with_type(property, ^^Annotation);

  Flags flags = compute_common_flags(property);

  for (auto entry : annotations) {
    Annotation annotation = std::meta::extract<Annotation>(entry);
    if (annotation.type == Annotation::Type::SkipSerialize) {
      flags.skip = true;
    }
  }

  return flags;
}

consteval Flags compute_deserialize_flags(const std::meta::info property) {
  auto annotations =
      std::meta::annotations_of_with_type(property, ^^Annotation);

  Flags flags = compute_common_flags(property);

  for (auto entry : annotations) {
    Annotation annotation = std::meta::extract<Annotation>(entry);
    if (annotation.type == Annotation::Type::SkipDeserialize) {
      flags.skip = true;
    }
  }

  return flags;
}

} // namespace serde::annotations
