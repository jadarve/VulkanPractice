
include_directories(include)
add_executable(computeKit
    src/Session.cpp
    src/main.cpp)

target_link_libraries(computeKit vulkan)