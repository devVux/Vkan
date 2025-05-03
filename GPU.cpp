#include "GPU.h"

#include "Context.h"

#include <algorithm>
#include <ranges>
#include <iostream>
#include <cassert>

VkSurfaceCapabilitiesKHR GPU::surfaceCapabilitiesFor(VkSurfaceKHR surface) const {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, surface, &capabilities);
    return capabilities;
}

std::vector<VkSurfaceFormatKHR> GPU::formatsFor(VkSurfaceKHR surface) const {
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, surface, &formatCount, nullptr);

	std::vector<VkSurfaceFormatKHR> formats;
    if (formatCount != 0) {
        formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, surface, &formatCount, formats.data());
    }

	return formats;
}

bool GPU::supportsPresentation(VkSurfaceKHR surface) const {
	VkBool32 supported = VK_FALSE;
	vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, mIndices.presentFamily.value(), surface, &supported);
	return supported == VK_TRUE;
}

std::vector<VkPresentModeKHR> GPU::modesFor(VkSurfaceKHR surface) const {
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, surface, &presentModeCount, nullptr);

	std::vector<VkPresentModeKHR> presentModes;
    if (presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, surface, &presentModeCount, presentModes.data());
    }

	return presentModes;
}








GPU* GPUSelector::select(int i) const {
	 
	const Device& selected = mDevices[i];
	const auto& indices = selected.indices;

	std::set<uint32_t> uniqueQueueFamilies;
	if (mRequiredQueues & QueueType::Graphics) 
		uniqueQueueFamilies.insert(indices.graphicsFamily.value());
	if (mRequiredQueues & QueueType::Present)
		uniqueQueueFamilies.insert(indices.presentFamily.value());
	if (mRequiredQueues & QueueType::Compute)
		uniqueQueueFamilies.insert(indices.computeFamily.value());
	if (mRequiredQueues & QueueType::Transfer)
		uniqueQueueFamilies.insert(indices.transferFamily.value());


	assert(not uniqueQueueFamilies.empty());



	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	float queuePriority = 1.0f;	
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
    }

	

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(mExtensions.size());
    createInfo.ppEnabledExtensionNames = mExtensions.data();
	createInfo.enabledLayerCount = 0;

	VkDevice gpu = ResourceFactory::selectGPU(selected.device, createInfo);

	return new GPU(selected.device, gpu, indices);
}

const GPUSelector& GPUSelector::tell() {

	auto candidateDevices = pContext->tellGPUs()
		| std::views::transform([&](VkPhysicalDevice device) {
				int score = rateDevice(device);
				QueueFamilyIndices indices = findQueueFamilies(device, pContext->surface());
				
				VkPhysicalDeviceProperties props;
				vkGetPhysicalDeviceProperties(device, &props);

				return Device{props.deviceName, score, device, indices};
			})
		| std::views::filter([&](const Device& device) {
				return device.score >= mMinimumScore && device.indices.isComplete(mRequiredQueues);
			});

	std::vector devices(candidateDevices.begin(), candidateDevices.end());

    mDevices = std::move(devices);  
    std::ranges::sort(mDevices, [](const auto& lhs, const auto& rhs) {  
		return rhs.score - lhs.score;  
    });

	std::ranges::for_each(mDevices, [](const auto& device) { 
		std::cout << device.name << '\n'; 
	});

	return *this;
}

GPUSelector::GPUSelector(Handle<Context> context): pContext(context) {
	// Initialize with reasonable defaults
	mExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}

QueueFamilyIndices GPUSelector::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) const {
    QueueFamilyIndices indices;
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        auto& props = queueFamilies[i];
        
        if (!indices.graphicsFamily.has_value() && (props.queueFlags & VK_QUEUE_GRAPHICS_BIT))
            indices.graphicsFamily = i;
        
        if (!indices.computeFamily.has_value() && (props.queueFlags & VK_QUEUE_COMPUTE_BIT))
            indices.computeFamily = i;
        
        if (!indices.transferFamily.has_value() && (props.queueFlags & VK_QUEUE_TRANSFER_BIT))
            indices.transferFamily = i;
        
        if (!indices.presentFamily.has_value() && surface != VK_NULL_HANDLE) {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport)
                indices.presentFamily = i;
        }
    }
    
    // Some devices might have dedicated compute or transfer queues
    // If we didn't find specific ones, fall back to graphics queue
    if (indices.graphicsFamily.has_value()) {
        if (!indices.computeFamily.has_value()) 
			indices.computeFamily = indices.graphicsFamily;
        if (!indices.transferFamily.has_value()) 
			indices.transferFamily = indices.graphicsFamily;
    }
    
    return indices;
}

int GPUSelector::rateDevice(VkPhysicalDevice device) const {
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
       
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);
       
	int score = 0;
       
	if (!checkDeviceExtensionSupport(device))
		return 0;
       
	if (properties.deviceType == mPreferredType)
		score += 1000;
	else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 800;
	else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		score += 500;
		   
	// Add more points for certain features
	if (features.geometryShader) score += 100;
	if (features.tessellationShader) score += 100;
       
	// Add points based on max image dimension
	score += properties.limits.maxImageDimension2D / 1000;
       
	return score;
}

bool GPUSelector::checkDeviceExtensionSupport(VkPhysicalDevice device) const {
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
	
    auto isSupported = [&](const char* required) {
        return std::any_of(availableExtensions.begin(), availableExtensions.end(),
            [&](const VkExtensionProperties& ext) {
                return std::strcmp(required, ext.extensionName) == 0;
            });
    };

    return std::all_of(mExtensions.begin(), mExtensions.end(), isSupported);
}

