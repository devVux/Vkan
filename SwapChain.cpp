#include "SwapChain.h"

#include <array>

#include "RenderPass.h"

SwapChain::SwapChain(VkSwapchainKHR swapChain, VkFormat imageFormat, VkExtent2D extent, Handle<GPU> gpu): 
	mSwapChain(swapChain), 
	mExtent(std::move(extent)),
	pGPU(gpu),
	presentQueue(gpu->presentQueue()),
	graphicsQueue(gpu->graphicsQueue()),
	mImageAvailableSemaphore(gpu->semaphore({ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO })),
	mRenderFinishedSemaphore(gpu->semaphore({ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO })),
	mInFlightFence(gpu->fence({ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, VK_FENCE_CREATE_SIGNALED_BIT}))
{

	uint32_t finalImageCount;
	vkGetSwapchainImagesKHR(*gpu, swapChain, &finalImageCount, nullptr);

	mImages.resize(finalImageCount);
	mImageViews.resize(finalImageCount);

	vkGetSwapchainImagesKHR(*gpu, swapChain, &finalImageCount, mImages.data());


	for (size_t i = 0; i < finalImageCount; i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = mImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = imageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(*pGPU, &createInfo, nullptr, &mImageViews[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to create image views!");

	}	
	
}
	
SwapChain::~SwapChain() {
	vkDestroySemaphore(*pGPU, mRenderFinishedSemaphore, nullptr);
    vkDestroySemaphore(*pGPU, mImageAvailableSemaphore, nullptr);
    vkDestroyFence(*pGPU, mInFlightFence, nullptr);

	for (auto imageView : mImageViews)
		vkDestroyImageView(*pGPU, imageView, nullptr);
		
    vkDestroySwapchainKHR(*pGPU, mSwapChain, nullptr);
}

void SwapChain::sync() const {
    vkWaitForFences(*pGPU, 1, &mInFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(*pGPU, 1, &mInFlightFence);
}

uint32_t SwapChain::acquireNextImage() const {
	uint32_t imageIndex;
    vkAcquireNextImageKHR(*pGPU, mSwapChain, UINT64_MAX, mImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	return imageIndex;
}

void SwapChain::present(const uint32_t index, const CommandBuffer& commands) const {  
   VkSubmitInfo submitInfo{};  
   submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;  

   VkSemaphore waitSemaphores[] = {mImageAvailableSemaphore};  
   VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};  
   submitInfo.waitSemaphoreCount = 1;  
   submitInfo.pWaitSemaphores = waitSemaphores;  
   submitInfo.pWaitDstStageMask = waitStages;  

   submitInfo.commandBufferCount = 1;  
   submitInfo.pCommandBuffers = reinterpret_cast<const VkCommandBuffer*>(&commands);

   VkSemaphore signalSemaphores[] = {mRenderFinishedSemaphore};  
   submitInfo.signalSemaphoreCount = 1;  
   submitInfo.pSignalSemaphores = signalSemaphores;  

   // Submit the command buffer
   vkQueueSubmit(graphicsQueue, 1, &submitInfo, mInFlightFence);  

   VkPresentInfoKHR presentInfo{};  
   presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;  

   presentInfo.waitSemaphoreCount = 1;  
   presentInfo.pWaitSemaphores = signalSemaphores;  

   VkSwapchainKHR swapChains[] = {mSwapChain};  
   presentInfo.swapchainCount = 1;  
   presentInfo.pSwapchains = swapChains;  

   presentInfo.pImageIndices = &index;  

   // Present the image
   vkQueuePresentKHR(presentQueue, &presentInfo);  
}







VkExtent2D chooseSwapExtent(VkSurfaceKHR surface, const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;

    int width = 800, height = 600;
    //glfwGetFramebufferSize(surface, &width, &height);
	
    VkExtent2D actualExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats)
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;

    return availableFormats[0];
}
		
VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes)
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;

    return VK_PRESENT_MODE_FIFO_KHR;
}



SwapChain* SwapChainBuilder::create(VkSurfaceKHR surface) {
    VkSurfaceCapabilitiesKHR capabilities = pGPU->surfaceCapabilitiesFor(surface);

	VkSurfaceFormatKHR selectedFormat = chooseSwapSurfaceFormat(pGPU->formatsFor(surface));
	VkPresentModeKHR mode = chooseSwapPresentMode(pGPU->modesFor(surface));

	QueueFamilyIndices indices = pGPU->queueFamilies();
    std::array<uint32_t, 2> familyIndices = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};


	if (!pGPU->supportsPresentation(surface))
        throw std::runtime_error("Surface is not supported for presentation by this device");

	mExtent = chooseSwapExtent(surface, capabilities);


    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        imageCount = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = selectedFormat.format;
    createInfo.imageColorSpace = selectedFormat.colorSpace;
    createInfo.imageExtent = mExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;



    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = static_cast<uint32_t>(familyIndices.size());
		createInfo.pQueueFamilyIndices = familyIndices.data();
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = mode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

	auto swapChain = ResourceFactory::createSwapChain(*pGPU, createInfo);

    return new SwapChain(swapChain, selectedFormat.format, mExtent, pGPU);
}
