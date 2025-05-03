#pragma once

#include <vulkan/vulkan.h>

#include "Common.h"

class GPU;

class Fence {
public:
    Fence(Handle<GPU> gpu, bool signaled = false);
    ~Fence();

    void wait(uint64_t timeout = UINT64_MAX) const;
    void reset() const;

    operator VkFence() const { return mFence; }

private:
    Handle<GPU> pGPU;
    VkFence mFence;
};
