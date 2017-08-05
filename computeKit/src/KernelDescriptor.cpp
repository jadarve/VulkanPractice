#include "ck/KernelDescriptor.hpp"

namespace ck {

KernelDescriptor& KernelDescriptor::setFunctionName(const std::string& name) {
    
    functionName = name;
    return *this;
}

KernelDescriptor& KernelDescriptor::addBufferParameter() {

    const vk::DescriptorSetLayoutBinding param {
        static_cast<uint32_t>(parameterBindings.size()),
        vk::DescriptorType::eStorageBuffer,
        1,
        vk::ShaderStageFlagBits::eCompute,
        nullptr
    };

    parameterBindings.push_back(param);
    return *this;
}

} // namespace ck