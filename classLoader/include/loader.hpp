
#ifndef LOADER_H_
#define LOADER_H_

#include <string>
#include <functional>

#include <dlfcn.h>

#include <vulkan/vulkan.h>

// THINK: Should this class be a Singleton?
class VulkanLibrary {

public:

	~VulkanLibrary();

	bool load();
	void initGlobalFunctions();
	std::string getErrorMessage();

	template<class T>
    std::function<T> loadFunction(const std::string& functionName) const {

        dlerror();
        void* result = dlsym(_library, functionName.c_str());
        if (!result) {
            char* error = dlerror();
            if (error) {
                throw std::logic_error("can't find symbol named \"" + functionName + "\": " + error);
            }
        }

        return reinterpret_cast<T*>(result);
    }

private:
	// FIXME: need to enclose _library in a shared_ptr to support
	// copy/move semantics. Otherwise the destructor will call dlclose
	// before time.
	void* _library {nullptr};


	std::function<VkResult(	const VkInstanceCreateInfo*,
    						const VkAllocationCallbacks*,
    						VkInstance*)> _vkCreateInstance;
};


#endif