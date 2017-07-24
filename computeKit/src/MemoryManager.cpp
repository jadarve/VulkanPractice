
#include <iostream>
#include <system_error>

#include "ck/MemoryManager.hpp"

namespace ck {

using namespace std;

bool containsMemoryFlags(const vk::MemoryPropertyFlags& flags1, const vk::MemoryPropertyFlags& flags2) {
    return (flags1 & flags2) != vk::MemoryPropertyFlags();
}

void printMemoryFlags(const vk::MemoryPropertyFlags& flags, const string& prefix,
    const string& tabs) {

    cout << prefix << endl;
    cout << tabs << "device local:\t" << ((flags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal) << endl;
        cout << tabs << "host visible:\t" << ((flags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible) << endl;
        cout << tabs << "host coherent:\t" << ((flags & vk::MemoryPropertyFlagBits::eHostCoherent) == vk::MemoryPropertyFlagBits::eHostCoherent) << endl;
        cout << tabs << "host cached:\t" << ((flags & vk::MemoryPropertyFlagBits::eHostCached) == vk::MemoryPropertyFlagBits::eHostCached) << endl << endl;
}


MemoryManager::MemoryManager():
    size(0),
    offset(0) {
}


MemoryManager::MemoryManager(vk::PhysicalDevice& physicalDevice,
    vk::Device& device, const vk::MemoryPropertyFlags flags,
    const size_t size) :

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
        if(containsMemoryFlags(flags, typeFlags)) {

            // check that there is enough memory in the heap
            vk::MemoryHeap heap = memProperties.memoryHeaps[type.heapIndex];
            if(heap.size > size) {
                // this manager will support all the flags of typeFlags
                memoryFlags = typeFlags;
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