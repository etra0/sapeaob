#include <benchmark/benchmark.h>
#include <sapeaob/sapeaob.hpp>

#include <fstream>
#include <vector>
#include <iterator>

#ifndef DATAFILE
#error "Must define DATAFILE"
#endif

static void BM_small_binary(benchmark::State& state) {
  std::ifstream datafile(DATAFILE, std::ios::binary);
  std::vector<std::uint8_t> data(std::istreambuf_iterator<char>(datafile), {});
  sapeaob::pattern<0xC0, 0xFF, 0xEE> p {};
  for (auto _ : state) {
    std::uintptr_t result = p.scan_match(data.begin(), data.size());
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(BM_small_binary);
BENCHMARK_MAIN();
