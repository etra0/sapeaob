#include <test-utils.hpp>

using namespace sapeaob;

TEST_CASE("Create diffent sized types") {

  SUBCASE("uint8_t") { CHECK(utils::merge_bytes<std::uint8_t>(0xAA) == 0xAA); }
  SUBCASE("uint16_t") {
    CHECK(utils::merge_bytes<std::uint16_t>(0xAA, 0xBB) == 0xBBAA);
  }
  SUBCASE("uint32_t") {
    CHECK(utils::merge_bytes<std::uint32_t>(0xAA, 0xBB, 0xCC, 0xDD) == 0xDDCCBBAA);
  }
  SUBCASE("uint64_t") {
    CHECK(utils::merge_bytes<std::uint64_t>(0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
                                         0x00, 0x11) == 0x1100FFEEDDCCBBAA);
  }
}

TEST_CASE("Match of size") {
  SUBCASE("Smaller") {
    CHECK_THROWS_AS((utils::merge_bytes<std::uint32_t>(0xAA, 0xBB)), size_error);
  }

  SUBCASE("Bigger") {
    CHECK_THROWS_AS((utils::merge_bytes<std::uint16_t>(0xAA, 0xBB, 0xCC, 0xDD)),
                    size_error);
  }
}
