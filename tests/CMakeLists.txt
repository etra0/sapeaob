cmake_minimum_required(VERSION 3.8)

include(${PROJECT_SOURCE_DIR}/vendor/doctest/scripts/cmake/doctest.cmake)

add_executable(
	sapeaob-test
	test-aob.cpp
    test-pattern.cpp 
	test-utils.hpp
)

target_include_directories(
	sapeaob-test PRIVATE .
)

target_link_libraries(
	sapeaob-test
	PRIVATE
	doctest
	doctest_with_main
	sapeaob
)

doctest_discover_tests(sapeaob-test ADD_LABELS _)
