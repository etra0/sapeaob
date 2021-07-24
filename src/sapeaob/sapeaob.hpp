#include <immintrin.h>
#include <iostream>
#include <optional>
#include <sapeaob/errors.hpp>
#include <sapeaob/utils.hpp>

namespace sapeaob {

namespace impl {
// step_calculator takes the first byte of the pattern and creates a
// std::uint64_t type out of it by repeating this byte. Then, it checks if that
// exact byte is in a 8-byte lookup. In the case it is, it'll return the index
// of that byte. When it's not, it'll return 8 since we couldn't find it
template <class it, std::uint16_t... Pattern> class step_calculator {

  template <std::uint16_t First, std::uint16_t... Rest>
  static constexpr std::uint8_t extract_first_byte() {
	  return First;
  }

  constexpr std::uint64_t get_first_byte() {
    constexpr std::uint8_t first_value = this->extract_first_byte<Pattern...>();
    return utils::byte_merging<std::uint64_t, first_value, first_value, first_value, first_value, first_value,
        first_value, first_value, first_value>::generate();
  }

public:
  inline std::size_t get_step(it arr, std::size_t offset, std::size_t size) {
    if (offset + sizeof...(Pattern) + 8 <= size) {
      std::uint64_t casted_value = *(std::uint64_t *)&*(arr + offset);
      unsigned long step =
          find_index(casted_value ^ this->get_first_byte()).value_or(8);
      return step > 0 ? step : 1;
    } else {
      return 1;
    }
  }
};

static std::optional<unsigned long> find_index(std::uint64_t x) {
  using optional_long = std::optional<unsigned long>;
  std::uint64_t y;
  unsigned long n;
  const std::uint64_t special_value = 0x7F7F7F7F7F7F7F7F;
  y = (x & special_value) + special_value;
  y = ~(y | x | special_value);
  char result = _BitScanForward64(&n, y);
  n >>= 3;
  if (result != 0 && n <= 7) {
    return optional_long{n};
  }
  return std::nullopt;
}

template <std::uint16_t...> struct function_compare;

template <std::uint16_t B0, std::uint16_t B1, std::uint16_t B2,
          std::uint16_t B3, std::uint16_t B4, std::uint16_t B5,
          std::uint16_t B6, std::uint16_t B7, std::uint16_t... Rest>
struct function_compare<B0, B1, B2, B3, B4, B5, B6, B7, Rest...> {
  template <class it> constexpr static bool compare(it arr) {
    return function_compare<B0, B1, B2, B3, B4, B5, B6, B7>::compare_(arr, 0) &&
           function_compare<Rest...>::compare_(arr, 8);
  }

  template <class it>
  constexpr static bool compare_(it arr, std::size_t offset) {
    constexpr std::uint8_t v0 = is_any(B0);
    constexpr std::uint8_t v1 = is_any(B1);
    constexpr std::uint8_t v2 = is_any(B2);
    constexpr std::uint8_t v3 = is_any(B3);
    constexpr std::uint8_t v4 = is_any(B4);
    constexpr std::uint8_t v5 = is_any(B5);
    constexpr std::uint8_t v6 = is_any(B6);
    constexpr std::uint8_t v7 = is_any(B7);
    constexpr std::uint64_t mask = utils::byte_merging<std::uint64_t, v0, v1, v2, v3, v4, v5, v6, v7>::generate();
    constexpr std::uint64_t v = utils::byte_merging<std::uint64_t, (B0 & 0xFF), (B1 & 0xFF), (B2 & 0xFF), (B3 & 0xFF), (B4 & 0xFF), (B5 & 0xFF), (B6 & 0xFF), (B7 & 0xFF)>::generate();
    std::uint64_t target = *reinterpret_cast<std::uint64_t *>(&*(arr + offset));
    return ((v ^ target) & mask) == 0 &&
           function_compare<Rest...>::compare_(arr, offset + 8);
  }
};

template <std::uint16_t B0, std::uint16_t B1, std::uint16_t B2,
          std::uint16_t B3, std::uint16_t... Rest>
struct function_compare<B0, B1, B2, B3, Rest...> {
  template <class it> constexpr static bool compare(it arr) {
    return function_compare<B0, B1, B2, B3>::compare_(arr, 0) &&
           function_compare<Rest...>::compare_(arr, 4);
  }

  template <class it>
  constexpr static bool compare_(it arr, std::size_t offset) {
    constexpr std::uint8_t v0 = is_any(B0);
    constexpr std::uint8_t v1 = is_any(B1);
    constexpr std::uint8_t v2 = is_any(B2);
    constexpr std::uint8_t v3 = is_any(B3);
    constexpr std::uint32_t mask = utils::byte_merging<std::uint32_t, v0, v1, v2, v3>::generate();
    constexpr std::uint32_t v =
        utils::byte_merging<std::uint32_t, (B0 & 0xFF), (B1 & 0xFF), (B2 & 0xFF), (B3 & 0xFF)>::generate();
    std::uint32_t target = *reinterpret_cast<std::uint32_t *>(&*(arr + offset));
    return ((v ^ target) & mask) == 0 &&
           function_compare<Rest...>::compare_(arr, offset + 4);
  }
};

template <std::uint16_t B0, std::uint16_t B1, std::uint16_t... Rest>
struct function_compare<B0, B1, Rest...> {
  template <class it> constexpr static bool compare(it arr) {
    return function_compare<B0, B1>::compare_(arr, 0) &&
           function_compare<Rest...>::compare_(arr, 2);
  }

  template <class it>
  constexpr static bool compare_(it arr, std::size_t offset) {
    constexpr std::uint8_t v0 = is_any(B0);
    constexpr std::uint8_t v1 = is_any(B1);
    constexpr std::uint16_t mask = utils::byte_merging<std::uint16_t, v0, v1>::generate();
    constexpr std::uint16_t v = utils::byte_merging<std::uint16_t, (B0 & 0xFF), (B1 & 0xFF)>::generate();
    std::uint16_t target = *reinterpret_cast<std::uint16_t *>(&*(arr + offset));
    return ((v ^ target) & mask) == 0 &&
           function_compare<Rest...>::compare_(arr, offset + 2);
  }
};

template <std::uint16_t B0> struct function_compare<B0> {
  template <class it> constexpr static bool compare(it arr) {
    return function_compare<B0>::compare_(arr, 0);
  }

  template <class it>
  static constexpr bool compare_(it arr, std::size_t offset) {
    constexpr std::uint8_t v0 = is_any(B0);
    return ((B0 ^ *(arr + offset)) & v0) == 0;
  }
};

template <> struct function_compare<> {
  template <class it> constexpr static bool compare(it arr) = delete;

  template <class it>
  static constexpr bool compare_(it arr, std::size_t offset) {
    return true;
  };
};
} // namespace impl

enum op : std::uint16_t {
  ANY = 0x100,
};

constexpr std::uint8_t is_any(std::uint16_t v) {
  return (v & ANY) != 0 ? 0 : 0xFF;
}

constexpr std::uint32_t make_u32(std::uint8_t a, std::uint8_t b, std::uint8_t c,
                                 std::uint8_t d) {
  return (a << 24) | (b << 16) | (c << 8) | d;
}


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
