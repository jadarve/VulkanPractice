#ifndef KERNEL_HPP_
#define KERNEL_HPP_

#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

// #include "ck/Program.hpp"
#include "ck/KernelDescriptor.hpp"

// compute kit
namespace ck {


// forward declarations
class Node;
class Program;

class Kernel {

public:
    Kernel();
    Kernel(const ck::Program& program, const ck::KernelDescriptor& desc);
    ~Kernel();


private:

    void init(const ck::Program& program, const ck::KernelDescriptor& desc);

    vk::Device device;
    vk::DescriptorSetLayout layout;
    vk::PipelineShaderStageCreateInfo stageInfo;

    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;

    std::shared_ptr<int> referenceCounter;

friend class Node;

};



} // namespace ck

#endif /* KERNEL_HPP_ */
