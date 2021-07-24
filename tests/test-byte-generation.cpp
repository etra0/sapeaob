#include <test-utils.hpp>

using namespace sapeaob;

TEST_CASE("Create diffent sized types") {

  SUBCASE("uint8_t") {
    CHECK(utils::byte_merging<std::uint8_t, 0xAA>::generate() == 0xAA);
  }
  SUBCASE("uint16_t") {
    CHECK(utils::byte_merging<std::uint16_t, 0xAA, 0xBB>::generate() == 0xBBAA);
  }
  SUBCASE("uint32_t") {
    CHECK(utils::byte_merging<std::uint32_t, 0xAA, 0xBB, 0xCC,
                              0xDD>::generate() == 0xDDCCBBAA);
  }
  SUBCASE("uint64_t") {
    CHECK(utils::byte_merging<std::uint64_t, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
                              0x00, 0x11>::generate() == 0x1100FFEEDDCCBBAA);
  }
}

TEST_CASE("Match of size") {
  auto value = utils::byte_merging<std::uint8_t, 0xAA>::generate();

  SUBCASE("Smaller") {
    CHECK_THROWS_AS(
        (utils::byte_merging<std::uint32_t, 0xAA, 0xBB>::generate()),
        size_error);
  }

  SUBCASE("Bigger") {
    CHECK_THROWS_AS((utils::byte_merging<std::uint16_t, 0xAA, 0xBB, 0xCC,
                                         0xDD>::generate()),
                    size_error);
  }
}
