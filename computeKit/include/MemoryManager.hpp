#ifndef MEMORY_MANAGER_HPP_
#define MEMORY_MANAGER_HPP_

#include <vector>

#include <vulkan/vulkan.hpp>


// compute kit
namespace ck {

class MemoryManager {

public:
    MemoryManager(vk::Device& device);

private:
    vk::Device device;

    size_t pageSize;
    std::vector<vk::DeviceMemory> memoryPages;
};


} // namespace ck

#endif /* MEMORY_MANAGER_HPP_ */
