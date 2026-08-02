#include "doctest.h"

import std;

import nlohmann.json;

import serde;
import serde.test.json;

using nlohmann::json;
using serde::test::from_json;
using serde::test::round_trip;
using serde::test::to_json;

namespace {

struct Point {
  int x = 0;
  int y = 0;

  bool operator==(const Point &) const = default;
};

struct Nested {
  Point point;
  double weight = 0.0;

  bool operator==(const Nested &) const = default;
};

class WithPrivateMembers {
public:
  WithPrivateMembers() = default;
  WithPrivateMembers(int value, std::string name)
      : value(value), name(std::move(name)) {}

  bool operator==(const WithPrivateMembers &) const = default;

private:
  int value = 0;
  std::string name;
};

struct Empty {};

} // namespace

TEST_CASE("serialize struct") {
  CHECK(to_json(Point{.x = 1, .y = 2}) == json{{"x", 1}, {"y", 2}});
}

TEST_CASE("serialize empty struct") {
  const json output = to_json(Empty{});
  CHECK(output.is_object());
  CHECK(output.empty());
}

TEST_CASE("serialize nested structs") {
  const Nested value{.point = {.x = 3, .y = 4}, .weight = 0.5};

  CHECK(to_json(value) ==
        json{{"point", {{"x", 3}, {"y", 4}}}, {"weight", 0.5}});
  CHECK(round_trip(value) == value);
}

TEST_CASE("serialize private members") {
  const WithPrivateMembers value{7, "seven"};

  CHECK(to_json(value) == json{{"value", 7}, {"name", "seven"}});
  CHECK(round_trip(value) == value);
}

TEST_CASE("serialize std:string") {
  CHECK(to_json(std::string("hello")) == json("hello"));
  CHECK(to_json(std::string("")) == json(""));
  CHECK(round_trip(std::string("hello")) == "hello");
}

TEST_CASE("serialize std::string_view") {
  CHECK(to_json(std::string_view("hello")) == json("hello"));
}

TEST_CASE("serialize vector<int>") {
  const std::vector<int> numbers{1, 2, 3};

  CHECK(to_json(numbers) == json::array({1, 2, 3}));
  CHECK(round_trip(numbers) == numbers);
}

TEST_CASE("serialize empty vector") {
  const json output = to_json(std::vector<int>{});
  CHECK(output.is_array());
  CHECK(output.empty());
}

TEST_CASE("serialize vector of structs") {
  const std::vector<Point> points{{.x = 1, .y = 2}, {.x = 3, .y = 4}};

  CHECK(to_json(points) ==
        json::array({{{"x", 1}, {"y", 2}}, {{"x", 3}, {"y", 4}}}));
  CHECK(round_trip(points) == points);
}

TEST_CASE("serialize nested vector") {
  const std::vector<std::vector<int>> nested{{1, 2}, {}, {3}};

  CHECK(to_json(nested) == json::array({{1, 2}, json::array(), {3}}));
  CHECK(round_trip(nested) == nested);
}

TEST_CASE_TEMPLATE("serialize std::(unordered_)map<std::string, std::string>",
                   T, std::map<std::string, std::string>,
                   std::unordered_map<std::string, std::string>) {
  const T kv{{"baz", "qux"}, {"foo", "bar"}};

  CHECK(to_json(kv) == json{{"baz", "qux"}, {"foo", "bar"}});
  CHECK(round_trip(kv) == kv);
}

TEST_CASE_TEMPLATE("serialize std::(unordered_)map<std::string, struct>", T,
                   std::map<std::string, Point>,
                   std::unordered_map<std::string, Point>) {
  const T points{{"a", {.x = 1, .y = 2}}};

  CHECK(to_json(points) == json{{"a", {{"x", 1}, {"y", 2}}}});
  CHECK(round_trip(points) == points);
}

TEST_CASE_TEMPLATE(
    "serialize std::(unordered_)map<std::string_view, std::string_view>", T,
    std::map<std::string_view, std::string_view>,
    std::unordered_map<std::string_view, std::string_view>) {
  const T kv{{"baz", "qux"}, {"foo", "bar"}};

  CHECK(to_json(kv) == json{{"baz", "qux"}, {"foo", "bar"}});
}

TEST_CASE("serialize smart pointers") {
  SUBCASE("unique_ptr") {
    const auto value = std::make_unique<Point>(Point{.x = 1, .y = 2});
    CHECK(to_json(value) == json{{"x", 1}, {"y", 2}});

    std::unique_ptr<Point> restored;
    from_json(json{{"x", 1}, {"y", 2}}, restored);
    REQUIRE(restored != nullptr);
    CHECK(*restored == Point{.x = 1, .y = 2});
  }

  SUBCASE("shared_ptr") {
    const auto value = std::make_shared<Point>(Point{.x = 1, .y = 2});
    CHECK(to_json(value) == json{{"x", 1}, {"y", 2}});

    std::shared_ptr<Point> restored;
    from_json(json{{"x", 1}, {"y", 2}}, restored);
    REQUIRE(restored != nullptr);
    CHECK(*restored == Point{.x = 1, .y = 2});
  }
}

TEST_CASE("serialize raw pointer") {
  const Point point{.x = 5, .y = 6};
  const Point *pointer = &point;

  CHECK(to_json(pointer) == json{{"x", 5}, {"y", 6}});
}

TEST_CASE("deserializing vector<int> clears content") {
  std::vector<int> numbers{1};
  from_json(json::array({2, 3}), numbers);

  CHECK(numbers == std::vector<int>{2, 3});
}
