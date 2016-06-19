
#include <iostream>
#include <sstream>
#include <vector>
#include <dlfcn.h>

#include "loader.hpp"

using namespace std;


//#########################################################
// Vk_Library
//#########################################################

// Singleton instance
std::shared_ptr<Vk_Library> Vk_Library::_instance = nullptr;


// can throw runtime_error
std::shared_ptr<Vk_Library> Vk_Library::get() {

	if(_instance == nullptr) {
		Vk_Library* aux = new Vk_Library();
		_instance = std::shared_ptr<Vk_Library>(aux);

		// I cannot use make_shared because the constructor is protected
		// _instance = std::make_shared<Vk_Library>();
	}

	return _instance;
}


// Protected constructor
Vk_Library::Vk_Library() {

	std::cout << "Vk_Library::Vk_Library(): creating instance" << std::endl;

	// FROM DOCUMENTATION: http://pubs.opengroup.org/onlinepubs/7908799/xsh/dlopen.html
	// Only a single copy of an object file is brought
	// into the address space, even if dlopen() is invoked
	// multiple times in reference to the file, and even if
	// different pathnames are used to reference the file. 
	void* libPtr = dlopen("libvulkan.so", RTLD_NOW);

	// error loading Vulkan
	if(libPtr == nullptr) {
		throw runtime_error{"Error loading Vulkan library: " + std::string{dlerror()}};
	}

	// when library is going to be deleted, it will call dlclose
	_library = std::shared_ptr<void>(libPtr,
		[](void* libPtr){dlclose(libPtr); std::cout << "Unloading Vullkan library..." << std::endl;});


	// load global function vkGetInstanceProcAddr
	_vkGetInstanceProcAddr = loadFunction<PFN_vkVoidFunction(VkInstance, const char*)>("vkGetInstanceProcAddr");

	// load instance functions
	initInstanceFunctions();
}


std::string Vk_Library::getExtensionPropertiesString() const {

	std::stringbuf buffer;
	std::ostream os{&buffer};

	uint32_t extensionCount = 0;

	if(_vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr) != VK_SUCCESS) {
		throw runtime_error{"Vk_Library::getExtensionPropertiesString(): error getting instance extension count"};
	}

	os << "Vulkan instance extension count: " << extensionCount << endl;

	if(extensionCount > 0) {

		vector<VkExtensionProperties> properties(extensionCount);
		_vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, &properties[0]);

		// iterates the instances
		for(auto p : properties) {
			os << p.extensionName << endl;
			os << "\tspecVersion: " << p.specVersion << endl;
		}
	}

	return buffer.str();
}


std::string Vk_Library::getLayerPropertiesString() const {

	std::stringbuf buffer;
	std::ostream os{&buffer};

	uint32_t layerCount = 0;
	if(_vkEnumerateInstanceLayerProperties(&layerCount, nullptr) != VK_SUCCESS) {
		throw runtime_error{std::string(__FUNCTION__) + std::string("(): error getting Vulkan instance layer count")};
	}

	os << "Vulkan instance layers count: " << layerCount << endl;

	if(layerCount > 0) {

		vector<VkLayerProperties> layers(layerCount);
		_vkEnumerateInstanceLayerProperties(&layerCount, &layers[0]);

		for(auto p : layers) {
			os << p.layerName << endl;
			os << "\tspecVersion: " << p.specVersion << endl;
			os << "\timplementationVersion: " << p.implementationVersion << endl;
			os << "\tdescription: " << p.description << endl;
		}
	}

	return buffer.str();
}

std::shared_ptr<Vk_Instance> Vk_Library::createInstance() {

	return std::make_shared<Vk_Instance>();
}

void Vk_Library::initInstanceFunctions() {

	_vkCreateInstance = loadInstanceFunction<VkResult(const VkInstanceCreateInfo*,
		const VkAllocationCallbacks*, VkInstance*) >("vkCreateInstance");

	_vkEnumerateInstanceExtensionProperties = loadInstanceFunction<VkResult(const char*,
		uint32_t*, VkExtensionProperties*)>("vkEnumerateInstanceExtensionProperties");

	_vkEnumerateInstanceLayerProperties = loadInstanceFunction<
		VkResult(uint32_t*, VkLayerProperties*)>("vkEnumerateInstanceLayerProperties");
}



//#########################################################
// Vk_Instance
//#########################################################

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

void Vk_Instance::loadFunctions() {

	auto vkLib = Vk_Library::get();
	_vkEnumeratePhysicalDevices = vkLib->loadInstanceFunction<VkResult(VkInstance, uint32_t*, VkPhysicalDevice*)>("vkEnumeratePhysicalDevices", _instance);
	_vkGetPhysicalDeviceproperties = vkLib->loadInstanceFunction<void(VkPhysicalDevice, VkPhysicalDeviceProperties*)>("vkGetPhysicalDeviceProperties", _instance);
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