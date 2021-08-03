#include <cstdint>
#include <emmintrin.h>
#include <smmintrin.h>
#include <utility>
#include <sapeaob/errors.hpp>

#ifndef SAPEAOB_UTILS
#define SAPEAOB_UTILS

namespace sapeaob {
namespace utils {

template <typename TargetType, std::size_t... Sequence>
constexpr TargetType spray_byte(std::uint8_t v,
  std::index_sequence<Sequence...>) {
  TargetType result = 0;
  ((result |= static_cast<TargetType>(v) << (8 * Sequence)), ...);
  return result;
}

template <typename TargetType>
constexpr TargetType merge_bytes(std::uint8_t item) {
  static_assert(sizeof(TargetType) > 1,
                "Don't use this function for types with size 1.");
  int v = 0;
  return spray_byte<TargetType>(item, std::make_index_sequence<sizeof(TargetType)>());
};

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
