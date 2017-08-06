#ifndef NODE_DESCRIPTOR_HPP_
#define NODE_DESCRIPTOR_HPP_

#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "ck/Program.hpp"

// compute kit
namespace ck {


class NodeDescriptor {

public:
    NodeDescriptor& setProgram(const ck::Program& program);
    NodeDescriptor& setFunctionName(const std::string& name);
    NodeDescriptor& addBufferParameter();

    NodeDescriptor& setLocalX(const uint32_t x);
    NodeDescriptor& setLocalY(const uint32_t y);
    NodeDescriptor& setLocalZ(const uint32_t z);

    std::vector<vk::DescriptorPoolSize> getDescriptorPoolSizes() const;

    std::string getFunctionName() const;
    ck::Program getProgram() const;
    uint32_t getLocalX() const;
    uint32_t getLocalY() const;
    uint32_t getLocalZ() const;
    std::vector<vk::DescriptorSetLayoutBinding> getParameterBindings() const;


private:
    ck::Program program;
    std::string functionName;
    std::vector<vk::DescriptorSetLayoutBinding> parameterBindings;

    // local work group
    std::vector<uint32_t> localGroup {1, 1, 1};

    // counters for layout parameters
    uint32_t bufferCount {0};

};

} // namespace ck

#endif /* NODE_DESCRIPTOR_HPP_ */
