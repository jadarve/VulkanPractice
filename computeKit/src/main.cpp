
#include <iostream>
#include <thread>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "ck/Session.hpp"
#include "ck/Buffer.hpp"
#include "ck/Program.hpp"
#include "ck/KernelDescriptor.hpp"
#include "ck/Kernel.hpp"
#include "ck/Node.hpp"

using namespace std;

// forward declarations
void printSessionMemoryFlags(const ck::Session& session);


int main() {

    cout << "START" << endl;

    ck::Session session;
    printSessionMemoryFlags(session);

    vk::MemoryPropertyFlags memDevLocal = vk::MemoryPropertyFlagBits::eDeviceLocal;
    vk::MemoryPropertyFlags memHostVisible = vk::MemoryPropertyFlagBits::eHostVisible |
                            vk::MemoryPropertyFlagBits::eHostCoherent;

    // allocate memories
    session.allocateMemory(memDevLocal, 32*1024*1024);
    session.allocateMemory(memHostVisible, 32*1024*1024);

    // create a buffer
    ck::Buffer buffer0 = session.createBuffer(memHostVisible, 1024);

    // TO BUILD A COMPUTE KERNEL
    //
    // * Build the shader module, bindings, stage info
    // * Create a DescriptorSetLayout object.
    // * Build the compute pipeline with pipeline layout.
    // * Create a descriptor pool and descriptor set to bind buffers to the shader.
    // * Set local and global sizes.

    // one program object to hold the SPIR-V code of many potential kernels
    ck::Program program = session.createProgram("/home/jadarve/git/VulkanPractice/shaders/comp.spv");

    ck::KernelDescriptor desc = ck::KernelDescriptor()
        .setFunctionName("main")
        .addBufferParameter();

    // THINK: Should the kernel manage the descriptor pool and descriptor sets?
    ck::Kernel kernel = program.buildKernel(desc);

    // // create a compute node to run the kernel
    // ck::Node node = session.createNode(kernel);



    // TO RUN THE KERNEL
    // * Create a command pool and command buffer.
    // * Record compute pipeline dispatch

    // std::this_thread::sleep_for (std::chrono::seconds(5));

    cout << "FINISH" << endl;
    return EXIT_SUCCESS;
}


void printSessionMemoryFlags(const ck::Session& session) {

    vector<vk::MemoryPropertyFlags> memFlags = session.getMemoryProperties();

    for(int i = 0; i < memFlags.size(); i ++) {

        auto flags = memFlags[i];

        cout << "Type: " << i << endl;
        cout << "\t\tdevice local:\t" << ((flags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal) << endl;
        cout << "\t\thost visible:\t" << ((flags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible) << endl;
        cout << "\t\thost coherent:\t" << ((flags & vk::MemoryPropertyFlagBits::eHostCoherent) == vk::MemoryPropertyFlagBits::eHostCoherent) << endl;
        cout << "\t\thost cached:\t" << ((flags & vk::MemoryPropertyFlagBits::eHostCached) == vk::MemoryPropertyFlagBits::eHostCached) << endl << endl;

    }
}