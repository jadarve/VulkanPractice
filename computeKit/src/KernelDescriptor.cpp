#include "ck/KernelDescriptor.hpp"

#include <vector>

namespace ck {

using namespace std;

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
    ++bufferCount;
    return *this;
}

std::vector<vk::DescriptorPoolSize> KernelDescriptor::getDescriptorPoolSizes() const {

    vector<vk::DescriptorPoolSize> poolSizes{
        {vk::DescriptorType::eStorageBuffer, bufferCount}
    };
    
    return poolSizes;
}


} // namespace ck