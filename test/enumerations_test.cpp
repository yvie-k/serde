#include "doctest.h"

import std;

import nlohmann.json;

import serde;
import serde.test.json;

using serde::test::from_json;
using serde::test::round_trip;
using serde::test::to_json;

namespace {

enum Color {
  Red,
  Green,
  Blue,
  White,
};

enum class Direction : std::uint8_t {
  North = 10,
  South = 20,
};

} // namespace

TEST_CASE("enumerators serialize as their identifier") {
  CHECK(to_json(Color::Red) == nlohmann::json("Red"));
  CHECK(to_json(Color::White) == nlohmann::json("White"));
  CHECK(to_json(Direction::South) == nlohmann::json("South"));
}

TEST_CASE("enumerators deserialize from their identifier") {
  CHECK(from_json<Color>(nlohmann::json("Green")) == Color::Green);
  CHECK(from_json<Direction>(nlohmann::json("North")) == Direction::North);
}

TEST_CASE("enumerators survive a round trip") {
  CHECK(round_trip(Color::Blue) == Color::Blue);
  CHECK(round_trip(Direction::South) == Direction::South);
}
