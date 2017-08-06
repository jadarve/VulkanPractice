#include "ck/NodeDescriptor.hpp"

#include <vector>

namespace ck {

using namespace std;

NodeDescriptor& NodeDescriptor::setProgram(const ck::Program& program) {
    this->program = program;
    return *this;
}

NodeDescriptor& NodeDescriptor::setFunctionName(const std::string& name) {
    
    functionName = name;
    return *this;
}


NodeDescriptor& NodeDescriptor::addBufferParameter() {

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

std::vector<vk::DescriptorPoolSize> NodeDescriptor::getDescriptorPoolSizes() const {

    vector<vk::DescriptorPoolSize> poolSizes{
        {vk::DescriptorType::eStorageBuffer, bufferCount}
    };
    
    return poolSizes;
}

std::string NodeDescriptor::getFunctionName() const {
    return functionName;
}


ck::Program NodeDescriptor::getProgram() const {
    return program;
}


std::vector<vk::DescriptorSetLayoutBinding> NodeDescriptor::getParameterBindings() const {
    return parameterBindings;
}

} // namespace ck