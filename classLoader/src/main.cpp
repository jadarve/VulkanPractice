
#include <iostream>
#include <memory>

#include "vulkanPractice.hpp"

using namespace std;

int main() {
	cout << "STARTING" << endl;

	// VulkanLibrary vk {};
	// if(vk.load()) {
	// 	cout << "Vulkan loaded successfully" << endl;
	// } else {
	// 	cout << "ERROR: error loading Vulkan library: " << vk.getErrorMessage() << endl;
	// }

	// vk.initGlobalFunctions();
	// vk.printInstanceExtensionProperties();
	// vk.printInstanceLayerProperties();

	

	auto lib = Vk_Library::get();

	cout << lib->getExtensionPropertiesString() << endl;
	cout << lib->getLayerPropertiesString() << endl;


	auto instance = lib->createInstance();
	std::cout << "Physical device count: " << instance->getDeviceCount() << std::endl;

	std::cout << "Device properties:" << std::endl;
	std::cout << instance->getPhysicalDevicePropertiesString(0) << std::endl;

	std::cout << "Device queue family properties" << std::endl;
	std::cout << instance->getPhysicalDeviceQueueFamilyPropertiesString(0) << std::endl;

	// or should I go directly to create queues?
	// auto dev = instance->getDevice("gpu-0");
	// auto queue = instance->createQueue("gpu-0", COMPUTE);
	// Vk_Device dev = instance.getDevice(...);
	// Vk_queue queue = dev.createQueue(...);

	cout << "FINISH" << endl;
}
