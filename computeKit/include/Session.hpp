#ifndef SESSION_HPP_
#define SESSION_HPP_


#include <vulkan/vulkan.hpp>

// compute kit
namespace ck {


class Session {


public:
    Session();
    ~Session();


public:
    uint32_t getComputeFamilyQueueIndex();
    vk::Buffer createBuffer(const vk::MemoryPropertyFlags& flags, const size_t size);

private:
    void createInstance();
    void createDevice();
    uint32_t getFamilyQueueIndex();

private:
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;

    vk::DeviceMemory memory;
    size_t memorySize;


    uint32_t computeQueueFamilyIndex;

};

} // namespace ck

#endif /* SESSION_HPP_ */
