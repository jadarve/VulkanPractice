
#include <iostream>
#include <fstream>
#include <vector>
#include <vulkan/vulkan.hpp>


using namespace std;


typedef struct {
    vk::ShaderModule module;
    vk::PipelineShaderStageCreateInfo stageInfo;
    vk::DescriptorSetLayout layout;

    string funcname;
} kernel;


typedef struct {
    vk::Pipeline pipeline;
    vk::PipelineLayout layout;
} computePipeline;


// Forward declarations
vk::Instance createInstance();
// vk::PhysicalDevice getComputeDevice(vk::Instance& instance);

void printMemoryProperties(const vk::PhysicalDeviceMemoryProperties& memProperties);

uint32_t getComputeQueueIndex(vk::PhysicalDevice& device);

vk::ShaderModule createShaderModule(vk::Device& device, const string& filename);
// vk::Pipeline createComputePipeline(vk::Device& device, const string& filename, const string& funcname);
vk::CommandPool createCommandPool(vk::Device& device, const uint32_t queueFamilyIndex);
vk::CommandBuffer createCommandBuffer(vk::Device& device, vk::CommandPool& commandPool);
vk::DescriptorPool createDescriptorPool(vk::Device& device);

kernel createKernel(vk::Device& device, const string& filename, const string& funcname);
computePipeline createComputePipeline(vk::Device& device, kernel& k);
// vk::Pipeline createComputePipeline(vk::Device& device, kernel& k);


