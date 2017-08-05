#include "ck/Session.hpp"

#include <iostream>
#include <fstream>
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

    // destroy all buffers
    for(auto buffer : buffers) {
        device.destroyBuffer(buffer.buffer);
    }

    for(auto memManager : memories) {
        memManager.free();
    }

    device.destroy();
    instance.destroy();
}


uint32_t Session::getComputeFamilyQueueIndex() {
    return computeQueueFamilyIndex;
}


std::vector<vk::MemoryPropertyFlags> Session::getMemoryProperties() const {

    vector<vk::MemoryPropertyFlags> memTypes;

    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();
    for(int i = 0; i < memProperties.memoryTypeCount; i ++) {

        vk::MemoryPropertyFlags flags = memProperties.memoryTypes[i].propertyFlags;

        // filter out flags with all bits set to 0
        if(flags == vk::MemoryPropertyFlags()) continue;

        // insert flags if it is not already memTypes
        bool found = false;
        for(auto f : memTypes) {
            if(f == flags) {
                found = true;
                break;
            }
        }

        if(!found) {
            memTypes.push_back(flags);
        }
    }

    return memTypes;
}


void Session::allocateMemory(const vk::MemoryPropertyFlags flags, const size_t size) {

    cout << "Session::allocateMemory(): requested flags:"  << endl;
    printMemoryFlags(flags, "");
    // if there is already a memory with the same flags, do nothing
    for(auto memManager : memories) {
        printMemoryFlags(memManager.getMemoryFlags(), "memory manager");
        if(containsMemoryFlags(flags, memManager.getMemoryFlags())) {
            cout << "Session::allocateMemory(): compatible memory already allocated."  << endl;
            return;
        }
    }

    cout << "Session::allocateMemory(): memory size (bytes): " << size << endl;;
    MemoryManager manager(physicalDevice, device, flags, size);
    memories.push_back(manager);

    printMemoryFlags(manager.getMemoryFlags(), "Session::allocateMemory(): allocated memory flags");
}


ck::Buffer Session::createBuffer(const vk::MemoryPropertyFlags flags, const size_t size) {

    // find a memory manager with the same flags as the parameter
    // and try to allocate a buffer
    // Need to keep track of buffer destruction to liberate the memory
    // probably need to wrap vk::Buffer with something with a reference
    // counter

    vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
        .setSharingMode(vk::SharingMode::eExclusive)
        .setSize(size)
        .setUsage(vk::BufferUsageFlagBits::eStorageBuffer)
        .setQueueFamilyIndexCount(1)
        .setPQueueFamilyIndices(&computeQueueFamilyIndex);


    ck::Buffer buffer;

    buffer.buffer = device.createBuffer(bufferInfo);
    buffer.size = size;

    // bind the buffer to a memory with the same flags
    for(auto memManager : memories) {
        if(containsMemoryFlags(flags, memManager.getMemoryFlags())) {
            memManager.bindBuffer(buffer);
            break;
        }
    }

    buffers.push_back(buffer);
    return buffer;
}


vk::ShaderModule Session::createShaderModule(const std::string& filename) {

    ifstream file(filename, std::ios::ate | std::ios::binary);
    if(!file.is_open()) {
        cerr << "ERROR: error reading shader file: " << filename << endl;
        exit(EXIT_FAILURE);
    }

    size_t fileSize = (size_t) file.tellg();
    cout << "createShaderModule(): fileSize: " << fileSize << endl;

    // read shader file
    std::vector<char> v(fileSize);
    file.seekg(0);
    file.read(v.data(), fileSize);
    file.close();


    vk::ShaderModuleCreateInfo moduleCreateInfo = vk::ShaderModuleCreateInfo()
        .setCodeSize(v.size())
        .setPCode(reinterpret_cast<const uint32_t*>(v.data()));

    vk::ShaderModule module = device.createShaderModule(moduleCreateInfo);
    return module;
}

ck::Program Session::createProgram(const std::string& filepath) {

    Program p {device, filepath};
    return std::move(p);
}


// ck::Kernel Session::createKernel() {
//     cout << "Session::createKernel()" << endl;
//     return std::move(Kernel(device));
// }

ck::Node Session::createNode(ck::Kernel& k) {
    Node node{k};
    return std::move(node);
}


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