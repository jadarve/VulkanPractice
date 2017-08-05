#ifndef PROGRAM_HPP_
#define PROGRAM_HPP_


#include "ck/KernelDescriptor.hpp"
#include "ck/Kernel.hpp"

#include <string>

#include <vulkan/vulkan.hpp>

// compute kit
namespace ck {


class Program {

public:
    Program();
    Program(vk::Device& device, const std::string& filepath);
    ~Program();

    ck::Kernel buildKernel(const KernelDescriptor& desc);

private:
    vk::Device device;
    vk::ShaderModule module;

    std::shared_ptr<int> referenceCounter;

friend class ck::Kernel;
};

} // namespace ck

#endif /* PROGRAM_HPP_ */
