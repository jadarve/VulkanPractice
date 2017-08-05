#ifndef KERNEL_HPP_
#define KERNEL_HPP_

#include <string>

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
    vk::Device device;
    vk::DescriptorSetLayout layout;
    std::shared_ptr<int> referenceCounter;

friend class Node;

};



// class Kernel {

// public:
//     Kernel();
//     Kernel(vk::Device& device);
//     // Kernel(const ck::Kernel& k) = default;
//     // Kernel(Kernel&& k);

//     ~Kernel();

// public:
//     Kernel& setShaderModule(vk::ShaderModule shaderModule);
//     Kernel& setFunctionName(const std::string& functionName);
//     Kernel& addBufferParameter();
//     void build();

//     // Kernel& operator = (const Kernel& k) = default;
//     // Kernel& operator = (Kernel&& k);

// private:
//     vk::Device device;
//     vk::ShaderModule module;
//     vk::PipelineShaderStageCreateInfo stageInfo;
//     std::string functionName;
//     vk::DescriptorSetLayout layout;
//     std::vector<vk::DescriptorSetLayoutBinding> parameterBindings;

//     bool isBuilt;
//     std::shared_ptr<int> referenceCounter;


// friend class Node;

// };

} // namespace ck

#endif /* KERNEL_HPP_ */
