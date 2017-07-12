
#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>


using namespace std;


// Forward declarations
vk::Instance createInstance();
// vk::PhysicalDevice getComputeDevice(vk::Instance& instance);

void printMemoryProperties(const vk::PhysicalDeviceMemoryProperties& memProperties);

uint32_t getComputeQueueIndex(vk::PhysicalDevice& device);

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

    uint32_t queueIndex = getComputeQueueIndex(physicalDevice);

    const float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo devQueueCreateInfo = vk::DeviceQueueCreateInfo()
        .setQueueCount(1)
        .setQueueFamilyIndex(queueIndex)
        .setPQueuePriorities(&queuePriority);


    vk::DeviceCreateInfo devCreateInfo = vk::DeviceCreateInfo()
            .setQueueCreateInfoCount(1)
            .setPQueueCreateInfos(&devQueueCreateInfo);

    vk::Device device = physicalDevice.createDevice(devCreateInfo);
    cout << "Device created" << endl;

    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();
    printMemoryProperties(memProperties);

    return EXIT_SUCCESS;
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

        cout << "queue: " << (queueIndex++) << endl;

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