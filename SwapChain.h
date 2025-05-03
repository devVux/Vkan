#pragma once

#include "Context.h"
#include "GPU.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <algorithm>
#include <optional>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Framebuffer.h"
#include "Fence.h"
#include "CommandBuffer.h"
#include <memory>

class RenderPass;

class SwapChain {
    friend class SwapChainBuilder;

public:
    ~SwapChain();

	void sync() const;
	uint32_t acquireNextImage() const;
	void present(const uint32_t index, const CommandBuffer& commands) const;

	const auto& imageViews() const { return mImageViews; }
	const std::size_t imageViewCount() const { return mImageViews.size(); }
	
	VkImageView& operator[](const std::size_t i) { return mImageViews[i]; }
	const VkImageView& operator[](const std::size_t i) const { return mImageViews[i]; }


private:
    SwapChain(VkSwapchainKHR swapChain, VkFormat imageFormat, VkExtent2D extent, Handle<GPU> gpu);

private:

    VkSwapchainKHR mSwapChain;
    VkFormat mImageFormat;
    std::vector<VkImage> mImages;
    std::vector<VkImageView> mImageViews;
    Handle<GPU> pGPU;
	
	VkExtent2D mExtent;

	VkFence mInFlightFence;
	VkSemaphore mSemaphore;

	uint32_t mImageIndex;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
	
    VkSemaphore mImageAvailableSemaphore;
    VkSemaphore mRenderFinishedSemaphore;
};

class SwapChainBuilder {

public:
    SwapChainBuilder(Handle<GPU> gpu): pGPU(gpu) {}

    SwapChain* create(VkSurfaceKHR surface);

	SwapChainBuilder& extent(VkExtent2D extent) {
		mExtent = extent;
		return *this;
	}


private:

	Handle<GPU> pGPU;

    VkExtent2D mExtent;
};
