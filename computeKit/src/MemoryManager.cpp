
#include <iostream>
#include <system_error>

#include "MemoryManager.hpp"

namespace ck {

using namespace std;

MemoryManager::MemoryManager():
    size(0),
    offset(0) {
}


MemoryManager::MemoryManager(vk::PhysicalDevice& physicalDevice,
    vk::Device& device, const vk::MemoryPropertyFlags flags,
    const size_t size) :

    memoryFlags(flags),
    device(device),
    size(size),
    offset(0) {


    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

    // search for a memory type that has all the memoryFlags bits
    int typeIndex = -1;
    for(int i = 0; i <  memProperties.memoryTypeCount; i ++) {

        vk::MemoryType type = memProperties.memoryTypes[i];
        vk::MemoryPropertyFlags typeFlags = type.propertyFlags;

        // check flags
        if(memoryFlags == typeFlags) {

            // check that there is enough memory in the heap
            vk::MemoryHeap heap = memProperties.memoryHeaps[type.heapIndex];
            if(heap.size > size) {
                typeIndex = i;
                break;
            }
        }
    }


    if(typeIndex < 0) {
        throw system_error(std::error_code(), "No compatible memory type found.");
    }

    vk::MemoryAllocateInfo allocateInfo = vk::MemoryAllocateInfo()
        .setAllocationSize(size)
        .setMemoryTypeIndex(typeIndex);


    // this throws systerm_error internally
    memory = device.allocateMemory(allocateInfo);
}


void MemoryManager::free() {

    device.freeMemory(memory);
}


size_t MemoryManager::getSize() const {
    return size;
}

vk::MemoryPropertyFlags MemoryManager::getMemoryFlags() const {
    return memoryFlags;
}


void MemoryManager::bindBuffer(const ck::Buffer& buffer) {

    device.bindBufferMemory(buffer.buffer, memory, offset);
    offset += buffer.size;
}

} // namespace ck