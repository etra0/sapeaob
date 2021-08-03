#include <cstdint>
#include <emmintrin.h>
#include <smmintrin.h>
#include <utility>
#include <sapeaob/errors.hpp>

#ifndef SAPEAOB_UTILS
#define SAPEAOB_UTILS

namespace sapeaob {
namespace utils {

template <typename TargetType, class... Items>
constexpr TargetType merge_bytes(Items... items) {
  int pos = 0;
  TargetType result = 0;
  if constexpr (sizeof...(Items) != sizeof(TargetType)) {
    throw size_error();
  }
  ((result |= static_cast<TargetType>(items) << (8 * pos++)), ...);

  return result;
}

} // namespace utils
} // namespace sapeaob

#endif // !SAPEAOB_UTILS
