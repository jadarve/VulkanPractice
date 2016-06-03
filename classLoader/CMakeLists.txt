cmake_minimum_required(VERSION 2.8)
project(classLoader)

include_directories(include)

set (CMAKE_CXX_FLAGS            "-std=c++11")

set (LIBS
    dl
)

set (SRCS
	src/loader.cpp
	src/main.cpp
)

add_definitions( -DVK_NO_PROTOTYPES )

add_executable(loader ${SRCS})
target_link_libraries(loader ${LIBS})