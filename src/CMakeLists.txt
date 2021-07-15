cmake_minimum_required (VERSION 3.8)

add_library(
	sapeaob
	main.cpp
	sapeaob/sapeaob.hpp
  )

target_include_directories(sapeaob PUBLIC .)

if (MSVC)
	target_compile_options(sapeaob PUBLIC /W3)
endif()
