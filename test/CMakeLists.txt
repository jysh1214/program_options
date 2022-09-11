include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG main
)
FetchContent_MakeAvailable(googletest)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
include(googletest)

enable_testing()

set(INCLUDES ${PROJECT_SOURCE_DIR}/lib)
set(LIBRARIES gtest program_option_lib)

include_directories(${INCLUDES})
add_executable(UnitTests unit_tests.cpp)
target_link_libraries(UnitTests ${LIBRARIES})

gtest_discover_tests(UnitTests)
# add_test(
#   NAME GUnitTests
#   COMMAND $<TARGET_FILE:UnitTests> 
# )