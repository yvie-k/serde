#include "doctest.h"

import std;

import nlohmann.json;

import serde;
import serde.test.json;

using nlohmann::json;
using serde::test::from_json;
using serde::test::to_json;

namespace {

struct Renamed {
  [[= serde::annotations::rename("colour")]] std::string color = "red";
  [[= serde::annotations::rename("n")]] int number = 1;
};

struct Skipped {
  int kept = 1;

  [[= serde::annotations::skip()]] int skipped = 2;

  /* A member without a converter only compiles because it is skipped. */
  [[= serde::annotations::skip()]] int *raw = nullptr;
};

struct SkippedOnSerialize {
  int kept = 1;
  [[= serde::annotations::skip_serialize()]] int write_only = 2;
};

struct SkippedOnDeserialize {
  int kept = 1;
  [[= serde::annotations::skip_deserialize()]] int read_only = 2;
};

struct RenamedAndSkipped {
  [[= serde::annotations::rename("visible")]] int kept = 1;

  [[ = serde::annotations::rename("hidden"),
     = serde::annotations::skip() ]] int skipped = 2;
};

} // namespace

TEST_CASE("annotation: rename") {
  CHECK(to_json(Renamed{}) == json{{"colour", "red"}, {"n", 1}});

  Renamed value;
  from_json(json{{"colour", "blue"}, {"n", 7}}, value);

  CHECK(value.color == "blue");
  CHECK(value.number == 7);
}

TEST_CASE("annotation: rename ignores original name") {
  Renamed value;
  from_json(json{{"color", "blue"}, {"number", 7}}, value);

  CHECK(value.color == "red");
  CHECK(value.number == 1);
}

TEST_CASE("annotation: skip (serialization)") {
  CHECK(to_json(Skipped{}) == json{{"kept", 1}});
}

TEST_CASE("annotation: skip (deserialization)") {
  Skipped value;
  from_json(json{{"kept", 10}, {"skipped", 20}}, value);

  CHECK(value.kept == 10);
  CHECK(value.skipped == 2);
}

TEST_CASE("annotation: skip_serialize") {
  CHECK(to_json(SkippedOnSerialize{}) == json{{"kept", 1}});

  SkippedOnSerialize value;
  from_json(json{{"kept", 10}, {"write_only", 20}}, value);
  CHECK(value.kept == 10);
  CHECK(value.write_only == 20);
}

TEST_CASE("annotation: skip_deserialize") {
  CHECK(to_json(SkippedOnDeserialize{}) == json{{"kept", 1}, {"read_only", 2}});

  SkippedOnDeserialize value;
  from_json(json{{"kept", 10}, {"read_only", 20}}, value);
  CHECK(value.kept == 10);
  CHECK(value.read_only == 2);
}

TEST_CASE("annotation: combine rename and skip") {
  CHECK(to_json(RenamedAndSkipped{}) == json{{"visible", 1}});

  RenamedAndSkipped value;
  from_json(json{{"visible", 10}, {"hidden", 20}}, value);
  CHECK(value.kept == 10);
  CHECK(value.skipped == 2);
}
