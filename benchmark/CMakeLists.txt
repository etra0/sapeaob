cmake_minimum_required(VERSION 3.8)

add_compile_definitions(DATAFILE="${PROJECT_SOURCE_DIR}/benchmark/output.bin")

add_executable(
  bench-sapeaob
  bench-scan.cpp
)

target_link_libraries(
  bench-sapeaob
  PUBLIC
  sapeaob
  benchmark::benchmark
)

