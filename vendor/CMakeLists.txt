cmake_minimum_required (VERSION 3.8)

if (SAPEAOB_BUILD_TESTING)
	include(doctest.cmake)
endif()

if (SAPEAOB_BUILD_BENCHMARK)
	include(benchmark.cmake)
endif()
