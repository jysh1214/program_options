cmake_minimum_required(VERSION 3.24)

project(program_option CXX)
include(CTest)

option(BUILD_TEST "Build unit tests." ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)

if (BUILD_TEST)
  add_subdirectory(test)
endif()
