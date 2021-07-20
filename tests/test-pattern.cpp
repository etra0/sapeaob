#include <test-utils.hpp>
using namespace sapeaob;

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

TEST_CASE("Scan vector") {
  std::vector<std::uint8_t> v{0xFF, 0xAA, 0xBB, 0xAA, 0xDD, 0xAA};
  pattern<0xAA, ANY, 0xAA> p{};
  std::uintptr_t result = p.scan_match(v.begin(), v.size());

  CHECK(result == reinterpret_it(v.begin() + 1));
}

TEST_CASE("Pattern not found") {
  std::uint8_t test_arr[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xFF};
  pattern<0xAA, ANY, 0xFE> p{};
  std::uintptr_t result = p.scan_match(test_arr, sizeof(test_arr));
  REQUIRE_FALSE(result);
}

TEST_CASE("README example") {
  std::vector<std::uint8_t> test_arr{0xFF, 0xAA, 0xDF, 0xCC};
  sapeaob::pattern<0xAA, sapeaob::ANY, 0xCC> p{};

  // You can scan a vector
  std::uintptr_t result = p.scan_match(test_arr.begin(), test_arr.size());
  CHECK(result == reinterpret_it(test_arr.begin() + 1));

  // Or an array, or anything as long as you can offset it!
  std::uint8_t test_arr2[] = {0xCC, 0xFF, 0xAA, 0xEE, 0xCC};
  result = p.scan_match(test_arr2, sizeof(test_arr2));
  CHECK(result == reinterpret_cast<std::uintptr_t>(test_arr2 + 2));
}