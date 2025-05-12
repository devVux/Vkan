#pragma once

#include <vulkan/vulkan.h>
#include <string>

#include "Common.h"
#include "GPU.h"

class GPUSelector {

	struct Device {
		std::string name;
		int score;
		VkPhysicalDevice device;
		QueueFamilyIndices indices;
	};

public:

	GPUSelector(VkInstance context = VK_NULL_HANDLE);

   	const GPUSelector& tellFor(const Window& window);
	GPU select(int i) const;
   
	GPUSelector& extensions(std::vector<const char*> extensions) {
		mExtensions = std::move(extensions);
		return *this;
	}
	GPUSelector& features(std::vector<const char*> features) {
		mFeatures = std::move(features);
		return *this;
	}
	GPUSelector& minimumScore(int score) {
		mMinimumScore = score;
		return *this;
	}
	GPUSelector& preferredDeviceType(VkPhysicalDeviceType type) {
		mPreferredType = type;
		return *this;
	}
	GPUSelector& requiredQueues(QueueType queue) {
		mRequiredQueues |= queue;
		return *this;
	}
   

private:

	int rateDevice(VkPhysicalDevice device) const;
	bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) const;
	 

private:
	
	VkInstance mContext;

	std::vector<const char*> mExtensions;
	std::vector<const char*> mFeatures;
	std::vector<Device> mDevices;

	QueueType mRequiredQueues;

	int mMinimumScore = 1;
	VkPhysicalDeviceType mPreferredType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

};
