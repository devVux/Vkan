#include "ResourceFactory.h"

#include "Common.h"

#include <stdexcept>

#include <GLFW/glfw3.h>

VkInstance ResourceFactory::createContext(const VkInstanceCreateInfo& info) {
	VkInstance instance { VK_NULL_HANDLE };

    if (vkCreateInstance(&info, nullptr, &instance)!= VK_SUCCESS)
        throw std::runtime_error("Failed to create Vulkan instance");
        
    return instance;
}

VkSurfaceKHR ResourceFactory::createSurface(VkInstance instance, GLFWwindow* window) {
    VkSurfaceKHR surface { VK_NULL_HANDLE };
	
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");

	return surface;
}

VkDevice ResourceFactory::selectGPU(VkPhysicalDevice device, const VkDeviceCreateInfo& info) {
	VkDevice gpu { VK_NULL_HANDLE };
    if (vkCreateDevice(device, &info, nullptr, &gpu) != VK_SUCCESS)
        throw std::runtime_error("failed to create logical device!");

	return gpu;
}

VkSwapchainKHR ResourceFactory::createSwapChain(VkDevice device, const VkSwapchainCreateInfoKHR& info) {
	VkSwapchainKHR swapChain { VK_NULL_HANDLE };

    if (vkCreateSwapchainKHR(device, &info, nullptr, &swapChain) != VK_SUCCESS)
        throw std::runtime_error("failed to create swap chain!");

	return swapChain;
}

VkRenderPass ResourceFactory::createRenderPass(VkDevice device, const VkRenderPassCreateInfo& info) {
	VkRenderPass renderPass { VK_NULL_HANDLE };

	if (vkCreateRenderPass(device, &info, nullptr, &renderPass) != VK_SUCCESS)
		throw std::runtime_error("failed to create render pass!");

	return renderPass;
}

VkPipelineLayout ResourceFactory::createPipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo& info) {
    VkPipelineLayout pipelineLayout { VK_NULL_HANDLE };

    if (vkCreatePipelineLayout(device, &info, nullptr, &pipelineLayout) != VK_SUCCESS)
        throw std::runtime_error("failed to create pipeline layout!");

	return pipelineLayout;
}

VkCommandBuffer ResourceFactory::createCommandBuffer(VkDevice device, const VkCommandBufferAllocateInfo & info) {
	VkCommandBuffer commands;
	
	if (vkAllocateCommandBuffers(device, &info, &commands) != VK_SUCCESS)
		throw std::runtime_error("failed to create command buffers!");

	return commands;
}

VkImageView ResourceFactory::createImageView(VkDevice device, const VkImageViewCreateInfo& info) {
	VkImageView image{};

	if (vkCreateImageView(device, &info, nullptr, &image) != VK_SUCCESS)
		throw std::runtime_error("failed to create image views!");

	return image;
}

VkFramebuffer ResourceFactory::createFramebuffer(VkDevice device, const VkFramebufferCreateInfo & info) {
	VkFramebuffer framebuffer{};
	
	if (vkCreateFramebuffer(device, &info, nullptr, &framebuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to create framebuffer");

	return framebuffer;
}

VkPipeline ResourceFactory::createGraphicsPipeline(VkDevice device, const VkGraphicsPipelineCreateInfo& info) {
    VkPipeline graphicsPipeline { VK_NULL_HANDLE };

	if (vkCreateGraphicsPipelines(device, nullptr, 1, &info, nullptr, &graphicsPipeline) != VK_SUCCESS)
		throw std::runtime_error("failed to create graphics pipeline!");

	return graphicsPipeline;
}


std::vector<VkPhysicalDevice> ResourceFactory::tellGPUs(VkInstance instance) {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
       
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
       
	return devices;
}

