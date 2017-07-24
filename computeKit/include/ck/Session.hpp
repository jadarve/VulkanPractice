#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <vector>

#include <vulkan/vulkan.hpp>

#include "ck/MemoryManager.hpp"
#include "ck/Buffer.hpp"

// compute kit
namespace ck {


class Session {


public:
    Session();
    ~Session();


public:
    uint32_t getComputeFamilyQueueIndex();
    void allocateMemory(const vk::MemoryPropertyFlags flags, const size_t size);
    
    ck::Buffer createBuffer(const vk::MemoryPropertyFlags flags, const size_t size);
    std::vector<vk::MemoryPropertyFlags> getMemoryProperties() const;


private:
    void createInstance();
    void createDevice();
    uint32_t getFamilyQueueIndex();

private:
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    uint32_t computeQueueFamilyIndex;

    std::vector<ck::MemoryManager> memories;
    std::vector<ck::Buffer> buffers;

};

} // namespace ck

#endif /* SESSION_HPP_ */
