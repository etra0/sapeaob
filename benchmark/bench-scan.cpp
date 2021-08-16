#include <benchmark/benchmark.h>
#include <sapeaob/sapeaob.hpp>

#include <fstream>
#include <vector>
#include <iterator>

#ifndef DATAFILE
#error "Must define DATAFILE"
#endif

#define CHECK(result, value)                                                   \
  do {                                                                         \
    if (result == 0)                                                           \
      throw std::runtime_error("Result is 0");                                     \
    std::uint32_t v = *reinterpret_cast<std::uint32_t *>(result);                \
    if (v != value)                                                            \
      throw std::runtime_error("Value is incorrect");                              \
  } while (false)

namespace sapeaob {
static void small_pattern(benchmark::State &state) {
  std::ifstream datafile(DATAFILE, std::ios::binary);
  std::vector<std::uint8_t> data(std::istreambuf_iterator<char>(datafile), {});
  sapeaob::pattern<0xC0, 0xFF, 0xEE> p{};
  std::uintptr_t result = 0;
  for (auto _ : state) {
    result = p.scan_match(data.begin(), data.size());
  }

  CHECK(result, 0xAAEEFFC0);
}

static void long_pattern(benchmark::State &state) {
  std::ifstream datafile(DATAFILE, std::ios::binary);
  std::vector<std::uint8_t> data(std::istreambuf_iterator<char>(datafile), {});
  sapeaob::pattern<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF, 0xEE, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA> p{};
  std::uintptr_t result = 0;
  for (auto _ : state) {
    result = p.scan_match(data.begin(), data.size());
  }

  CHECK(result, 0xEFBEADDE);
}

static void long_pattern_wildcard(benchmark::State &state) {
  using namespace sapeaob;
  std::ifstream datafile(DATAFILE, std::ios::binary);
  std::vector<std::uint8_t> data(std::istreambuf_iterator<char>(datafile), {});
  sapeaob::pattern<0xDE, literals::_, 0xBE, 0xEF, 0xC0, literals::_, 0xEE, 0xAA, 0xBB, literals::_, 0xDD, 0xEE, 0xFF, literals::_, 0x11, 0x22, 0x33, 0x44, literals::_, 0x66, 0x77, 0x88, 0x99, 0xAA> p{};
  std::uintptr_t result = 0;
  for (auto _ : state) {
    result = p.scan_match(data.begin(), data.size());
  }

  CHECK(result, 0xEFBEADDE);
}

BENCHMARK(small_pattern);
BENCHMARK(long_pattern);
BENCHMARK(long_pattern_wildcard);
}

BENCHMARK_MAIN();
