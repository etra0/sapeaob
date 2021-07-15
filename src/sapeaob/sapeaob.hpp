#include <iostream>

template <std::uint8_t F, std::uint8_t... B>
struct function_compare {
  template <class it>
  constexpr static bool compare(it arr) {
    return function_compare::compare_<it, F, B...>(arr, 0);
  }

  template <class it, class _ =  void>
  static constexpr bool compare_(it arr, [[maybe_unused]] std::size_t offset) {
    return true;
  }

  template <class it, std::uint8_t Fx, std::uint8_t... Bx>
  static constexpr bool compare_(it arr, std::size_t offset) {
    return *(arr + offset) == Fx && function_compare::compare_<it, Bx...>(arr, offset + 1);
  }

};