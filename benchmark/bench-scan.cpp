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
      throw std::exception("Result is 0");                                     \
    std::uint32_t v = *reinterpret_cast<std::uint32_t *>(result);                \
    if (v != value)                                                            \
      throw std::exception("Value is incorrect");                              \
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
  sapeaob::pattern<0xDE, ANY, 0xBE, 0xEF, 0xC0, ANY, 0xEE, 0xAA, 0xBB, ANY, 0xDD, 0xEE, 0xFF, ANY, 0x11, 0x22, 0x33, 0x44, ANY, 0x66, 0x77, 0x88, 0x99, 0xAA> p{};
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
