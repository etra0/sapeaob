#include <iostream>
#include <sapeaob/errors.hpp>

namespace sapeaob {

enum op : std::uint16_t {
  ANY = 0x100,
};

template <std::uint16_t F, std::uint16_t... B> struct function_compare {
  template <class it> constexpr static bool compare(it arr) {
    return function_compare::compare_<it, F, B...>(arr, 0);
  }

private:
  template <class it, class _ = void>
  static constexpr bool compare_([[maybe_unused]] it arr,
                                 [[maybe_unused]] std::size_t offset) {
    return true;
  }

  template <class it, std::uint16_t Fx, std::uint16_t... Bx>
  static constexpr bool compare_(it arr, std::size_t offset) {
    constexpr const std::uint8_t val = Fx & 0xFF;
    // TODO: Find how to use a better constraint.
    static_assert(Fx < 0x101, "You can only use values of size 1");
    if constexpr ((Fx & ANY) != 0) {
      return function_compare::compare_<it, Bx...>(arr, offset + 1);
    }
    return *(arr + offset) == val &&
           function_compare::compare_<it, Bx...>(arr, offset + 1);
  }
};

template <std::uint16_t... PAT> struct pattern {
  explicit pattern(){};

  template <class it> std::size_t scan_match_offset(it arr, std::size_t size);
  template <class it> std::uintptr_t scan_match(it arr, std::size_t size);
  template <class it>
  std::vector<std::uintptr_t> find_all_matches(it arr, std::size_t size);

private:
  template <class it>
  std::size_t scan_match_offset(it arr, std::size_t size, std::size_t offset);
};

template <std::uint16_t... PAT>
template <class it>
std::size_t pattern<PAT...>::scan_match_offset(it arr, std::size_t size) {
  return this->scan_match_offset(arr, size, 0);
}

template <std::uint16_t... PAT>
template <class it>
std::size_t pattern<PAT...>::scan_match_offset(it arr, std::size_t size,
                                               std::size_t offset) {
  constexpr std::size_t pattern_size = sizeof...(PAT);

  while (offset + pattern_size <= size) {
    if (function_compare<PAT...>::compare(arr + offset)) {
      return offset;
    }
    offset++;
  };

  throw pattern_not_found();
}

template <std::uint16_t... PAT>
template <class it>
std::uintptr_t pattern<PAT...>::scan_match(it arr, std::size_t size) {
  std::uintptr_t result = 0;

  std::uintptr_t arr_ptr = reinterpret_cast<std::uintptr_t>(&arr[0]);
  std::size_t offset = this->scan_match_offset(arr, size);
  return arr_ptr + offset;
}

template <std::uint16_t... PAT>
template <class it>
std::vector<std::uintptr_t>
pattern<PAT...>::find_all_matches(it arr, std::size_t size) {
  std::vector<std::uintptr_t> matches{};
  std::size_t offset = 0;
  auto arr_ptr = reinterpret_cast<std::uintptr_t>(arr);
  bool subsequent_search = false;
  while (true) {
    try {
      offset = this->scan_match_offset(arr, size, offset + subsequent_search);
      matches.push_back(arr_ptr + offset);
      subsequent_search = true;
    } catch (sapeaob::pattern_not_found &e) {
      break;
    }
  }

  return matches;
}

} // namespace sapeaob
