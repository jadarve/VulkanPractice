#include "ck/NodeDescriptor.hpp"

#include <vector>
#include <iostream>

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


NodeDescriptor& NodeDescriptor::setLocalX(const uint32_t x) {
    localGroup[0] = x;
    return *this;
}


NodeDescriptor& NodeDescriptor::setLocalY(const uint32_t y) {
    localGroup[1] = y;
    return *this;
}


NodeDescriptor& NodeDescriptor::setLocalZ(const uint32_t z) {
    localGroup[2] = z;
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


uint32_t NodeDescriptor::getLocalX() const {
    return localGroup[0];
}


uint32_t NodeDescriptor::getLocalY() const {
    return localGroup[1];
}


uint32_t NodeDescriptor::getLocalZ() const {
    return localGroup[2];
}


// vk::SpecializationInfo NodeDescriptor::getSpecializationInfo() const {

//     std::cout << "vk::SpecializationInfo NodeDescriptor::getSpecializationInfo(): local x: " << localGroup[0] << std::endl;

//     const size_t size = sizeof(uint32_t);
//     vector<vk::SpecializationMapEntry> specializationMapEntries {
//         {1, 0*size, size},
//         {2, 1*size, size},
//         {3, 2*size, size}
//     };

//     vk::SpecializationInfo specializationInfo = vk::SpecializationInfo()
//         .setMapEntryCount(specializationMapEntries.size())
//         .setPMapEntries(specializationMapEntries.data())
//         .setDataSize(localGroup.size()*sizeof(uint32_t))
//         .setPData(localGroup.data());

//     vk::SpecializationInfo info;
//     return std::move(info);
// }

std::vector<vk::DescriptorSetLayoutBinding> NodeDescriptor::getParameterBindings() const {
    return parameterBindings;
}

} // namespace ck