
#include <iostream>
#include <memory>

#include "loader.hpp"

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
	// Vk_Device dev = instance.getDevice(...);
	// Vk_queue queue = dev.createQueue(...);

	cout << "FINISH" << endl;
}
