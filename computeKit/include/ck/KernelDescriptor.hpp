#ifndef KERNEL_DESCRIPTOR_HPP_
#define KERNEL_DESCRIPTOR_HPP_

#include <string>

#include <vulkan/vulkan.hpp>

// compute kit
namespace ck {

// forward declarations
class Kernel;


class KernelDescriptor {

public:
    KernelDescriptor& setFunctionName(const std::string& name);
    KernelDescriptor& addBufferParameter();

private:
    std::string functionName;
    std::vector<vk::DescriptorSetLayoutBinding> parameterBindings;


friend class ck::Kernel;
};

} // namespace ck

#endif /* KERNEL_DESCRIPTOR_HPP_ */
