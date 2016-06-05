
#include <iostream>
#include <vector>
#include <dlfcn.h>

#include "loader.hpp"

using namespace std;


VulkanLibrary::~VulkanLibrary() {

	if(_library != nullptr) {

		// dlclose does not work with nullptr value
		if(dlclose(_library)) {
			string errMsg(dlerror());
			cerr << "ERROR: VulkanLibrary::~VulkanLibrary(): error closing Vulkan library: " << 
				errMsg << endl;
		}
	}
}

bool VulkanLibrary::load() {

	// FROM DOCUMENTATION: http://pubs.opengroup.org/onlinepubs/7908799/xsh/dlopen.html
	// Only a single copy of an object file is brought
	// into the address space, even if dlopen() is invoked
	//multiple times in reference to the file, and even if
	//different pathnames are used to reference the file. 
	_library = dlopen("libvulkan.so", RTLD_NOW);
    return _library != nullptr;
}

void VulkanLibrary::initGlobalFunctions() {

	_vkGetInstanceProcAddr = loadFunction<	PFN_vkVoidFunction(VkInstance,
											const char*)>("vkGetInstanceProcAddr");

	// load using _vkGetInstanceProcAddr
	_vkCreateInstance = loadInstanceFunction<VkResult(const VkInstanceCreateInfo*,
		const VkAllocationCallbacks*, VkInstance*) >("vkCreateInstance");

	_vkEnumerateInstanceExtensionProperties = loadInstanceFunction<VkResult(const char*,
		uint32_t*, VkExtensionProperties*)>("vkEnumerateInstanceExtensionProperties");

	_vkEnumerateInstanceLayerProperties = loadInstanceFunction<
		VkResult(uint32_t*, VkLayerProperties*)>("vkEnumerateInstanceLayerProperties");
}

string VulkanLibrary::getErrorMessage() {
	return string(dlerror());
}

void VulkanLibrary::printInstanceExtensionProperties() {

	uint32_t extensionCount = 0;
	_vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	cout << "Instance extension count: " << extensionCount << endl;

	if(extensionCount > 0) {

		vector<VkExtensionProperties> properties {extensionCount};
		_vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, &properties[0]);

		// iterates the instances
		for(auto p : properties) {
			cout << "extension name: " << p.extensionName << endl;
			cout << "\tspecVersion: " << p.specVersion << endl;
		}

	}
}

void VulkanLibrary::printInstanceLayerProperties() {

	uint32_t layerCount = 0;
	_vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	cout << "Layers found: " << layerCount << endl;

	if(layerCount > 0) {

		vector<VkLayerProperties> layers {layerCount};
		_vkEnumerateInstanceLayerProperties(&layerCount, &layers[0]);

		for(auto p : layers) {
			cout << "Layer: " << p.layerName << endl;
			cout << "\tspecVersion: " << p.specVersion << endl;
			cout << "\timplementationVersion: " << p.implementationVersion << endl;
			cout << "\tdescription: " << p.description << endl;
		}
	}
}
