
#include "Kernel.hpp"

namespace ck {

Kernel& Kernel::addBufferParameter() {

	parameterBindings.push_back(
	{static_cast<uint32_t>(parameterBindings.size()), vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute, nullptr});

	return *this;
}

} // namespace ck