#ifndef NODE_HPP_
#define NODE_HPP_

#include <string>
#include <memory>
#include <vulkan/vulkan.hpp>

#include "ck/Kernel.hpp"

// compute kit
namespace ck {


class Node {


public:
    Node(ck::Kernel& kernel);
    ~Node();


private:
    vk::Device device;
    vk::PipelineLayout layout;
    vk::Pipeline pipeline;

    // reference counter for pipeline and layout
    std::shared_ptr<int> referenceCounter;
};

} // namespace ck

#endif /* NODE_HPP_ */
