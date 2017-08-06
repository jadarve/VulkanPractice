#include "ck/Program.hpp"

#include <fstream>

namespace ck {

using namespace std;

Program::Program() {
    // nothing to do
}


Program::Program(vk::Device& device, const std::string& filepath):
    device {device} {

    // TODO: check if file exists
    ifstream file(filepath, std::ios::ate | std::ios::binary);
    size_t fileSize = (size_t) file.tellg();

    // read shader file
    std::vector<char> binCode(fileSize);
    file.seekg(0);
    file.read(binCode.data(), fileSize);
    file.close();


    vk::ShaderModuleCreateInfo moduleCreateInfo = vk::ShaderModuleCreateInfo()
        .setCodeSize(binCode.size())
        .setPCode(reinterpret_cast<const uint32_t*>(binCode.data()));

    // TODO: Check exception behavior
    module = device.createShaderModule(moduleCreateInfo);

    referenceCounter = std::make_shared<int>(0);
}


Program::~Program() {

    if(referenceCounter.use_count() == 1) {
        device.destroyShaderModule(module, nullptr);
    }
}


vk::ShaderModule Program::getShaderModule() const {
    return module;
}


} // namespace ck