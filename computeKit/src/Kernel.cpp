
#include "ck/Kernel.hpp"
#include "ck/Program.hpp"

#include <fstream>
#include <iostream>

namespace ck {

using namespace std;


Kernel::Kernel() {

}


Kernel::Kernel(const ck::Program& program, const ck::KernelDescriptor& desc):
    device{program.device} {

    init(program, desc);
}


Kernel::~Kernel() {

    if(referenceCounter.use_count() == 1) {
        device.destroyDescriptorSetLayout(layout);
    }
}


void Kernel::init(const ck::Program& program, const ck::KernelDescriptor& desc) {

    stageInfo = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eCompute)
        .setModule(program.module)
        .setPName(desc.functionName.c_str());

    vk::DescriptorSetLayoutCreateInfo descLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
        .setBindingCount(desc.parameterBindings.size())
        .setPBindings(desc.parameterBindings.data());

    layout = device.createDescriptorSetLayout(descLayoutInfo);

    descriptorPoolSizes = desc.getDescriptorPoolSizes();
    descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo()
        .setMaxSets(1)
        .setPoolSizeCount(descriptorPoolSizes.size())
        .setPPoolSizes(descriptorPoolSizes.data());

    referenceCounter = std::make_shared<int>(0);
}

} // namespace ck