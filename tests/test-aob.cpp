#include <doctest/doctest.h>
#include <sapeaob/sapeaob.hpp>
#include <string>
#include <vector>

using namespace sapeaob;

TEST_CASE("check first byte is correct") {

  std::uint8_t test_arr[] = {0xAA, 0xBB, 0xCC, 0xDD};
  CHECK(function_compare<0xAA, 0xBB, 0xCC>::compare(test_arr));
  CHECK(function_compare<0xBB, 0xCC, 0xDD>::compare(test_arr + 1));

  // This one should fail.
  REQUIRE_FALSE(function_compare<0xAA, 0xBB, 0xAA>::compare(test_arr));
}

TEST_CASE("check aob with vector class") {
  std::vector<std::uint8_t> v{0xAA, 0xBB, 0xCC, 0xDD};
  CHECK(function_compare<0xAA, 0xBB, 0xCC>::compare(v.begin()));

  // This one should fail.
  REQUIRE_FALSE(function_compare<0xAA, 0xBB, 0xCC>::compare(v.begin() + 1));

  CHECK(function_compare<0xBB, 0xCC, 0xDD>::compare(v.begin() + 1));
}

TEST_CASE("Add the option to use any") {
  std::uint8_t test_arr[] = {0xAA, 0xBB, 0xAA, 0xDD, 0xAA};
  CHECK(function_compare<0xAA, ANY, 0xAA>::compare(test_arr));
  CHECK(function_compare<0xAA, ANY, 0xAA>::compare(test_arr + 2));

  // this one should fail
  REQUIRE_FALSE(function_compare<0xAA, ANY, 0xAA>::compare(test_arr + 1));
}
