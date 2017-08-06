#ifndef PROGRAM_HPP_
#define PROGRAM_HPP_

#include <string>

#include <vulkan/vulkan.hpp>

// compute kit
namespace ck {


class Program {

public:
    Program();
    Program(vk::Device& device, const std::string& filepath);
    ~Program();

    vk::ShaderModule getShaderModule() const;

private:
    vk::Device device;
    vk::ShaderModule module;

    std::shared_ptr<int> referenceCounter;

};

} // namespace ck

#endif /* PROGRAM_HPP_ */
