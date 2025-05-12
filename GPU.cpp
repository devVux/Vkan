#include "GPU.h"

#include "ResourceFactory.h"

VkSurfaceCapabilitiesKHR GPU::surfaceCapabilitiesFor(const Window& window) const {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, window, &capabilities);
    return capabilities;
}

std::vector<VkSurfaceFormatKHR> GPU::formatsFor(const Window& window) const {
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, window, &formatCount, nullptr);

	std::vector<VkSurfaceFormatKHR> formats;
    if (formatCount != 0) {
        formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, window, &formatCount, formats.data());
    }

	return formats;
}

bool GPU::supportsPresentation(const Window& window) const {
	VkBool32 supported = VK_FALSE;
	vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, mIndices.presentFamily.value(), window, &supported);
	return supported == VK_TRUE;
}

PrimaryCommandBuffer GPU::createPrimaryCommandBuffer(const CommandPool& pool) const {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	return PrimaryCommandBuffer(ResourceFactory::createCommandBuffer(mDevice, allocInfo));
}

SecondaryCommandBuffer GPU::createSecondaryCommandBuffer(const CommandPool& pool) const {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	allocInfo.commandBufferCount = 1;

	return SecondaryCommandBuffer(ResourceFactory::createCommandBuffer(mDevice, allocInfo));
}

std::vector<VkPresentModeKHR> GPU::modesFor(const Window& window) const {
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, window, &presentModeCount, nullptr);

	std::vector<VkPresentModeKHR> presentModes;
    if (presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, window, &presentModeCount, presentModes.data());
    }

	return presentModes;
}


