cmake_minimum_required(VERSION 3.8)
project("sapeaob" C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory(src)
add_subdirectory(vendor)

if(BUILD_TESTING)
	include(CTest)
	add_subdirectory(tests)
endif()

if (BUILD_BENCHMARK)
  add_subdirectory(benchmark)
endif()

