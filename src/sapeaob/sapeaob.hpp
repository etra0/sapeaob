#include <iostream>

namespace sapeaob {

enum op {
  ANY = 0x100,
};

template <std::uint16_t F, std::uint16_t... B>
struct function_compare {
  template <class it>
  constexpr static bool compare(it arr) {
    return function_compare::compare_<it, F, B...>(arr, 0);
  }

  private:
  template <class it, class _ =  void>
  static constexpr bool compare_(it arr, [[maybe_unused]] std::size_t offset) {
    return true;
  }

  template <class it, std::uint16_t Fx, std::uint16_t... Bx>
  static constexpr bool compare_(it arr, std::size_t offset) {
    constexpr const std::uint8_t val = Fx & 0xFF;
    if constexpr ((Fx & ANY) != 0) {
      return function_compare::compare_<it, Bx...>(arr, offset + 1);
    }
    return *(arr + offset) == val && function_compare::compare_<it, Bx...>(arr, offset + 1);
  }

};

}
