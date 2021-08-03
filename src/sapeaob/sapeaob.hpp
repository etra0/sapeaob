#include <immintrin.h>
#include <iostream>
#include <optional>
#include <sapeaob/errors.hpp>
#include <sapeaob/utils.hpp>

namespace sapeaob {

enum op : std::uint16_t {
  ANY = 0x100,
};

namespace impl {

// Function factory which takes a pattern and creates a function which is a long
// concatenation of logical AND in order to check if the array is equal to the
// pattern. It doesn't has any size check so if you go out of bounds it will
// most likely crash. Should be use for internal stuff mostly.
template <std::uint16_t... Pattern> struct function_compare {
  template <class it> constexpr static bool compare(it arr) {
    return function_compare::compare_<it>(
        arr, std::make_index_sequence<sizeof...(Pattern)>());
  }

private:
  template <class it, std::size_t... Indexes>
  static inline constexpr bool
  compare_(it arr, std::index_sequence<Indexes...>) noexcept {
    return (... && compare_one_<it, Pattern>(arr, Indexes));
  }

  // If the current byte is equal to ANY, we skip that one.
  // We make sure we're not using any other value other than ANY that are bigger
  // than std::uint8_t
  template <class it, std::uint16_t Byte>
  static constexpr std::uint8_t compare_one_(it arr, std::size_t offset) {
    constexpr const std::uint8_t val = Byte & 0xFF;
    // TODO: Find how to use a better constraint.
    static_assert(Byte < 0x101, "You can only use values of size 1");
    constexpr std::uint8_t skip = (Byte & ANY) != 0;
    return skip || (*(arr + offset) == val);
  }
};

inline char bitscanfwd(unsigned long *n, unsigned long long y) {
#if _MSVC
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
template <class it, std::uint16_t... Pattern> class step_calculator {
  template <class First, class... Items>
  static constexpr std::uint8_t get_first_byte(First first, Items... _) {
    return first & 0xFF;
  }

  constexpr std::uint64_t spray_first_byte() {
    std::uint8_t first_value = this->get_first_byte(Pattern...);
    return utils::merge_bytes<std::uint64_t>(
        first_value, first_value, first_value, first_value, first_value,
        first_value, first_value, first_value);
  }

public:
  explicit step_calculator() {};

  inline std::size_t get_step(it arr, std::size_t offset, std::size_t size) {
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

template <std::uint16_t... Pattern> struct pattern {
  explicit pattern(){};

  template <class it> std::size_t scan_match_offset(it arr, std::size_t size);
  template <class it> std::uintptr_t scan_match(it arr, std::size_t size);
  template <class it>
  std::vector<std::uintptr_t> find_all_matches(it arr, std::size_t size);

private:
  template <class it>
  std::size_t scan_match_offset(it arr, std::size_t size, std::size_t offset);
};

template <std::uint16_t... Pattern>
template <class it>
std::size_t pattern<Pattern...>::scan_match_offset(it arr, std::size_t size) {
  return this->scan_match_offset(arr, size, 0);
}

template <std::uint16_t... Pattern>
template <class it>
std::size_t pattern<Pattern...>::scan_match_offset(it arr, std::size_t size,
                                                   std::size_t offset) {
  constexpr std::size_t pattern_size = sizeof...(Pattern);
  int v = 0;
  impl::step_calculator<it, Pattern...> sc;

  while (offset + pattern_size <= size) {
    if (impl::function_compare<Pattern...>::compare(arr + offset)) {
      return offset;
    }
    offset += sc.get_step(arr, offset, size);
  };

  throw pattern_not_found();
}

template <std::uint16_t... Pattern>
template <class it>
std::uintptr_t pattern<Pattern...>::scan_match(it arr, std::size_t size) {
  std::uintptr_t arr_ptr = reinterpret_cast<std::uintptr_t>(&arr[0]);
  std::size_t offset = this->scan_match_offset(arr, size);
  return arr_ptr + offset;
}

template <std::uint16_t... Pattern>
template <class it>
std::vector<std::uintptr_t>
pattern<Pattern...>::find_all_matches(it arr, std::size_t size) {
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
