#include "SwapChain.h"

#include <array>

#include "RenderPass.h"
#include <GLFW/glfw3.h>
#include "ResourceFactory.h"


SwapChain::SwapChain(VkSwapchainKHR swapChain, const std::vector<VkImage>& images, const std::vector<VkImageView>& imageViews, VkQueue presentQueue, VkDevice device):
	mSwapChain(swapChain), 
	mImages(images),
	mImageViews(imageViews),
	mDevice(device),
	mPresentQueue(presentQueue) { 
	}

SwapChain::~SwapChain() {
	if (mDevice == VK_NULL_HANDLE)
		return;
	if (mSwapChain == VK_NULL_HANDLE)
		return;

	for (auto imageView : mImageViews)
		if (imageView != VK_NULL_HANDLE)
			vkDestroyImageView(mDevice, imageView, nullptr);

    vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);
}

uint32_t SwapChain::acquireNextImage(const VkSemaphore& semaphoreToSignal) const {
	uint32_t imageIndex;
    vkAcquireNextImageKHR(mDevice, mSwapChain, UINT64_MAX, semaphoreToSignal, VK_NULL_HANDLE, &imageIndex);
	return imageIndex;
}

void SwapChain::present(const uint32_t index, const VkSemaphore& semaphore) const {  
   VkPresentInfoKHR info{};  
   info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;  

   info.waitSemaphoreCount = 1;  
   info.pWaitSemaphores = &semaphore;

   VkSwapchainKHR swapChains[] = { mSwapChain };  
   info.swapchainCount = 1;  
   info.pSwapchains = swapChains;  

   info.pImageIndices = &index;  

   vkQueuePresentKHR(mPresentQueue, &info);  
}
