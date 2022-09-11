cmake_minimum_required(VERSION 3.24)

project(program_option C CXX)
include(CTest)

option(BUILD_TEST "Build unit tests." ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)

if (BUILD_TEST)
  add_subdirectory(test)
endif()