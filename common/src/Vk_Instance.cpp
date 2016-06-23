
#include <iostream>
#include <sstream>

#include "Vk_Instance.hpp"
#include "Vk_Library.hpp"

using namespace std;

Vk_Instance::Vk_Instance() {

	auto vkLib = Vk_Library::get();
	
	// Application info
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = "VulkanPractice";
	appInfo.applicationVersion = 0x01;
	appInfo.pEngineName = "Lluvia";
	appInfo.engineVersion = 0x01;
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// instance create info
	VkInstanceCreateInfo instInfo;
	instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instInfo.pNext = nullptr;
	instInfo.flags = 0;
	instInfo.pApplicationInfo = &appInfo;
	instInfo.enabledLayerCount = 0;
	instInfo.ppEnabledLayerNames = nullptr;
	instInfo.enabledExtensionCount = 0;
	instInfo.ppEnabledExtensionNames = nullptr;

	VkResult res = vkLib->_vkCreateInstance(&instInfo, nullptr, &_instance);
	if(res != VK_SUCCESS) {
		throw runtime_error("Error creating Vulkan instance");
	}

	// load _vkDestroyInstance
	_vkDestroyInstance = vkLib->loadInstanceFunction<void(VkInstance, const VkAllocationCallbacks*)>("vkDestroyInstance", _instance);

	// I cannot use a shared pointer to encapsulate _instance as the type of instance
	// is not fully known at compile time. It's a gray object managed by the GPU driver.
	// auto ptr = std::shared_ptr<VkInstance_T>(_instance);

	// use a shared_ptr as reference counter for this, so it can call
	// _vkDestroyInstance only once
	_refCounter = std::make_shared<int>(0);


	// load Instance level functions
	loadFunctions();
	loadPhysicalDevices();
}

Vk_Instance::~Vk_Instance() {
	std::cout << "Vk_Instance::~Vk_Instance()" << std::endl;
	
	if(_refCounter.unique()) {
		std::cout << "\tcalling _vkDestroyInstance()" << std::endl;
		_vkDestroyInstance(_instance, nullptr);
	}
}

int Vk_Instance::getDeviceCount() const {
	uint32_t devCount = 0;
	if(_vkEnumeratePhysicalDevices(_instance, &devCount, nullptr) != VK_SUCCESS) {
		throw runtime_error{std::string(__FUNCTION__) + std::string("(): error getting Vulkan physical device count")};
	}
	return devCount;
}

/*
typedef struct VkPhysicalDeviceProperties {
    uint32_t                            apiVersion;
    uint32_t                            driverVersion;
    uint32_t                            vendorID;
    uint32_t                            deviceID;
    VkPhysicalDeviceType                deviceType;
    char                                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
    uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
    VkPhysicalDeviceLimits              limits;
    VkPhysicalDeviceSparseProperties    sparseProperties;
} VkPhysicalDeviceProperties;

*/
std::string Vk_Instance::getPhysicalDevicePropertiesString(const int devIndex) const {

	std::stringbuf buffer;
	std::ostream os{&buffer};

	VkPhysicalDeviceProperties prop;
	_vkGetPhysicalDeviceproperties(_physicalDevices[devIndex], &prop);

	os << "API version: " << std::hex << prop.apiVersion << std::endl;
	os << "Driver version: " << std::hex << prop.driverVersion << std::endl;
	os << "Vendor ID: " << std::hex << prop.vendorID << std::endl;
	os << "Device ID: " << std::hex << prop.deviceID << std::endl;
	os << "Device type: " << prop.deviceType << std::endl;
	os << "Device name: " << prop.deviceName << std::endl;

	return buffer.str();
}

/*
typedef struct VkQueueFamilyProperties {
    VkQueueFlags    queueFlags;
    uint32_t        queueCount;
    uint32_t        timestampValidBits;
    VkExtent3D      minImageTransferGranularity;
} VkQueueFamilyProperties;
*/
std::string Vk_Instance::getPhysicalDeviceQueueFamilyPropertiesString(const int devIndex) const {

	std::stringbuf buffer;
	std::ostream os{&buffer};

	uint32_t count = 0;
	_vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevices[devIndex], &count, nullptr);

	os << "Physical device queue family count: " << count << std::endl;

	if(count != 0) {
		std::vector<VkQueueFamilyProperties> prop(count);
		_vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevices[devIndex], &count, &prop[0]);

		for(auto q : prop) {

			os << "\tflags: " << std::hex << q.queueFlags << std::endl;
			os << "\t\tGRAPHICS:\t" << (bool)(q.queueFlags & VK_QUEUE_GRAPHICS_BIT) << std::endl;
			os << "\t\tCOMPUTE:\t" << (bool)(q.queueFlags & VK_QUEUE_COMPUTE_BIT) << std::endl;
			os << "\t\tTRANSFER:\t" << (bool)(q.queueFlags & VK_QUEUE_TRANSFER_BIT) << std::endl;
			os << "\t\tSPARSE:\t\t" << (bool)(q.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) << std::endl;

			os << "\tqueue count: " << q.queueCount << std::endl;
			os << "\ttime stamp valid bits: " << std::hex << q.timestampValidBits << std::endl;
			os << "\tmin image transfer: [" << q.minImageTransferGranularity.width << ", "
				<< q.minImageTransferGranularity.height << ", " << q.minImageTransferGranularity.depth << "]" << std::endl;
		}
	}
	
	return buffer.str();
}

void Vk_Instance::loadFunctions() {

	auto vkLib = Vk_Library::get();
	_vkEnumeratePhysicalDevices = vkLib->loadInstanceFunction<VkResult(VkInstance, uint32_t*, VkPhysicalDevice*)>("vkEnumeratePhysicalDevices", _instance);
	_vkGetPhysicalDeviceproperties = vkLib->loadInstanceFunction<void(VkPhysicalDevice, VkPhysicalDeviceProperties*)>("vkGetPhysicalDeviceProperties", _instance);
	_vkGetPhysicalDeviceQueueFamilyProperties = vkLib->loadInstanceFunction<void(VkPhysicalDevice, uint32_t*, VkQueueFamilyProperties*)>("vkGetPhysicalDeviceQueueFamilyProperties", _instance);
}

void Vk_Instance::loadPhysicalDevices() {

	uint32_t devCount = getDeviceCount();
	_physicalDevices.resize(devCount);

	VkResult res = _vkEnumeratePhysicalDevices(_instance, &devCount, &_physicalDevices[0]);
	if(res != VK_SUCCESS) {
		throw runtime_error{std::string(__FUNCTION__) + std::string("(): error creating physical device list")};
	}

	// check that all devices were listed
	if(devCount != getDeviceCount()) {
		throw runtime_error{std::string(__FUNCTION__) + std::string("(): not all physical devices were created")};	
	}
}

