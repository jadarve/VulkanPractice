#ifndef NODE_DESCRIPTOR_HPP_
#define NODE_DESCRIPTOR_HPP_

#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "ck/Program.hpp"

// compute kit
namespace ck {

// forward declarations
// class Node;
// class Program;


class NodeDescriptor {

public:
    NodeDescriptor& setProgram(const ck::Program& program);
    NodeDescriptor& setFunctionName(const std::string& name);
    NodeDescriptor& addBufferParameter();

    // TODO: specialization constants

    std::vector<vk::DescriptorPoolSize> getDescriptorPoolSizes() const;

    std::string getFunctionName() const;
    ck::Program getProgram() const;
    std::vector<vk::DescriptorSetLayoutBinding> getParameterBindings() const;


private:
    ck::Program program;
    std::string functionName;
    std::vector<vk::DescriptorSetLayoutBinding> parameterBindings;

    // counters for layout parameters
    uint32_t bufferCount {0};

};

} // namespace ck

#endif /* NODE_DESCRIPTOR_HPP_ */
