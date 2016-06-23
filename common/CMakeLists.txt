cmake_minimum_required(VERSION 2.8)

include_directories(include)

set (CMAKE_CXX_FLAGS            "-std=c++11")

set (LIBS
    dl
)

set (SRCS
	src/Vk_Library.cpp
	src/Vk_Instance.cpp
)

message(STATUS "Sources: ${SRCS}")

add_definitions( -DVK_NO_PROTOTYPES )

add_library(vkPractice STATIC ${SRCS})
target_link_libraries(vkPractice ${LIBS})
