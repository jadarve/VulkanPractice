#include "Session.hpp"

#include <iostream>
#include <system_error>
#include <vector>

namespace ck {

using namespace std;


Session::Session() {

    cout << "Session::Session()" << endl;

    createInstance();

    // get the first physical device available
    physicalDevice = instance.enumeratePhysicalDevices()[0];

    computeQueueFamilyIndex = getComputeFamilyQueueIndex();
    createDevice();
}


Session::~Session() {

    cout << "Session::~Session()" << endl;

    device.destroy();
    instance.destroy();
}


uint32_t Session::getComputeFamilyQueueIndex() {
    return computeQueueFamilyIndex;
}

vk::Buffer createBuffer(const vk::MemoryPropertyFlags& flags, const size_t size) {

    // find a memory manager with the same flags as the parameter
    // and try to allocate a buffer
    // Need to keep track of buffer destruction to liberate the memory
    // probably need to wrap vk::Buffer with something with a reference
    // counter

    return vk::Buffer();
}

// size_t Session::allocateDeviceMemory(const vk::MemoryPropertyFlags& flags, size_t size) {

//     vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

    
//     // TODO check size alignment
//     size_t sizeAligned = size;

//     // search for a memory type that checks all the type flags
//     int typeIndex = -1;
//     for(int i = 0; i <  memProperties.memoryTypeCount; i ++) {

//         vk::MemoryType type = memProperties.memoryTypes[i];
//         vk::MemoryPropertyFlags tflags = type.propertyFlags;

//         // check flags
//         if(flags == tflags) {

//             // check that there is enough memory in the heap
//             vk::MemoryHeap heap = memProperties.memoryHeaps[type.heapIndex];
//             if(heap.size > size) {
//                 typeIndex = i;
//                 break;
//             }
//         }
//     }

//     if(typeIndex < 0) {
//         throw system_error(error_code(), "Unable to find a memory type that supports all flags.");
//     }


//     vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
//         .setAllocationSize(sizeAligned)
//         .setMemoryTypeIndex(typeIndex);

//     // allocate device memory to store the buffer data
//     memory = device.allocateMemory(allocInfo);
//     memorySize = sizeAligned;

//     return sizeAligned;
// }


void Session::createInstance() {

    const vk::ApplicationInfo appInfo = vk::ApplicationInfo()
            .setPApplicationName("computeKit")
            .setApplicationVersion(0)
            .setEngineVersion(0)
            .setPEngineName("none")
            .setApiVersion(VK_MAKE_VERSION(1, 0, 46));

    const vk::InstanceCreateInfo instanceInfo = vk::InstanceCreateInfo()
            .setPApplicationInfo(&appInfo);

    vk::Result result = vk::createInstance(&instanceInfo, nullptr, &instance);

    if(result == vk::Result::eErrorIncompatibleDriver) {
        throw system_error(std::error_code(), "Incompatible driver");
    }
}


void Session::createDevice() {

    const float queuePriority = 1.0f;

    vk::DeviceQueueCreateInfo devQueueCreateInfo = vk::DeviceQueueCreateInfo()
        .setQueueCount(1)
        .setQueueFamilyIndex(computeQueueFamilyIndex)
        .setPQueuePriorities(&queuePriority);

    vk::DeviceCreateInfo devCreateInfo = vk::DeviceCreateInfo()
            .setQueueCreateInfoCount(1)
            .setPQueueCreateInfos(&devQueueCreateInfo);

    device = physicalDevice.createDevice(devCreateInfo);
}


uint32_t Session::getFamilyQueueIndex() {

    vector<vk::QueueFamilyProperties> queueProperties = physicalDevice.getQueueFamilyProperties();

    uint32_t queueIndex = 0;
    for(auto prop : queueProperties) {
        
        auto compute = ((prop.queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute);
        if(compute != 0) {
            return queueIndex;
        }

        queueIndex ++;
    }

    throw system_error(std::error_code(), "No compute capable queue family found.");
}

} // namespace ck