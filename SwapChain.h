#pragma once

#include "GPU.h"
#include "Window.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <algorithm>
#include <optional>

class RenderPass;

class SwapChain {

public:
	
    SwapChain(VkSwapchainKHR swapChain = VK_NULL_HANDLE, 
		const std::vector<VkImage>& images = {},
		const std::vector<VkImageView>& imageViews = {},
		VkQueue presentQueue = VK_NULL_HANDLE, 
		VkExtent2D extent = { 800, 600 },
		VkDevice device = VK_NULL_HANDLE);
	SwapChain(const SwapChain&) = delete;
	SwapChain(SwapChain&& other) noexcept: mSwapChain(other.mSwapChain), mImages(std::move(other.mImages)), mDevice(other.mDevice),
		mImageViews(std::move(other.mImageViews)), mPresentQueue(other.mPresentQueue), mExtent(other.mExtent) {
		other.mSwapChain = VK_NULL_HANDLE;
		other.mDevice = VK_NULL_HANDLE;
		other.mPresentQueue = VK_NULL_HANDLE;
		other.mExtent = { 800, 600 };
	}
	SwapChain& operator=(const SwapChain&) = delete;
	SwapChain& operator=(SwapChain&& other) noexcept {
		if (this != &other) {
			for (auto imageView : mImageViews)
				if (imageView != VK_NULL_HANDLE)
					vkDestroyImageView(mDevice, imageView, nullptr);

			if (mSwapChain != VK_NULL_HANDLE)
				vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);

			mSwapChain = other.mSwapChain;
			mDevice = other.mDevice;
			mExtent = other.mExtent;
			mPresentQueue = other.mPresentQueue;
			mImages = std::move(other.mImages);
			mImageViews = std::move(other.mImageViews);

			other.mExtent = { 800, 600 };
			other.mSwapChain = VK_NULL_HANDLE;
			other.mDevice = VK_NULL_HANDLE;
			other.mPresentQueue = VK_NULL_HANDLE;
		}
		return *this;
	}
    ~SwapChain();

	uint32_t acquireNextImage(const VkSemaphore& semaphoreToSignal) const;
	void present(const uint32_t index, const VkSemaphore& semaphore) const;

	const auto& imageViews() const { return mImageViews; }
	const std::size_t imageViewCount() const { return mImageViews.size(); }
	
	VkImageView& operator[](const std::size_t i) { return mImageViews[i]; }
	const VkImageView& operator[](const std::size_t i) const { return mImageViews[i]; }

	VkExtent2D extent() const { return mExtent; }

private:

    VkSwapchainKHR mSwapChain;
    std::vector<VkImage> mImages;
    std::vector<VkImageView> mImageViews;
	VkExtent2D mExtent;
    VkDevice mDevice;
	
    VkQueue mPresentQueue;

};
