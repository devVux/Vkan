#pragma once

#include <vulkan/vulkan.h>
#include <iostream>

class Semaphore {

public:

	Semaphore(VkSemaphore semaphore = VK_NULL_HANDLE, VkDevice device = VK_NULL_HANDLE): mSemaphore(semaphore), mDevice(device) { }
	Semaphore(const Semaphore&) = delete;
	Semaphore(Semaphore&& other) noexcept: mSemaphore(other.mSemaphore), mDevice(other.mDevice) {
		other.mSemaphore = VK_NULL_HANDLE;
		other.mDevice = VK_NULL_HANDLE;
	}
	Semaphore& operator=(const Semaphore&) = delete;
	Semaphore& operator=(Semaphore&& other) noexcept {
		if (this != &other) {
			if (mSemaphore != VK_NULL_HANDLE)
				vkDestroySemaphore(mDevice, mSemaphore, nullptr);
			
			mSemaphore = other.mSemaphore;
			mDevice = other.mDevice;
			
			other.mSemaphore = VK_NULL_HANDLE;
			other.mDevice = VK_NULL_HANDLE;
		}
		return *this;
	}
	~Semaphore() noexcept {
		if (mSemaphore == VK_NULL_HANDLE)
			return;
		
	
		vkDestroySemaphore(mDevice, mSemaphore, nullptr);
	}

	operator VkSemaphore() const { return mSemaphore; }

private:

	VkSemaphore mSemaphore;
	VkDevice mDevice;

};