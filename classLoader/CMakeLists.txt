cmake_minimum_required(VERSION 2.8)
project(classLoader)

include_directories(include
	../common/include
)
link_directories(${CMAKE_BINARY_DIR}/common)

set (CMAKE_CXX_FLAGS            "-std=c++11")

set (LIBS
    dl
    vkPractice
)

set (SRCS
	src/main.cpp
)

message(STATUS "Sources: ${SRCS}")

add_definitions( -DVK_NO_PROTOTYPES )

add_executable(loader ${SRCS})
target_link_libraries(loader ${LIBS})
