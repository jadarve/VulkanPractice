#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <vector>
#include <memory>

#include <vulkan/vulkan.hpp>


// compute kit
namespace ck {

// forward declarations
class MemoryManager;
class Session;



class Buffer {


private:
    vk::Buffer buffer;
    size_t size;
 

friend class ck::MemoryManager;
friend class ck::Session;

};


} // namespace ck

#endif /* BUFFER_HPP_ */
