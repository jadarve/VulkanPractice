cmake_minimum_required(VERSION 2.8)
project(vulkanPractice)

# set(VULKAN_SDK_ROOT ~/vulkan/VulkanSDK)
# set(VULKAN_SDK_VERSION 1.0.46.0)
# set(VULKAN_SDK_ARCH x86_64)
# set(VULKAN_SDK ${VULKAN_SDK_ROOT}/${VULKAN_SDK_VERSION}/${VULKAN_SDK_ARCH})

#################################################
# VULKAN SETTINGS
#################################################
include_directories($ENV{VULKAN_SDK}/include)
link_directories($ENV{VULKAN_SDK}/lib)
# include_directories(${VULKAN_SDK}/include)


set (CMAKE_CXX_FLAGS            "-std=c++11")

add_subdirectory(common)
add_subdirectory(classLoader)
add_subdirectory(computePipeline)
add_subdirectory(computeKit)

