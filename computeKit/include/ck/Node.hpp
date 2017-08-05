#ifndef NODE_HPP_
#define NODE_HPP_

#include <string>
#include <memory>
#include <vulkan/vulkan.hpp>

#include "ck/Buffer.hpp"
#include "ck/Kernel.hpp"

// compute kit
namespace ck {


class Node {


public:
    Node();
    Node(vk::Device& device, const ck::Kernel& kernel);
    ~Node();

    void bind(int index, const ck::Buffer& buffer);

    void record(vk::CommandBuffer& commandBufer) const;

private:

    void init(const ck::Kernel& kernel);

    vk::Device device;
    vk::PipelineLayout layout;
    vk::Pipeline pipeline;

    vk::DescriptorSet descriptorSet;
    vk::DescriptorPool descriptorPool;

    // reference counter for pipeline and layout
    std::shared_ptr<int> referenceCounter;
};

} // namespace ck

#endif /* NODE_HPP_ */
