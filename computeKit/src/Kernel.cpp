
#include "ck/Kernel.hpp"

#include <fstream>
#include <iostream>

namespace ck {

using namespace std;


Kernel::Kernel() :
    isBuilt{false} {
    referenceCounter = std::make_shared<int>(0);
}


Kernel::Kernel(vk::Device& device) :
    Kernel() {

    this->device = device;
}

// Kernel::Kernel(Kernel&& k) :
//     device(k.device),
//     module(k.module),
//     stageInfo(k.stageInfo),
//     functionName(k.functionName),
//     layout(k.layout),
//     parameterBindings(k.parameterBindings),
//     isBuilt(k.isBuilt) {

//     cout << "Kernel::Kernel(Kernel&& k)" << endl;

//     k.isBuilt = false;
// }


Kernel::~Kernel() {

    cout << "Kernel::~Kernel(): isBuilt: " << isBuilt <<
        " ref count: " << referenceCounter.use_count() << endl;

    if(isBuilt && referenceCounter.use_count() == 1) {
        cout << "Kernel::~Kernel(): destroying kernel" << endl;
        device.destroyDescriptorSetLayout(layout);
        // isBuilt = false;
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


// Kernel& Kernel::operator = (Kernel&& k) {

//     cout << "Kernel& Kernel::operator = (Kernel&& k)" << endl;

//     *this = std::move(k);
//     k.isBuilt = false;
//     return *this;
// }


} // namespace ck