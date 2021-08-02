#include <iostream>
#include <immintrin.h>
#include <optional>
#include <sapeaob/errors.hpp>
#include <sapeaob/utils.hpp>

namespace sapeaob {

enum op : std::uint16_t {
  ANY = 0x100,
};

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

  template <class it, std::uint16_t Byte>
  static constexpr std::uint8_t compare_one_(it arr, std::size_t offset) {
    constexpr const std::uint8_t val = Byte & 0xFF;
    // TODO: Find how to use a better constraint.
    static_assert(Byte < 0x101, "You can only use values of size 1");
    constexpr std::uint8_t skip = (Byte & ANY) != 0;
    return skip || (*(arr + offset) == val);
  }
};

template <class it, std::uint16_t... Pattern> struct step_calculator {
  constexpr std::uint64_t get_first_byte() {
    int v = 0;
    std::uint8_t first_value = (((v++ == 0 ? Pattern : 0)) | ...);
    return utils::merge_bytes<std::uint64_t>(
        first_value, first_value, first_value, first_value, first_value,
        first_value, first_value, first_value);
  }

  inline std::size_t get_step(it arr, std::size_t offset, std::size_t size) {
    if (offset + sizeof...(Pattern) + 8 <= size) {
      std::uint64_t casted_value = *(std::uint64_t *)&*(arr + offset);
      unsigned long step = find_index(casted_value ^ this->get_first_byte()).value_or(8);
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
  step_calculator<it, Pattern...> sc;
  

  while (offset + pattern_size <= size) {
    if (function_compare<Pattern...>::compare(arr + offset)) {
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
