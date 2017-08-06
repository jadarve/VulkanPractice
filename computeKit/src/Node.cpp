#include "ck/Node.hpp"

#include <iostream>

namespace ck {

using namespace std;


Node::Node() {
    // nothing to do
}


Node::Node(vk::Device& device, const ck::NodeDescriptor& desc):
    device {device} {

    init(desc);
}


Node::~Node() {

    if(referenceCounter.use_count() == 1) {
        device.destroyPipeline(pipeline, nullptr);
        device.destroyPipelineLayout(pipelineLayout, nullptr);
        device.destroyDescriptorPool(descriptorPool, nullptr);
        device.destroyDescriptorSetLayout(descriptorSetLayout);
    }
}



void Node::bind(int index, const ck::Buffer& buffer) {

    vk::DescriptorBufferInfo descBufferInfo = vk::DescriptorBufferInfo()
        .setOffset(0)
        .setRange(VK_WHOLE_SIZE)
        .setBuffer(buffer.buffer);

    vk::WriteDescriptorSet writeDescSet = vk::WriteDescriptorSet()
        .setDescriptorType(vk::DescriptorType::eStorageBuffer)
        .setDstSet(descriptorSet)
        .setDstBinding(index)
        .setDescriptorCount(1)
        .setPBufferInfo(&descBufferInfo);

    // update the informacion of the descriptor set
    device.updateDescriptorSets(1, &writeDescSet, 0, nullptr);
}


void Node::record(vk::CommandBuffer& commandBufer) const {

    commandBufer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
    commandBufer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    commandBufer.dispatch(1, 1, 1);
}


void Node::init(const ck::NodeDescriptor& desc) {

    /////////////////////////////////////////////
    // Specialization constants
    /////////////////////////////////////////////

    const size_t size = sizeof(uint32_t);
    vector<vk::SpecializationMapEntry> specializationMapEntries {
        {1, 0*size, size},
        {2, 1*size, size},
        {3, 2*size, size}
    };

    std::vector<uint32_t> localGroup {desc.getLocalX(), desc.getLocalY(), desc.getLocalZ()};

    vk::SpecializationInfo specializationInfo = vk::SpecializationInfo()
        .setMapEntryCount(specializationMapEntries.size())
        .setPMapEntries(specializationMapEntries.data())
        .setDataSize(localGroup.size()*sizeof(uint32_t))
        .setPData(localGroup.data());

    /////////////////////////////////////////////
    // Pipeline stage info
    /////////////////////////////////////////////
    stageInfo = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eCompute)
        .setModule(desc.getProgram().getShaderModule())
        .setPName(desc.getFunctionName().c_str())
        .setPSpecializationInfo(&specializationInfo);


    /////////////////////////////////////////////
    // Descriptor pool and descriptor set
    /////////////////////////////////////////////
    std::vector<vk::DescriptorSetLayoutBinding> parameterBindings {desc.getParameterBindings()};
    vk::DescriptorSetLayoutCreateInfo descLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
        .setBindingCount(parameterBindings.size())
        .setPBindings(parameterBindings.data());

    descriptorSetLayout = device.createDescriptorSetLayout(descLayoutInfo);

    descriptorPoolSizes = desc.getDescriptorPoolSizes();
    descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo()
        .setMaxSets(1)
        .setPoolSizeCount(descriptorPoolSizes.size())
        .setPPoolSizes(descriptorPoolSizes.data());

    device.createDescriptorPool(&descriptorPoolCreateInfo, nullptr, &descriptorPool);

    // only one descriptor set for this Node object
    vk::DescriptorSetAllocateInfo descSetAllocInfo = vk::DescriptorSetAllocateInfo()
        .setDescriptorPool(descriptorPool)
        .setDescriptorSetCount(1)
        .setPSetLayouts(&descriptorSetLayout);

    device.allocateDescriptorSets(&descSetAllocInfo, &descriptorSet);


    /////////////////////////////////////////////
    // Compute pipeline
    /////////////////////////////////////////////
    vk::PipelineLayoutCreateInfo pipeLayoutInfo = vk::PipelineLayoutCreateInfo()
        .setSetLayoutCount(1)
        .setPSetLayouts(&descriptorSetLayout);

    pipelineLayout = device.createPipelineLayout(pipeLayoutInfo);
    vk::ComputePipelineCreateInfo computePipeInfo = vk::ComputePipelineCreateInfo()
        .setStage(stageInfo)
        .setLayout(pipelineLayout);

    // create the compute pipeline
    pipeline = device.createComputePipeline(nullptr, computePipeInfo);

    referenceCounter = std::make_shared<int>(0);
}


} // namespace ck