#include <cstdint>
#include <utility>

namespace sapeaob {
namespace utils {

template <typename TargetType, std::uint8_t... Bytes> struct byte_merging {
  byte_merging() = delete;

  constexpr static TargetType generate() {
    return generate_(std::make_index_sequence<sizeof...(Bytes)>());
  }

private:
  template <std::size_t... Indexes>
  constexpr static TargetType generate_(std::index_sequence<Indexes...>) {
    constexpr std::size_t number_of_bytes = sizeof...(Bytes);
    constexpr std::size_t type_size = sizeof(TargetType);
    if constexpr (number_of_bytes != type_size) {
      throw size_error();
    }
    return (... | join_bytes<Bytes>(Indexes));
  }

  template <std::uint8_t Byte>
  constexpr static TargetType join_bytes(std::size_t pos) noexcept {
    return static_cast<TargetType>(Byte) << (8 * pos);
  }
};

} // namespace utils
} // namespace sapeaob
