#ifndef LOADER_IMPL_H_
#define LOADER_IMPL_H_


template<class T>
std::function<T> Vk_Library::loadFunction(const std::string& functionName) const {

    // clear dlerror
    dlerror();
    
    void* result = dlsym(_library.get(), functionName.c_str());
    if (!result) {
        char* error = dlerror();
        if (error) {
            throw std::runtime_error("cannot find symbol name: \"" + functionName + "\": " + error);
        }
    }

    return reinterpret_cast<T*>(result);
}


template<class T>
std::function<T> Vk_Library::loadInstanceFunction(const std::string& functionName,
    VkInstance instance) const {

    auto functionPtr = _vkGetInstanceProcAddr(instance, functionName.c_str());
    if(functionPtr == nullptr) {
        throw std::runtime_error("Error loading Vulkan instance function: \"" + functionName);
    }
    return reinterpret_cast<T*>(functionPtr);
}

#endif