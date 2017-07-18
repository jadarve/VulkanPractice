#ifndef MEMORY_MANAGER_HPP_
#define MEMORY_MANAGER_HPP_

#include <vector>
#include <memory>

#include <vulkan/vulkan.hpp>


// compute kit
namespace ck {

class Buffer {

private:
    vk::Buffer buffer;
    size_t size;
    
};


} // namespace ck

#endif /* MEMORY_MANAGER_HPP_ */
