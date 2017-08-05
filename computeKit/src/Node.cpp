#include "ck/Node.hpp"

#include <iostream>

namespace ck {

using namespace std;


Node::Node() {
    // nothing to do
}


Node::Node(vk::Device& device, const ck::Kernel& kernel):
    device {device} {

    init(kernel);
}


Node::~Node() {

    if(referenceCounter.use_count() == 1) {
        device.destroyDescriptorPool(descriptorPool, nullptr);
        device.destroyPipelineLayout(layout, nullptr);
        device.destroyPipeline(pipeline, nullptr);
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
    commandBufer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, layout, 0, 1, &descriptorSet, 0, nullptr);
    commandBufer.dispatch(1, 1, 1);
}


void Node::init(const ck::Kernel& kernel) {

    /////////////////////////////////////////////
    // Descriptor pool and descriptor set
    /////////////////////////////////////////////
    device.createDescriptorPool(&kernel.descriptorPoolCreateInfo, nullptr, &descriptorPool);

    // only one descriptor set for this Node object
    vk::DescriptorSetAllocateInfo descSetAllocInfo = vk::DescriptorSetAllocateInfo()
        .setDescriptorPool(descriptorPool)
        .setDescriptorSetCount(1)
        .setPSetLayouts(&kernel.layout);

    device.allocateDescriptorSets(&descSetAllocInfo, &descriptorSet);

    referenceCounter = std::make_shared<int>(0);


    /////////////////////////////////////////////
    // Compute pipeline
    /////////////////////////////////////////////
    vk::PipelineLayoutCreateInfo pipeLayoutInfo = vk::PipelineLayoutCreateInfo()
        .setSetLayoutCount(1)
        .setPSetLayouts(&kernel.layout);

    layout = device.createPipelineLayout(pipeLayoutInfo);


    // compute pipeline creation info. Use stageInfo (bytecode and entry function) and
    // layout (buffer binding)
    vk::ComputePipelineCreateInfo computePipeInfo = vk::ComputePipelineCreateInfo()
        .setStage(kernel.stageInfo)
        .setLayout(layout);

    // create the compute pipeline
    pipeline = device.createComputePipeline(nullptr, computePipeInfo);    
}

} // namespace ck