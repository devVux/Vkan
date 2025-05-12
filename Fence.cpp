#include "Fence.h"

#include <cassert>
#include <iostream>


void Fence::wait(uint64_t timeout) const {
	assert(mGPU != VK_NULL_HANDLE && mFence != VK_NULL_HANDLE);
	vkWaitForFences(mGPU, 1, &mFence, VK_TRUE, timeout);
}

void Fence::reset() const {
	assert(mGPU != VK_NULL_HANDLE && mFence != VK_NULL_HANDLE);
	vkResetFences(mGPU, 1, &mFence);
}
