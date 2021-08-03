cmake_minimum_required (VERSION 3.8)

add_library(sapeaob INTERFACE)

target_include_directories(sapeaob INTERFACE .)

if (MSVC)
	target_compile_options(sapeaob INTERFACE /W3)
else()
  target_compile_options(sapeaob INTERFACE -mavx2)
endif()
