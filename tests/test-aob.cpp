#include <doctest/doctest.h>
#include <string>
#include <vector>
#include <sapeaob/sapeaob.hpp>

TEST_CASE("check first byte is correct") {
  std::uint8_t test_arr[] = { 0xAA, 0xBB, 0xCC, 0xDD };
  CHECK(function_compare<0xAA, 0xBB, 0xCC>::compare(test_arr));
  CHECK(function_compare<0xBB, 0xCC, 0xDD>::compare(test_arr + 1));

  // This one should fail.
  CHECK(!function_compare<0xAA, 0xBB, 0xAA>::compare(test_arr));
}

TEST_CASE("check aob with vector class") {
  std::vector<std::uint8_t> v{ 0xAA, 0xBB, 0xCC, 0xDD };
  CHECK(function_compare<0xAA, 0xBB, 0xCC>::compare(v.begin()));

  // This one should fail.
  CHECK(!function_compare<0xAA, 0xBB, 0xCC>::compare(v.begin() + 1));

  CHECK(function_compare<0xBB, 0xCC, 0xDD>::compare(v.begin() + 1));
}