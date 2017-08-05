
#include "ck/Buffer.hpp"
#include "ck/MemoryManager.hpp"

#include <iostream>

namespace ck {

void* Buffer::map() {

    return this->memoryManager->mapBuffer(*this);
}


void Buffer::unmap() {
    this->memoryManager->unmap();
}


} // namespace ck