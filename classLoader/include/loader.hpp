
#ifndef LOADER_H_
#define LOADER_H_

#include <string>
#include <functional>
#include <memory>

#include <dlfcn.h>

#include <vulkan/vulkan.h>

class Vk_Library;
class Vk_Instance;


typedef std::shared_ptr<Vk_Library> Vk_Library_p;


class Vk_Library {


public:
    /**
     * \brief returns Vulkan library instance
     * \throws std::runtime_error if loading Vulkan failed
     */
    static std::shared_ptr<Vk_Library> get();


    std::string getExtensionPropertiesString() const;
    std::string getLayerPropertiesString() const;
    

    std::shared_ptr<Vk_Instance> createInstance();

protected:
    Vk_Library();


private:
    // Singleton variable
    static std::shared_ptr<Vk_Library> _instance;

    template<class T>
    std::function<T> loadFunction(const std::string& functionName) const;

    template<class T>
    std::function<T> loadInstanceFunction(const std::string& functionName, VkInstance instance=nullptr) const;

    void initInstanceFunctions();

    // FIXME: add Windows compatibility
    std::shared_ptr<void> _library;


    // Global function
    std::function<PFN_vkVoidFunction(VkInstance, const char*)> _vkGetInstanceProcAddr;

    // Instance level commands
    std::function<VkResult(const VkInstanceCreateInfo*, const VkAllocationCallbacks*, VkInstance*)> _vkCreateInstance;
    std::function<VkResult(const char*, uint32_t*, VkExtensionProperties*)> _vkEnumerateInstanceExtensionProperties;
    std::function<VkResult(uint32_t*, VkLayerProperties*)> _vkEnumerateInstanceLayerProperties;

// Friendship
friend class Vk_Instance;
};





class Vk_Instance {

public:
    Vk_Instance();
    ~Vk_Instance();

private:

	std::shared_ptr<int> _refCounter;
	VkInstance _instance;

    // from documentation, vkDestroyInstance needs to be loaded specifically for each instance.
    std::function<void(VkInstance, const VkAllocationCallbacks*)> _vkDestroyInstance;
};


// include template implementations
#include "loader_impl.hpp"

#endif