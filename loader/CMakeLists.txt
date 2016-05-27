CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(loader)

set (CMAKE_CXX_FLAGS            "-std=c++11")

set (LIBS
    dl
)

add_executable(loader main.cpp)
target_link_libraries(loader ${LIBS})