SET(SRCS program_options.cpp)
SET(HEADERS program_options.h)

ADD_LIBRARY(program_options ${SRCS} ${HEADERS})
TARGET_LINK_LIBRARIES(program_options ${LIBS})