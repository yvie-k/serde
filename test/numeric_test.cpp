#include "doctest.h"

import std;

import nlohmann.json;

import serde;
import serde.test.json;

using serde::test::from_json;
using serde::test::round_trip;
using serde::test::to_json;

TEST_CASE("serialize signed integer") {
  CHECK(to_json(std::int32_t{42}) == nlohmann::json(42));
  CHECK(to_json(std::int64_t{-1}) == nlohmann::json(-1));
  CHECK(to_json(short{-32768}) == nlohmann::json(-32768));

  CHECK(to_json(std::int64_t{-1}).is_number_integer());
}

TEST_CASE("serialize unsigned integer") {
  CHECK(to_json(std::uint32_t{42}) == nlohmann::json(42u));
  CHECK(to_json(std::numeric_limits<std::uint64_t>::max()) ==
        nlohmann::json(std::numeric_limits<std::uint64_t>::max()));

  CHECK(to_json(std::uint32_t{42}).is_number_unsigned());
}

TEST_CASE("serialize float") {
  CHECK(to_json(1.5).is_number_float());
  CHECK(to_json(1.5) == nlohmann::json(1.5));
  CHECK(to_json(1.5f) == nlohmann::json(1.5));

  CHECK(from_json<double>(nlohmann::json(1.5)) == doctest::Approx(1.5));
  CHECK(from_json<float>(nlohmann::json(1.5)) == doctest::Approx(1.5f));

  CHECK(from_json<double>(nlohmann::json(4)) == doctest::Approx(4.0));
  CHECK(from_json<float>(nlohmann::json(4)) == doctest::Approx(4.0f));
}

TEST_CASE("serialize numbers round trip") {
  CHECK(round_trip(std::int32_t{-12345}) == -12345);
  CHECK(round_trip(std::uint16_t{65535}) == 65535);
  CHECK(round_trip(std::int64_t{std::numeric_limits<std::int64_t>::min()}) ==
        std::numeric_limits<std::int64_t>::min());
  CHECK(round_trip(2.25) == doctest::Approx(2.25));
  CHECK(round_trip(2.25f) == doctest::Approx(2.25f));
}

TEST_CASE("deserialize integers") {
  SUBCASE("0 is valid as signed and unsigned") {
    CHECK(from_json<std::int32_t>(nlohmann::json(0)) == 0);
    CHECK(from_json<std::uint32_t>(nlohmann::json(0)) == 0);
  }

  SUBCASE("a negative value is invalid as unsigned") {
    std::uint32_t value = 123;
    from_json(nlohmann::json(-7), value);
    CHECK(value == 123);
  }
}

TEST_CASE("serialize bool") {
  CHECK(to_json(true) == nlohmann::json(true));
  CHECK(round_trip(true) == true);
  CHECK(round_trip(false) == false);
}