int main() {

    vk::Instance instance = createInstance();


    // get the first physical device available
    vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
    vk::PhysicalDevice physicalDevice = physicalDevices[0];
    vk::PhysicalDeviceProperties deviceProperties;
    vk::PhysicalDeviceFeatures deviceFeatures;
    physicalDevice.getProperties(&deviceProperties);
    physicalDevice.getFeatures(&deviceFeatures);

    cout << "device:\t\t\t" << deviceProperties.deviceName << endl;
    cout << "device ID:\t\t" << deviceProperties.deviceID << endl;
    cout << "API version:\t\t" << deviceProperties.apiVersion << endl;
    cout << "device type:\t\t" << (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) << endl;


    // get a queue index that has the compute bit set to 1
    uint32_t queueFamilyIndex = getComputeQueueIndex(physicalDevice);

    const float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo devQueueCreateInfo = vk::DeviceQueueCreateInfo()
        .setQueueCount(1)
        .setQueueFamilyIndex(queueFamilyIndex)
        .setPQueuePriorities(&queuePriority);

    std::vector<uint32_t> queueFamilies {queueFamilyIndex};


    vk::DeviceCreateInfo devCreateInfo = vk::DeviceCreateInfo()
            .setQueueCreateInfoCount(1)
            .setPQueueCreateInfos(&devQueueCreateInfo);

    vk::Device device = physicalDevice.createDevice(devCreateInfo);
    cout << "Device created" << endl;


    /////////////////////////
    // MEMORY ALLOCATION
    /////////////////////////

    // query memory heaps and types
    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();
    printMemoryProperties(memProperties);

    size_t bufferLength = 64;
    size_t bufferSize = bufferLength*sizeof(float);

    vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
        .setAllocationSize(2*bufferSize)
        .setMemoryTypeIndex(9);

    // allocate device memory to store the buffer data
    vk::DeviceMemory devMemory = device.allocateMemory(allocInfo);


    vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
        .setSharingMode(vk::SharingMode::eExclusive)
        .setSize(bufferSize)
        .setUsage(vk::BufferUsageFlagBits::eStorageBuffer)
        .setQueueFamilyIndexCount(queueFamilies.size())
        .setPQueueFamilyIndices(queueFamilies.data());

    vk::Buffer buffer0 = device.createBuffer(bufferInfo);
    vk::Buffer buffer1 = device.createBuffer(bufferInfo);

    // bind buffers to device memory
    device.bindBufferMemory(buffer0, devMemory, 0);
    device.bindBufferMemory(buffer1, devMemory, bufferSize);


    float* bufferData = static_cast<float*>(device.mapMemory(devMemory, 0, bufferSize));
    for(int i = 0; i < bufferLength; i ++) {
        bufferData[i] = i;
    }
    device.unmapMemory(devMemory);

    // create a kernel structure holding shader code, entry point, layout and stage info
    kernel k = createKernel(device, "/home/jadarve/git/VulkanPractice/shaders/comp.spv", "main");

    // create a pipeline to run the kernel
    computePipeline pipeline = createComputePipeline(device, k);


    // Command buffer creation
    vk::CommandPool cmdPool = createCommandPool(device, queueFamilyIndex);
    vk::CommandBuffer cmdBuffer = createCommandBuffer(device, cmdPool);


    // Descriptor pool to get descriptor set to bind buffer to shader
    vk::DescriptorPool descriptorPool = createDescriptorPool(device);

    
    // descriptor set
    vk::DescriptorSetAllocateInfo descSetAllocInfo = vk::DescriptorSetAllocateInfo()
        .setDescriptorPool(descriptorPool)
        .setDescriptorSetCount(1)
        .setPSetLayouts(&k.layout);


    vk::DescriptorSet descriptorSet;
    device.allocateDescriptorSets(&descSetAllocInfo, &descriptorSet);

    vk::DescriptorBufferInfo descBufferInfo = vk::DescriptorBufferInfo()
        .setOffset(0)
        .setRange(VK_WHOLE_SIZE)
        .setBuffer(buffer0);


    vk::WriteDescriptorSet writeDescSet = vk::WriteDescriptorSet()
        .setDescriptorType(vk::DescriptorType::eStorageBuffer)
        .setDstSet(descriptorSet)
        .setDstBinding(0)           // parameter 0 in kernel
        .setDescriptorCount(1)
        .setPBufferInfo(&descBufferInfo);

    // update the informacion of the descriptor set
    device.updateDescriptorSets(1, &writeDescSet, 0, nullptr);



    // record command buffer
    vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    cmdBuffer.begin(beginInfo);

    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline.pipeline);
    cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline.layout, 0, 1, &descriptorSet, 0, nullptr);
    cmdBuffer.dispatch(1, 1, 1);

    cmdBuffer.end();


    // Submission queue
    vk::Queue queue = device.getQueue(queueFamilyIndex, 0);

    // Here I can set semaphores to wait for and 
    // semaphores to trigger once execution is completed.
    vk::SubmitInfo submitInfo = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setPCommandBuffers(&cmdBuffer);

    queue.submit(1, &submitInfo, nullptr);
    queue.waitIdle();


    // map device memory to read data
    bufferData = static_cast<float*>(device.mapMemory(devMemory, 0, bufferSize));
    for(int i = 0; i < bufferLength; i ++) {
        cout << "[" << i << "]: " << bufferData[i] << endl;
        bufferData[i] = i;
    }
    device.unmapMemory(devMemory);



    // device.destroyDescriptorPool(descriptorPool);
    device.destroyPipeline(pipeline.pipeline);
    // TODO: should destroy shader module too

    // device.destroyCommandPool(cmdPool);
    
    // destroy the buffers
    device.destroyBuffer(buffer0);
    device.destroyBuffer(buffer1);

    // free device memory
    device.freeMemory(devMemory);

    cout << "FINISH" << endl;
    return EXIT_SUCCESS;
}


kernel createKernel(vk::Device& device, const string& filename, const string& funcname) {

    // read shader code in SPIR-V bytecode format
    vk::ShaderModule module = createShaderModule(device, filename);

    // create the shader stage info with the bytecode and entry point of the compute function
    vk::PipelineShaderStageCreateInfo stageInfo = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eCompute)
        .setModule(module)
        .setPName(funcname.c_str());

    // Bindings. It describes the argument of the shader so they can be assigned from the host.
    std::vector<vk::DescriptorSetLayoutBinding> bindings {
        {0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute, nullptr}
    };


    vk::DescriptorSetLayoutCreateInfo descLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
        .setBindingCount(bindings.size())
        .setPBindings(bindings.data());

    vk::DescriptorSetLayout descLayout = device.createDescriptorSetLayout(descLayoutInfo);


    kernel k;
    k.module = module;
    k.stageInfo = stageInfo;
    k.funcname = funcname;
    k.layout = descLayout;
    return k;
}

