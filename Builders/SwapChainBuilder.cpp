#include "SwapChainBuilder.h"

#include <array>
#include <cstdint>
#include "ResourceFactory.h"

#include <GLFW/glfw3.h>

#include <iostream>

VkExtent2D chooseSwapExtent(const Window& window, const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;

	int width, height;
	glfwGetFramebufferSize(window.native(), &width, &height);

	VkExtent2D actualExtent(
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height)
	);

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



SwapChain SwapChainBuilder::create(const Window& window, const GPU& gpu) const {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, window, &capabilities);

	uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, window, &formatCount, nullptr);

	std::vector<VkSurfaceFormatKHR> formats;
    if (formatCount != 0) {
        formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, window, &formatCount, formats.data());
    }
	
	VkSurfaceFormatKHR selectedFormat = chooseSwapSurfaceFormat(formats);


	uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, window, &presentModeCount, nullptr);

	std::vector<VkPresentModeKHR> presentModes;
    if (presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, window, &presentModeCount, presentModes.data());
    }


	VkPresentModeKHR mode = chooseSwapPresentMode(presentModes);

	QueueFamilyIndices indices = gpu.queueFamilies();
    std::array<uint32_t, 2> familyIndices = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};


	if (not gpu.supportsPresentation(window))
        throw std::runtime_error("Surface is not supported for presentation by this device");

	
	auto extent = chooseSwapExtent(window, capabilities);

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        imageCount = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = window;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = selectedFormat.format;
    createInfo.imageColorSpace = selectedFormat.colorSpace;
    createInfo.imageExtent = extent;
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




	auto swapChain = ResourceFactory::createSwapChain(gpu, createInfo);

	
	uint32_t finalImageCount;
	vkGetSwapchainImagesKHR(gpu, swapChain, &finalImageCount, nullptr);

    std::vector<VkImage> images(finalImageCount);
	vkGetSwapchainImagesKHR(gpu, swapChain, &finalImageCount, images.data());

	
	std::vector<VkImageView> imageViews;
	imageViews.reserve(finalImageCount);

	for (size_t i = 0; i < finalImageCount; i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = selectedFormat.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		imageViews.push_back(ResourceFactory::createImageView(gpu, createInfo));
	}	


    return SwapChain(swapChain, images, imageViews, gpu.createPresentQueue(), extent, gpu);
}
