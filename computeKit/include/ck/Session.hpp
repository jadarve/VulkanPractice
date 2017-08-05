#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <vector>
#include <string>

#include <vulkan/vulkan.hpp>

#include "ck/MemoryManager.hpp"
#include "ck/Buffer.hpp"
#include "ck/Kernel.hpp"
#include "ck/Node.hpp"
#include "ck/Program.hpp"

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

    ck::Program createProgram(const std::string& filepath);

    // vk::ShaderModule createShaderModule(const std::string& filename);
    ck::Node createNode(const ck::Kernel& kernel);

    void run(const ck::Node& node);

private:
    void createInstance();
    void createDevice();
    void initQueue();
    uint32_t getFamilyQueueIndex();

private:
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::Queue queue;
    uint32_t computeQueueFamilyIndex;

    std::vector<ck::MemoryManager> memories;
    std::vector<ck::Buffer> buffers;



};

} // namespace ck

#endif /* SESSION_HPP_ */