computePipeline createComputePipeline(vk::Device& device, kernel& k) {

    vk::PipelineLayoutCreateInfo pipeLayoutInfo = vk::PipelineLayoutCreateInfo()
        .setSetLayoutCount(1)
        .setPSetLayouts(&k.layout);

    vk::PipelineLayout pipeLayout = device.createPipelineLayout(pipeLayoutInfo);


    // compute pipeline creation info. Use stageInfo (bytecode and entry function) and
    // layout (buffer binding)
    vk::ComputePipelineCreateInfo computePipeInfo = vk::ComputePipelineCreateInfo()
        .setStage(k.stageInfo)
        .setLayout(pipeLayout);

    // create the compute pipeline
    vk::Pipeline pipeline = device.createComputePipeline(nullptr, computePipeInfo);
    
    computePipeline cpipe;
    cpipe.pipeline = pipeline;
    cpipe.layout = pipeLayout;
    return cpipe;
}


vk::DescriptorPool createDescriptorPool(vk::Device& device) {

    // only one parameter
    vector<vk::DescriptorPoolSize> poolSizes{
        {vk::DescriptorType::eStorageBuffer, 1}
    };

    vk::DescriptorPoolCreateInfo info = vk::DescriptorPoolCreateInfo()
        .setMaxSets(1)
        .setPoolSizeCount(1)
        .setPPoolSizes(poolSizes.data());
        

    vk::DescriptorPool descPool;
    device.createDescriptorPool(&info, nullptr, &descPool);
    return descPool;
}


vk::CommandPool createCommandPool(vk::Device& device, const uint32_t queueFamilyIndex) {

    vk::CommandPoolCreateInfo createInfo = vk::CommandPoolCreateInfo()
        .setQueueFamilyIndex(queueFamilyIndex);

    vk::CommandPool pool = device.createCommandPool(createInfo);
    return pool;
}

vk::CommandBuffer createCommandBuffer(vk::Device& device, vk::CommandPool& commandPool) {

    vk::CommandBufferAllocateInfo allocInfo = vk::CommandBufferAllocateInfo()
        .setCommandPool(commandPool)
        .setCommandBufferCount(1);

    vector<vk::CommandBuffer> cmdBuffers = device.allocateCommandBuffers(allocInfo);
    return cmdBuffers[0];
}


