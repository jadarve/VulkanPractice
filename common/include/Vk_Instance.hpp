
#ifndef VK_INSTANCE_H_
#define VK_INSTANCE_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "Vk_Device.hpp"

class Vk_Instance {

public:
    Vk_Instance();
    ~Vk_Instance();


public:
	int getDeviceCount() const;

	std::string getPhysicalDevicePropertiesString(const int devIndex) const;
	std::string getPhysicalDeviceQueueFamilyPropertiesString(const int devIndex) const;

	std::shared_ptr<Vk_Device> createDevice(const int physicalDeviceIndex);

private:
	void loadFunctions();
	void loadPhysicalDevices();

private:

	std::shared_ptr<int> _refCounter;
	VkInstance _instance;

	std::vector<VkPhysicalDevice> _physicalDevices;

    // from documentation, vkDestroyInstance needs to be loaded specifically for each instance.
    std::function<void(VkInstance, const VkAllocationCallbacks*)> _vkDestroyInstance;

    // Physical device functions
    std::function<VkResult(VkInstance, uint32_t*, VkPhysicalDevice*)> _vkEnumeratePhysicalDevices;
    std::function<void(VkPhysicalDevice, VkPhysicalDeviceProperties*)> _vkGetPhysicalDeviceproperties;
    std::function<void(VkPhysicalDevice, uint32_t*, VkQueueFamilyProperties*)> _vkGetPhysicalDeviceQueueFamilyProperties;
    std::function<VkResult(VkPhysicalDevice, const VkDeviceCreateInfo*, const VkAllocationCallbacks*, VkDevice*)> _vkCreateDevice;
    std::function<void(VkDevice, const char*)> _vkGetDeviceProcAddr;
};

#endif