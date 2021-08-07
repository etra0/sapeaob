#include <test-utils.hpp>

using namespace sapeaob;

TEST_CASE("Find right-most index") {
  std::optional<unsigned long> result =
      impl::find_index(0xCCEEAAFFAABBCCDD ^ 0xCCCCCCCCCCCCCCCC);
  CHECK(result == 1);

  result = impl::find_index(0xCCEEAAFFAACCFFDD ^ 0xCCCCCCCCCCCCCCCC);
  CHECK(result == 2);

  result = impl::find_index(0xCCEEAAFFCCBBAADD ^ 0xCCCCCCCCCCCCCCCC);
  CHECK(result == 3);

  result = impl::find_index(0xCCEEAACCAABBAADD ^ 0xCCCCCCCCCCCCCCCC);
  CHECK(result == 4);

  result = impl::find_index(0xCCEEAACAAABBAADD ^ 0xCCCCCCCCCCCCCCCC);
  CHECK(result == 7);

  result = impl::find_index(0xCCCCAACAAABBAADD ^ 0xCCCCCCCCCCCCCCCC);
  CHECK(result == 6);

  result = impl::find_index(0xCACCAACAAABBAACC ^ 0xCCCCCCCCCCCCCCCC);
  CHECK(result == 0);

  result = impl::find_index(0xACCAAACAAABBAAC0 ^ 0xCCCCCCCCCCCCCCCC);
  CHECK(result == std::nullopt);

  result = impl::find_index(0xCACAAACAAABBAACA ^ 0xCCCCCCCCCCCCCCCC);
  CHECK(result == std::nullopt);
}

TEST_CASE("check first byte is correct") {

  std::uint8_t test_arr[] = {0xAA, 0xBB, 0xCC, 0xDD};
  CHECK(impl::function_compare<0xAA, 0xBB, 0xCC>::compare(test_arr));
  CHECK(impl::function_compare<0xBB, 0xCC, 0xDD>::compare(test_arr + 1));

  SUBCASE("Incorrect pattern") {
    // This one should fail.
    REQUIRE_FALSE(impl::function_compare<0xAA, 0xBB, 0xAA>::compare(test_arr));
  }
}

TEST_CASE("check aob with vector class") {
  std::vector<std::uint8_t> v{0xAA, 0xBB, 0xCC, 0xDD};
  CHECK(impl::function_compare<0xAA, 0xBB, 0xCC>::compare(v.begin()));

  // This one should fail.
  REQUIRE_FALSE(
      impl::function_compare<0xAA, 0xBB, 0xCC>::compare(v.begin() + 1));

  CHECK(impl::function_compare<0xBB, 0xCC, 0xDD>::compare(v.begin() + 1));
}

TEST_CASE("Add the option to use any") {
  std::uint8_t test_arr[] = {0xAA, 0xBB, 0xAA, 0xDD, 0xAA};
  CHECK(impl::function_compare<0xAA, ANY, 0xAA>::compare(test_arr));
  CHECK(impl::function_compare<0xAA, ANY, 0xAA>::compare(test_arr + 2));

  // this one should fail
  REQUIRE_FALSE(impl::function_compare<0xAA, ANY, 0xAA>::compare(test_arr + 1));
}

// This is discouraged but anyway we need to check this works.
TEST_CASE("Single value to scan") {
  std::uint8_t test_arr[] = {0xAA, 0xBB, 0xCC};
  CHECK(impl::function_compare<0xAA>::compare(test_arr));

  CHECK(impl::function_compare<ANY>::compare(test_arr));

  REQUIRE_FALSE(impl::function_compare<0xBB>::compare(test_arr));
}

