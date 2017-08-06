#ifndef NODE_HPP_
#define NODE_HPP_

#include <string>
#include <memory>
#include <vulkan/vulkan.hpp>

#include "ck/Buffer.hpp"
#include "ck/NodeDescriptor.hpp"

// compute kit
namespace ck {


class Node {

public:
    Node();
    Node(vk::Device& device, const ck::NodeDescriptor& desc);
    ~Node();

    void bind(int index, const ck::Buffer& buffer);

    void record(vk::CommandBuffer& commandBufer) const;

private:

    // void init(const ck::Kernel& kernel);

    void init(const ck::NodeDescriptor& desc);

    vk::Device device;

    vk::DescriptorSetLayout descriptorSetLayout;
    vk::PipelineShaderStageCreateInfo stageInfo;

    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;

    vk::PipelineLayout pipelineLayout;
    vk::Pipeline pipeline;

    vk::DescriptorSet descriptorSet;
    vk::DescriptorPool descriptorPool;

    // specialization constants
    // vk::SpecializationInfo specializationInfo;
    // std::vector<vk::SpecializationMapEntry> specializationMapEntries;
    // // uint32_t local_x {1};

    // std::shared_ptr<uint32_t> local_x;

    // reference counter for pipeline and layout
    std::shared_ptr<int> referenceCounter;
};

} // namespace ck

#endif /* NODE_HPP_ */
