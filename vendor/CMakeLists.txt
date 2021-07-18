cmake_minimum_required (VERSION 3.8)

if (BUILD_TESTING)
	include(doctest.cmake)
endif()

if (BUILD_BENCHMARK)
	include(benchmark.cmake)
endif()
