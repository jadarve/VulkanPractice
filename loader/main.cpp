#include <iostream>
#include <dlfcn.h>

#include <vulkan/vulkan.h>

using namespace std;

int main() {

    cout << "loading vulkan library" << endl;

    void* vulkanLibrary = dlopen("libvulkan.so", RTLD_NOW);
    if(vulkanLibrary == nullptr) {
        cout << "error loading vulkan: " << dlerror() << endl;
        return -1;
    }

    cout << "load successful :D" << endl;
}