
#include <iostream>

#include "loader.hpp"

using namespace std;

int main() {
	cout << "STARTING" << endl;

	VulkanLibrary vk {};
	if(vk.load()) {
		cout << "Vulkan loaded successfully" << endl;
	} else {
		cout << "ERROR: error loading Vulkan library: " << vk.getErrorMessage() << endl;
	}

	vk.initGlobalFunctions();

	cout << "FINISH" << endl;
}