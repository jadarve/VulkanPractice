cmake_minimum_required(VERSION 2.8)

# set(VULKAN_SDK_ROOT ~/vulkan/VulkanSDK)
# set(VULKAN_SDK_VERSION 1.0.46.0)
# set(VULKAN_SDK_ARCH x86_64)
# set(VULKAN_SDK ${VULKAN_SDK_ROOT}/${VULKAN_SDK_VERSION}/${VULKAN_SDK_ARCH})

include_directories(include
    $ENV{VULKAN_SDK}/include)

set (CMAKE_CXX_FLAGS            "-std=c++11")

set (LIBS
    dl
)

set (SRCS
	src/Vk_Library.cpp
	src/Vk_Instance.cpp
	src/Vk_Device.cpp
)

message(STATUS "Sources: ${SRCS}")

add_definitions( -DVK_NO_PROTOTYPES )

add_library(vkPractice STATIC ${SRCS})
target_link_libraries(vkPractice ${LIBS})
