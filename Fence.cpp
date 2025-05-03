#include "Fence.h"

#include "GPU.h"
#include <stdexcept>

Fence::Fence(Handle<GPU> gpu, bool signaled): pGPU(gpu) {
    VkFenceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    if (vkCreateFence(*pGPU, &createInfo, nullptr, &mFence) != VK_SUCCESS)
        throw std::runtime_error("failed to create mFence!");
}

Fence::~Fence() {
    if (mFence != VK_NULL_HANDLE)
        vkDestroyFence(*pGPU, mFence, nullptr);
}

void Fence::wait(uint64_t timeout) const {
    vkWaitForFences(*pGPU, 1, &mFence, VK_TRUE, timeout);
}

void Fence::reset() const {
    vkResetFences(*pGPU, 1, &mFence);
}

