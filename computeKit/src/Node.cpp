#include "ck/Node.hpp"

#include <iostream>

namespace ck {

using namespace std;

// Node::Node():
//     referenceCounter{nullptr} {
// }

Node::Node(ck::Kernel& kernel) {

    referenceCounter = std::make_shared<int>(0);
    device = kernel.device;

    vk::PipelineLayoutCreateInfo pipeLayoutInfo = vk::PipelineLayoutCreateInfo()
        .setSetLayoutCount(1)
        .setPSetLayouts(&kernel.layout);

    layout = device.createPipelineLayout(pipeLayoutInfo);


    // compute pipeline creation info. Use stageInfo (bytecode and entry function) and
    // layout (buffer binding)
    vk::ComputePipelineCreateInfo computePipeInfo = vk::ComputePipelineCreateInfo()
        .setStage(kernel.stageInfo)
        .setLayout(layout);

    // create the compute pipeline
    pipeline = device.createComputePipeline(nullptr, computePipeInfo);    
}

Node::~Node() {

    cout << "Node::~Node(): ref count: "
        << referenceCounter.use_count() << endl;

    if(referenceCounter.use_count() == 1) {
        cout << "Node::~Node(): destroying pipeline" << endl;
        device.destroyPipelineLayout(layout);
        device.destroyPipeline(pipeline);
    }
}

} // namespace ck