#include <cstdint>
#include <emmintrin.h>
#include <sapeaob/errors.hpp>
#include <smmintrin.h>
#include <utility>

#ifndef SAPEAOB_UTILS
#define SAPEAOB_UTILS

namespace sapeaob {
namespace utils {

// Takes a byte and a sequence of the size of TargetType and
// sprays that byte to the entire size of TargetType. For example
// if TargetType is std::uint32_t, then sizeof(std::uint32_t) = 4,
// and v = 0xC0, the result will be 0xC0C0C0C0.
// This function shouldn't be used by itself since it already requires a
// sequence as an input. You should use `merge_bytes` instead.
template <typename TargetType, std::size_t... Sequence>
constexpr TargetType spray_byte(std::uint8_t v,
                                std::index_sequence<Sequence...>) {
  TargetType result = 0;
  ((result |= static_cast<TargetType>(v) << (8 * Sequence)), ...);
  return result;
}

// Variant of merge_bytes which allows you to use a TargetType that's bigger
// than one and spray that byte using the `spray_byte` function. Useful when you
// have a big type and you want to cast a single byte to it.
template <typename TargetType>
constexpr TargetType merge_bytes(std::uint8_t item) {
  static_assert(sizeof(TargetType) > 1,
                "Don't use this function for types with size 1.");
  return spray_byte<TargetType>(item,
                                std::make_index_sequence<sizeof(TargetType)>());
};

// This function takes a variadic number of bytes and a type and join the bytes
// and creates a TargetType with those bytes joint. The number of bytes and the
// size of TargetType should be the same.
template <typename TargetType, class... Items>
constexpr TargetType merge_bytes(Items... items) {
  int pos = 0;
  TargetType result = 0;
  static_assert(sizeof(TargetType) == sizeof...(Items),
                "The TargetType and the number of bytes don't match.");
  ((result |= static_cast<TargetType>(items) << (8 * pos++)), ...);

  return result;
}

} // namespace utils
} // namespace sapeaob

#endif // !SAPEAOB_UTILS