TEST_CASE("Different pattern sizes") {
  std::uint8_t test_arr[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF, 0xEE, 0xAA,
                             0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11, 0x22,
                             0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA};
  SUBCASE("17 bytes") {
    CHECK(impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF, 0xEE, 0xAA,
                                 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11, 0x22,
                                 0x33>::compare(test_arr));

    REQUIRE_FALSE(
        impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF, 0xEE, 0xAA,
                               0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11, 0x22,
                               0x32>::compare(test_arr));
    REQUIRE_FALSE(
        impl::function_compare<0xDE, 0xAD, 0xBA, 0xEF, 0xC0, 0xFF, 0xEE, 0xAA,
                               0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11, 0x22,
                               0x33>::compare(test_arr));

    REQUIRE_FALSE(
        impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF, 0xEE, 0xAA,
                               0xBB, 0xCF, 0xDD, 0xEE, 0xFA, 0x00, 0x11, 0x22,
                               0x32>::compare(test_arr));
  }

  SUBCASE("11 bytes") {
    CHECK(impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF, 0xEE, 0xAA,
                                 0xBB, 0xCC, 0xDD>::compare(test_arr));

    REQUIRE_FALSE(
        impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF, 0xEE, 0xAA,
                               0xBB, 0xCC, 0xDA>::compare(test_arr));
    REQUIRE_FALSE(
        impl::function_compare<0xDE, 0xAD, 0xBE, 0xEA, 0xC0, 0xFF, 0xEE, 0xAA,
                               0xBB, 0xCC, 0xDD>::compare(test_arr));
  }

  SUBCASE("10 bytes") {
    CHECK(impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF, 0xEE, 0xAA,
                                 0xBB, 0xCC>::compare(test_arr));

    REQUIRE_FALSE(
        impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF, 0xEE, 0xAA,
                               0xBB, 0xCA>::compare(test_arr));
    REQUIRE_FALSE(
        impl::function_compare<0xDE, 0xAD, 0xBF, 0xEF, 0xC0, 0xFF, 0xEE, 0xAA,
                               0xBB, 0xCC>::compare(test_arr));
  }

  SUBCASE("9 bytes") {
    CHECK(impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF, 0xEE, 0xAA,
                                 0xBB>::compare(test_arr));

    REQUIRE_FALSE(impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF,
                                         0xEE, 0xAA, 0xBF>::compare(test_arr));
    REQUIRE_FALSE(impl::function_compare<0xDE, 0xAF, 0xBE, 0xEF, 0xC0, 0xFF,
                                         0xEE, 0xAA, 0xBB>::compare(test_arr));
  }

  SUBCASE("8 bytes") {
    CHECK(impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF, 0xEE,
                                 0xAA>::compare(test_arr));

    REQUIRE_FALSE(impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF,
                                         0xEE, 0xAF>::compare(test_arr));
    REQUIRE_FALSE(impl::function_compare<0xDE, 0xAF, 0xBE, 0xEF, 0xC0, 0xFF,
                                         0xEE, 0xAA>::compare(test_arr));
  }

  SUBCASE("7 bytes") {
    CHECK(impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF,
                                 0xEE>::compare(test_arr));

    REQUIRE_FALSE(impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF,
                                         0xEF>::compare(test_arr));
    REQUIRE_FALSE(impl::function_compare<0xDE, 0xAD, 0xBE, 0xEA, 0xC0, 0xFF,
                                         0xEE>::compare(test_arr));
  }

  SUBCASE("6 bytes") {
    CHECK(impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF>::compare(
        test_arr));

    REQUIRE_FALSE(
        impl::function_compare<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFA>::compare(
            test_arr));
    REQUIRE_FALSE(
        impl::function_compare<0xDE, 0xAD, 0xBA, 0xEF, 0xC0, 0xFF>::compare(
            test_arr));
  }
}

TEST_CASE("4 byte pattern") {
  std::uint8_t test_arr[] = {0xFF, 0xAA, 0xBB, 0xCC, 0xDD};

  CHECK(impl::function_compare<0xFF, ANY, 0xBB, 0xCC>::compare(test_arr));
}
