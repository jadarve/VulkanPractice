
#include <iostream>
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

	_vkCreateInstance = loadFunction< VkResult(	const VkInstanceCreateInfo*,
					    						const VkAllocationCallbacks*,
					    						VkInstance*) >("vkCreateInstance");

}

string VulkanLibrary::getErrorMessage() {
	return string(dlerror());
}