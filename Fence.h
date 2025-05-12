#pragma once

#include <vulkan/vulkan.h>

#include "Common.h"
#include <iostream>

class Fence {

public:

    Fence(VkFence fence = VK_NULL_HANDLE, VkDevice gpu = VK_NULL_HANDLE): mFence(fence), mGPU(gpu) { }
	Fence(const Fence&) = delete;
	Fence(Fence&& other) noexcept: mFence(other.mFence), mGPU(other.mGPU) {
		other.mFence = VK_NULL_HANDLE;
	}
	Fence& operator=(const Fence&) = delete;
	Fence& operator=(Fence&& other) noexcept {
		if (this != &other) {
			if (mFence)
				vkDestroyFence(mGPU, mFence, nullptr);

			mFence = other.mFence;
			mGPU = other.mGPU;
			other.mFence = VK_NULL_HANDLE;
		}
		return *this;
	}
    ~Fence() noexcept {
		if (mFence == VK_NULL_HANDLE) 
			return;

		vkDestroyFence(mGPU, mFence, nullptr);
	}


	void wait(uint64_t timeout = UINT64_MAX) const;
    void reset() const;


    operator VkFence() const { return mFence; }

private:
   
    VkFence mFence;
	VkDevice mGPU;

};
