
#include <iostream>
#include <thread>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "Session.hpp"

using namespace std;

// forward declarations
void printMemoryFlags(const ck::Session& session);


int main() {

    cout << "START" << endl;

    ck::Session session;
    printMemoryFlags(session);
    session.allocateMemory(vk::MemoryPropertyFlagBits::eDeviceLocal, 1024*1024*1024);

    std::this_thread::sleep_for (std::chrono::seconds(5));

    cout << "FINISH" << endl;
    return EXIT_SUCCESS;
}


void printMemoryFlags(const ck::Session& session) {

    vector<vk::MemoryPropertyFlags> memFlags = session.getMemoryProperties();

    for(int i = 0; i < memFlags.size(); i ++) {

        auto flags = memFlags[i];

        cout << "Type: " << i << endl;
        cout << "\t\tdevice local:\t" << ((flags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal) << endl;
        cout << "\t\thost visible:\t" << ((flags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible) << endl;
        cout << "\t\thost coherent:\t" << ((flags & vk::MemoryPropertyFlagBits::eHostCoherent) == vk::MemoryPropertyFlagBits::eHostCoherent) << endl;
        cout << "\t\thost cached:\t" << ((flags & vk::MemoryPropertyFlagBits::eHostCached) == vk::MemoryPropertyFlagBits::eHostCached) << endl << endl;

    }
}