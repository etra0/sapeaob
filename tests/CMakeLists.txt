cmake_minimum_required(VERSION 3.8)

add_executable(
	sapeaob-test
	test-aob.cpp
)

target_link_libraries(
	sapeaob-test
	PRIVATE
	doctest
	doctest_with_main
	sapeaob
)