vk::ShaderModule createShaderModule(vk::Device& device, const string& filename) {

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



vk::Instance createInstance() {

    const vk::ApplicationInfo appInfo = vk::ApplicationInfo()
            .setPApplicationName("computeShader")
            .setApplicationVersion(0)
            .setEngineVersion(0)
            .setPEngineName("none")
            .setApiVersion(VK_MAKE_VERSION(1, 0, 46));

    const vk::InstanceCreateInfo instanceInfo = vk::InstanceCreateInfo()
            .setPApplicationInfo(&appInfo);


    vk::Instance instance;
    vk::Result result = vk::createInstance(&instanceInfo, nullptr, &instance);

    if(result == vk::Result::eErrorIncompatibleDriver) {
        cerr << "ERROR: createInstance(): incompatible driver" << endl;
        exit(-1);
    }

    return instance;
}

uint32_t getComputeQueueIndex(vk::PhysicalDevice& device) {

    vector<vk::QueueFamilyProperties> queueProperties = device.getQueueFamilyProperties();
    cout << "getComputeQueueIndex(): queue family count: " << queueProperties.size() << endl;

    uint32_t queueIndex = 0;

    for(auto prop : queueProperties) {

        cout << "queue: " << (queueIndex) << endl;

        auto graphics = ((prop.queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics);
        auto compute = ((prop.queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute);
        auto transfer = ((prop.queueFlags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer);
        auto sparse = ((prop.queueFlags & vk::QueueFlagBits::eSparseBinding) == vk::QueueFlagBits::eSparseBinding);

        cout << "\tgraphics: " << graphics << endl;
        cout << "\tcompute: " << compute << endl;
        cout << "\ttransfer: " << transfer << endl;
        cout << "\tsparse binding: " << sparse << endl << endl;

        if(compute != 0) {
            cout << "queue " << queueIndex << " selected." << endl;
            return queueIndex;
        }

        queueIndex ++;
    }

    cerr << "ERROR: getComputeQueueIndex(): no compute queue found." << endl;
    exit(EXIT_FAILURE);

    // unreachable
    return queueIndex;
}

void printMemoryProperties(const vk::PhysicalDeviceMemoryProperties& memProperties) {

    cout << "memProperties:\theap count: " << memProperties.memoryHeapCount << endl;
    cout << "memProperties:\ttype count: " << memProperties.memoryTypeCount << endl;


    for(int i = 0; i < memProperties.memoryHeapCount; i ++) {
        vk::MemoryHeap heap = memProperties.memoryHeaps[i];

        cout << "Heap: " << i << endl;
        cout << "\tsize:\t" << heap.size << endl;
        cout << "\tflags:" << endl;
        cout << "\t\tdevice local:\t" << ((heap.flags & vk::MemoryHeapFlagBits::eDeviceLocal) == vk::MemoryHeapFlagBits::eDeviceLocal) << endl;
        cout << "\t\tmulti instance:\t" << ((heap.flags & vk::MemoryHeapFlagBits::eMultiInstanceKHX) == vk::MemoryHeapFlagBits::eMultiInstanceKHX) << endl;
        cout << endl;
    }

    for(int i = 0; i <  memProperties.memoryTypeCount; i ++) {
        vk::MemoryType type = memProperties.memoryTypes[i];
        vk::MemoryPropertyFlags flags = type.propertyFlags;

        cout << "Type: " << i << endl;
        cout << "\theap index: " << type.heapIndex << endl;
        cout << "\tflags" << endl;
        cout << "\t\tdevice local:\t\t" <<  ((flags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal) << endl;
        cout << "\t\thost visible:\t\t" << ((flags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible) << endl;
        cout << "\t\thost coherent:\t\t" << ((flags & vk::MemoryPropertyFlagBits::eHostCoherent) == vk::MemoryPropertyFlagBits::eHostCoherent) << endl;
        cout << "\t\thost cached:\t\t" << ((flags & vk::MemoryPropertyFlagBits::eHostCached) == vk::MemoryPropertyFlagBits::eHostCached) << endl;
        cout << "\t\tlazily allocated:\t" << ((flags & vk::MemoryPropertyFlagBits::eLazilyAllocated) == vk::MemoryPropertyFlagBits::eLazilyAllocated) << endl;
        cout << endl;
    }
}


// vk::Pipeline createComputePipeline(vk::Device& device, kernel& k) {

//     vk::PipelineLayoutCreateInfo pipeLayoutInfo = vk::PipelineLayoutCreateInfo()
//         .setSetLayoutCount(1)
//         .setPSetLayouts(&k.layout);

//     vk::PipelineLayout pipeLayout = device.createPipelineLayout(pipeLayoutInfo);


//     // compute pipeline creation info. Use stageInfo (bytecode and entry function) and
//     // layout (buffer binding)
//     vk::ComputePipelineCreateInfo computePipeInfo = vk::ComputePipelineCreateInfo()
//         .setStage(k.stageInfo)
//         .setLayout(pipeLayout);

//     // create the compute pipeline
//     vk::Pipeline pipeline = device.createComputePipeline(nullptr, computePipeInfo);
//     return pipeline;
// }

// vk::Pipeline createComputePipeline(vk::Device& device, const string& filename, const string& funcname) {

//     // read shader code in SPIR-V bytecode format
//     vk::ShaderModule module = createShaderModule(device, filename);

//     // create the shader stage info with the bytecode and entry point of the compute function
//     vk::PipelineShaderStageCreateInfo stageInfo = vk::PipelineShaderStageCreateInfo()
//         .setStage(vk::ShaderStageFlagBits::eCompute)
//         .setModule(module)
//         .setPName(funcname.c_str());

//     // Bindings. It describes the argument of the shader so they can be assigned from the host.
//     std::vector<vk::DescriptorSetLayoutBinding> bindings {
//         {0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute, nullptr}
//     };


//     vk::DescriptorSetLayoutCreateInfo descLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
//         .setBindingCount(bindings.size())
//         .setPBindings(bindings.data());

//     vk::DescriptorSetLayout descLayout = device.createDescriptorSetLayout(descLayoutInfo);



//     vk::PipelineLayoutCreateInfo pipeLayoutInfo = vk::PipelineLayoutCreateInfo()
//         .setSetLayoutCount(1)
//         .setPSetLayouts(&descLayout);

//     vk::PipelineLayout pipeLayout = device.createPipelineLayout(pipeLayoutInfo);


//     // compute pipeline creation info. Use stageInfo (bytecode and entry function) and
//     // layout (buffer binding)
//     vk::ComputePipelineCreateInfo computePipeInfo = vk::ComputePipelineCreateInfo()
//         .setStage(stageInfo)
//         .setLayout(pipeLayout);

//     // create the compute pipeline
//     vk::Pipeline pipeline = device.createComputePipeline(nullptr, computePipeInfo);
//     return pipeline;
// }