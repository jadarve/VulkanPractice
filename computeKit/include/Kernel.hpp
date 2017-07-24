#ifndef KERNEL_MANAGER_HPP_
#define KERNEL_MANAGER_HPP_

#include <string>

#include <vulkan/vulkan.hpp>

// compute kit
namespace ck {


class Kernel {


public:
    Kernel& addBufferParameter();

    Kernel& build();

private:
    vk::ShaderModule module;
    vk::PipelineShaderStageCreateInfo stageInfo;
    vk::DescriptorSetLayout layout;
    std::vector<vk::DescriptorSetLayoutBinding> parameterBindings;
    std::string functionName;
};

} // namespace ck

#endif /* KERNEL_MANAGER_HPP_ */
