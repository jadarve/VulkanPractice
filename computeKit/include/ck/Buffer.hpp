#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <vector>
#include <memory>

#include <vulkan/vulkan.hpp>

// #include "ck/MemoryManager.hpp"

// compute kit
namespace ck {

// forward declarations
class Node;
class MemoryManager;
class Session;



class Buffer {

public:
    void* map();
    void unmap();

private:
    vk::Buffer buffer;
    size_t size {0};
    size_t offset {0};

    // FIXME: ugly
    ck::MemoryManager* memoryManager {nullptr};
 

friend class ck::MemoryManager;
friend class ck::Session;
friend class ck::Node;

};


} // namespace ck

#endif /* BUFFER_HPP_ */
