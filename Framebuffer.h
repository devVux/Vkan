#pragma once

#include <vulkan/vulkan.h>

#include "Common.h"

#include <iostream>

class Framebuffer {
public:

    Framebuffer(VkFramebuffer framebuffer = VK_NULL_HANDLE, VkDevice device = VK_NULL_HANDLE): mFramebuffer(framebuffer), mDevice(device) { }
	Framebuffer(const Framebuffer&) = delete;
	Framebuffer(Framebuffer&& other) noexcept: mFramebuffer(other.mFramebuffer), mDevice(other.mDevice) {
		other.mFramebuffer = VK_NULL_HANDLE;
		other.mDevice = VK_NULL_HANDLE;
	}
	Framebuffer& operator=(const Framebuffer&) = delete;
	Framebuffer& operator=(Framebuffer&& other) noexcept {
		if (this != &other) {
			if (mFramebuffer != VK_NULL_HANDLE)
                vkDestroyFramebuffer(mDevice, mFramebuffer, nullptr);

			mFramebuffer = other.mFramebuffer;
			mDevice = other.mDevice;

			other.mFramebuffer = VK_NULL_HANDLE;
			other.mDevice = VK_NULL_HANDLE;
		}
		return *this;
	}
	~Framebuffer() {
		if (mDevice == VK_NULL_HANDLE)
			return;
		
		if (mFramebuffer == VK_NULL_HANDLE)
			return;
		

		vkDestroyFramebuffer(mDevice, mFramebuffer, nullptr);
	}


	operator VkFramebuffer() const { return mFramebuffer; }

private:

    VkFramebuffer mFramebuffer;
	VkDevice mDevice;

};
