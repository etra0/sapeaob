#include <iostream>

template <std::uint8_t F, std::uint8_t... B>
struct function_compare {
  constexpr static bool compare(std::uint8_t* arr) {
    return function_compare::compare_<F, B...>(arr, 0);
  }

  template <class _ =  void>
  static constexpr bool compare_(std::uint8_t* arr, std::size_t offset) {
    return true;
  }

  template <std::uint8_t F, std::uint8_t... B>
  static constexpr bool compare_(std::uint8_t* arr, std::size_t offset) {
    return *(arr + offset) == F && function_compare::compare_<B...>(arr, offset + 1);
  }

};