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

// This is discouraged but anyway we need to check this works.
TEST_CASE("Single value to scan") {
  std::uint8_t test_arr[] = {0xAA, 0xBB, 0xCC};
  CHECK(function_compare<0xAA>::compare(test_arr));

  CHECK(function_compare<ANY>::compare(test_arr));

  REQUIRE_FALSE(function_compare<0xBB>::compare(test_arr));
}

TEST_CASE("Scan for an AOB") {
  std::uint8_t test_arr[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xAA};
  pattern<0xCC, 0xDD> p{};
  std::uintptr_t result = p.scan_match(test_arr, 5);
  CHECK(result == reinterpret_cast<std::uintptr_t>(&*(test_arr + 2)));
}

TEST_CASE("Scan for an AOB with ANY") {
  std::uint8_t test_arr[] = {0xFF, 0xAA, 0xBB, 0xAA, 0xDD, 0xAA};
  pattern<0xAA, ANY, 0xAA> p{};
  std::uintptr_t result = p.scan_match(test_arr, sizeof(test_arr));

  CHECK(result == reinterpret_cast<std::uintptr_t>(test_arr + 1));

  result = p.scan_match(test_arr + 2, sizeof(test_arr) - 1);

  CHECK(result == reinterpret_cast<std::uintptr_t>(test_arr + 3));
}

TEST_CASE("Pattern not found") {
  std::uint8_t test_arr[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xFF};
  pattern<0xAA, ANY, 0xFE> p{};
  std::uintptr_t result = p.scan_match(test_arr, sizeof(test_arr));
  REQUIRE_FALSE(result);
}
