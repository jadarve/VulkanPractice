#ifndef MEMORY_MANAGER_HPP_
#define MEMORY_MANAGER_HPP_

#include <vector>

#include <vulkan/vulkan.hpp>


// compute kit
namespace ck {

class MemoryManager {

public:
    MemoryManager();
    MemoryManager(  vk::PhysicalDevice& physicalDevice,
                    vk::Device& device,
                    const vk::MemoryPropertyFlags flags,
                    const size_t size);
    ~MemoryManager();


public:
    size_t getSize() const;
    vk::MemoryPropertyFlags getMemoryFlags() const;

private:
    vk::Device device;
    vk::DeviceMemory memory;
    vk::MemoryPropertyFlags memoryFlags;
    size_t size;


    // size_t pageSize;
    // std::vector<vk::DeviceMemory> memoryPages;
};


} // namespace ck

#endif /* MEMORY_MANAGER_HPP_ */
