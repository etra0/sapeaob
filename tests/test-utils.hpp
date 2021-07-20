#include <vector>
#include <doctest/doctest.h>
#include <sapeaob/sapeaob.hpp>
#include <string>
#include <vector>

#define CHECK_RIGHT_VALUE(Fx, Val)                                             \
  do {                                                                         \
    CHECK(*reinterpret_cast<std::uint8_t *>(Fx) == Val);                       \
  } while (false)

auto reinterpret_it =
    [](std::vector<std::uint8_t>::iterator it) -> std::uintptr_t {
  return reinterpret_cast<std::uintptr_t>(static_cast<std::uint8_t *>(&*it));
};