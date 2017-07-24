
#include "ck/Kernel.hpp"

#include <fstream>
#include <iostream>

namespace ck {

using namespace std;


Kernel::Kernel() :
    isBuilt(false) {
}


Kernel::Kernel(vk::Device device) :
    Kernel() {

    this->device = device;
}

Kernel::Kernel(Kernel&& k) :
    device(k.device),
    module(k.module),
    stageInfo(k.stageInfo),
    functionName(k.functionName),
    layout(k.layout),
    parameterBindings(k.parameterBindings),
    isBuilt(k.isBuilt) {

    cout << "Kernel::Kernel(Kernel&& k)" << endl;

    k.isBuilt = false;
}

Kernel::Kernel(const ck::Kernel& k) {
    cout << "Kernel::Kernel(const Kernel& k)" << endl;
    *this = k;
}


Kernel::~Kernel() {

    cout << "Kernel::~Kernel(): isBuilt: " << isBuilt << endl;

    if(isBuilt) {
        device.destroyDescriptorSetLayout(layout);
        isBuilt = false;
    }
}


Kernel& Kernel::setShaderModule(vk::ShaderModule shaderModule) {
    module = shaderModule;
    return *this;
}

Kernel& Kernel::setFunctionName(const std::string& functionName) {

    this->functionName = functionName;

    // instantiate the stage info
    stageInfo = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eCompute)
        .setModule(module)
        .setPName(this->functionName.c_str());

    return *this;
}

Kernel& Kernel::addBufferParameter() {

	parameterBindings.push_back(
	{static_cast<uint32_t>(parameterBindings.size()), vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute, nullptr});

	return *this;
}

void Kernel::build() {

    // construct the DescriptorSetLayout
    vk::DescriptorSetLayoutCreateInfo descLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
        .setBindingCount(parameterBindings.size())
        .setPBindings(parameterBindings.data());

    layout = device.createDescriptorSetLayout(descLayoutInfo);
    isBuilt = true;
}


Kernel& Kernel::operator = (Kernel&& k) {

    cout << "Kernel& Kernel::operator = (Kernel&& k)" << endl;

    *this = std::move(k);
    k.isBuilt = false;
    return *this;
}


Kernel& Kernel::operator = (const Kernel& k) {

    cout << "Kernel& Kernel::operator = (const Kernel& k)" << endl;

    device = k.device;
    module = k.module;
    stageInfo = k.stageInfo;
    functionName = k.functionName;
    layout = k.layout;
    parameterBindings = k.parameterBindings;
    isBuilt = k.isBuilt;
    return *this;
}

} // namespace ck