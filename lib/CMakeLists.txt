file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

add_library(program_option_lib ${HEADERS} ${SOURCES})
