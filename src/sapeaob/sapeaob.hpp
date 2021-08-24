#pragma warning(disable:26812)
#include <immintrin.h>
#include <iostream>
#include <optional>
#include <sapeaob/errors.hpp>
#include <sapeaob/utils.hpp>

namespace sapeaob {

namespace literals {
enum literal_ { _ };
}

namespace impl {

// Function factory which takes a pattern and creates a function which is a long
// concatenation of logical AND in order to check if the array is equal to the
// pattern. It doesn't has any size check so if you go out of bounds it will
// most likely crash. Should be use for internal stuff mostly.
template <auto... Pattern> struct function_compare {
  // This function calls compare_ which takes an index_sequence to avoid
  // recursive solution using C++17 goodies. compare_ will call compare_one_ for
  // every byte in the pattern. This specification was needed since you have
  // special cases for certain bytes, specifically to the byte 0x100 which means
  // ANY, that acts as a wildcard in the pattern.
  template <class it> constexpr static bool compare(const it& arr) {
    return function_compare::compare_<it>(
        arr, std::make_index_sequence<sizeof...(Pattern)>());
  }

private:
  template <class it, std::size_t... Indexes>
  static inline constexpr bool
  compare_(const it& arr, std::index_sequence<Indexes...>) noexcept {
    return (... && compare_one_(arr, Indexes, Pattern));
  }

  // If the current byte is equal to ANY, we skip that one.
  // We make sure we're not using any other value other than ANY that are bigger
  // than std::uint8_t
  template <class it, class V>
  static inline constexpr std::uint8_t compare_one_(const it& arr, std::size_t offset,
                                                    V byte) {
    if constexpr (std::is_same_v<V, literals::literal_>) {
      return true;
    }
    return (*(arr + offset) == static_cast<unsigned int>(byte));
  }
};

// Wrapper around the BSF instruction for x86 assembly. This one uses the x64
// bits variant.
inline char bitscanfwd(unsigned long *n, unsigned long long y) {
#ifdef _WIN32
  return _BitScanForward64(n, y);
#else
  if (y == 0) {
    return 0;
  } else {
    *n = __builtin_ctzll(y);
    return 1;
  }
#endif
}

// Finds the first 0 byte (byte, not bit) from LSB to MSB
// in a little-endian machine. If no byte is found, it returns nullopt.
static std::optional<unsigned long> find_index(std::uint64_t x) {
  using optional_long = std::optional<unsigned long>;
  const std::uint64_t special_value = 0x7F7F7F7F7F7F7F7F;
  std::uint64_t y;
  unsigned long n;
  y = (x & special_value) + special_value;
  y = ~(y | x | special_value);
  char result = bitscanfwd(&n, y);
  n >>= 3;
  if (result != 0 && n <= 7) {
    return optional_long{n};
  }
  return std::nullopt;
}

// step_calculator takes the first byte of the pattern and creates a
// std::uint64_t type out of it by repeating this byte. Then, it checks if that
// exact byte is in a 8-byte lookup. In the case it is, it'll return the index
// of that byte. When it's not, it'll return 8 since we couldn't find it
template <class it, std::uint8_t First, auto... _> class step_calculator {
  constexpr std::uint64_t spray_first_byte() {
    return utils::merge_bytes<std::uint64_t>(First);
  }

public:
  explicit step_calculator(){};

  // returns the maximum step possible by checking if the first byte of the
  // pattern is found in a 8-byte lookup. If the first byte is not present in
  // the next 8 bytes, it will return 8. If it is present, it'll return the
  // offset in bytes between the current position and the position of the first
  // byte of the pattern. For example if the first byte is 0xC0, and the lookup
  // is 0xAABBC0DD_FFEE0011, this function will return 2 since 0xC0 is the third
  // byte in that lookup.
  inline std::size_t get_step(const it& arr, std::size_t offset, std::size_t size) {
    if (offset + 8 <= size) {
      std::uint64_t casted_value = *(std::uint64_t *)&*(arr + offset);
      unsigned long step =
          find_index(casted_value ^ this->spray_first_byte()).value_or(8);
      return step > 0 ? step : 1;
    } else {
      return 1;
    }
  }
};

} // namespace impl

template <auto... Pattern> struct pattern {
  explicit pattern() {
    static_assert(
        ((std::is_same_v<decltype(Pattern), literals::literal_> ||
          (0x00U <= static_cast<unsigned int>(Pattern) &&
           static_cast<unsigned int>(Pattern) <= 0xFFU)) &&
         ...),
        "One of the arguments is either not of size 1 or of type ANY");
  };

  // Search for the pattern in the specific array. If there's a match, it will
  // return the offset.
  // In the case there's nothing found, this function throws the
  // `sapeaob::pattern_not_found` exception.
  template <class it> std::size_t scan_match_offset(const it& arr, std::size_t size);

  // Search for the pattern. If there's a match, returns the pointer of the
  // match. If there's nothing found, this function throws the
  // `sapeaob::pattern_not_found` exception.
  template <class it> std::uintptr_t scan_match(const it& arr, std::size_t size);

  // Find all the matches in the array. If nothing is found, this function
  // returns an empty vector.
  template <class it>
  std::vector<std::uintptr_t> find_all_matches(const it& arr,
                                               std::size_t size) noexcept;

private:
  template <class it>
  // Implementation of the `scan_match_offset`. This one uses offset since if
  // there's a match, we can still iterate from the first offset.
  std::size_t scan_match_offset(const it& arr, std::size_t size, std::size_t offset);
};

template <auto... Pattern>
template <class it>
std::size_t pattern<Pattern...>::scan_match_offset(const it& arr, std::size_t size) {
  return this->scan_match_offset(arr, size, 0);
}

template <auto... Pattern>
template <class it>
std::size_t pattern<Pattern...>::scan_match_offset(const it& arr, std::size_t size,
                                                   std::size_t offset) {
  constexpr std::size_t pattern_size = sizeof...(Pattern);
  impl::step_calculator<it, Pattern...> sc;

  while (offset + pattern_size <= size) {
    if (impl::function_compare<Pattern...>::compare(arr + offset)) {
      return offset;
    }
    offset += sc.get_step(arr, offset, size);
  };

  throw pattern_not_found();
}

template <auto... Pattern>
template <class it>
std::uintptr_t pattern<Pattern...>::scan_match(const it& arr, std::size_t size) {
  std::uintptr_t arr_ptr = reinterpret_cast<std::uintptr_t>(&arr[0]);
  std::size_t offset = this->scan_match_offset(arr, size);
  return arr_ptr + offset;
}

template <auto... Pattern>
template <class it>
std::vector<std::uintptr_t>
pattern<Pattern...>::find_all_matches(const it& arr, std::size_t size) noexcept {
  std::vector<std::uintptr_t> matches{};
  std::size_t offset = 0;
  auto arr_ptr = reinterpret_cast<std::uintptr_t>(arr);
  bool subsequent_search = false;
  while (true) {
    try {
      offset = this->scan_match_offset(arr, size, offset + subsequent_search);
      matches.push_back(arr_ptr + offset);
      subsequent_search = true;
    } catch (sapeaob::pattern_not_found) {
      break;
    }
  }

  return matches;
}

} // namespace sapeaob
