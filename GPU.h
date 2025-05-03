#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <stdexcept>
#include <set>
#include <functional>

#include "Common.h"
#include "CommandBuffer.h"

class Context;

class GPU {
		friend class GPUSelector;

	public:

	    struct Queues {
			VkQueue graphics = VK_NULL_HANDLE;
			VkQueue present = VK_NULL_HANDLE;
			VkQueue compute = VK_NULL_HANDLE;
			VkQueue transfer = VK_NULL_HANDLE;
		};

		~GPU() {
			vkDestroyDevice(mDevice, nullptr);
		}

		void waitIdle() const {
			vkDeviceWaitIdle(mDevice); 
		}

		VkSurfaceCapabilitiesKHR surfaceCapabilitiesFor(VkSurfaceKHR surface) const;
		std::vector<VkPresentModeKHR> modesFor(VkSurfaceKHR surface) const;
		std::vector<VkSurfaceFormatKHR> formatsFor(VkSurfaceKHR surface) const;
		bool supportsPresentation(VkSurfaceKHR surface) const;

		QueueFamilyIndices queueFamilies() const { return mIndices; }
		operator VkDevice() const { return mDevice; }
		operator VkPhysicalDevice() const { return mPhysicalDevice; }

		CommandBuffer* createCommandQueue(const VkCommandPool& pool) const {
			VkCommandBuffer commands;

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = pool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = 1;

			if (vkAllocateCommandBuffers(mDevice, &allocInfo, &commands) != VK_SUCCESS)
				throw std::runtime_error("failed to allocate command buffers!");

			return new CommandBuffer(commands);
		}
		VkCommandPool createCommandPool() const {
			VkCommandPool pool;

			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			poolInfo.queueFamilyIndex = mIndices.graphicsFamily.value();

			if (vkCreateCommandPool(mDevice, &poolInfo, nullptr, &pool) != VK_SUCCESS)
				throw std::runtime_error("failed to create command pool!");

			return pool;
		}

		VkQueue graphicsQueue() const {
			VkQueue graphics;
			vkGetDeviceQueue(mDevice, mIndices.graphicsFamily.value(), 0, &graphics);
			return graphics;
		}
		VkQueue presentQueue() const {
			VkQueue present;
			vkGetDeviceQueue(mDevice, mIndices.presentFamily.value(), 0, &present);
			return present;
		}

		VkSemaphore semaphore(const VkSemaphoreCreateInfo& info) const {
			VkSemaphore semaphore;
			if (vkCreateSemaphore(mDevice, &info, nullptr, &semaphore) != VK_SUCCESS)
				std::runtime_error("Cannot create semaphore");

			return semaphore;
		}
		VkFence fence(const VkFenceCreateInfo& info) const {
			VkFence fence;
			if (vkCreateFence(mDevice, &info, nullptr, &fence) != VK_SUCCESS)
				std::runtime_error("Cannot create fence");

			return fence;
		}

	private:

		GPU(VkPhysicalDevice physicalDevice, VkDevice device, QueueFamilyIndices indices):
			mPhysicalDevice(std::move(physicalDevice)), mDevice(std::move(device)), mIndices(std::move(indices)) {

		}


		VkDevice mDevice;
		VkPhysicalDevice mPhysicalDevice;
		QueueFamilyIndices mIndices;

};

class GPUSelector {

	struct Device {
		std::string name;
		int score;
		VkPhysicalDevice device;
		QueueFamilyIndices indices;
	};

public:

	GPUSelector(Handle<Context> resourceFactory);

   	const GPUSelector& tell();
	GPU* select(int i) const;
   
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
	
	Handle<Context> pContext;

	std::vector<const char*> mExtensions;
	std::vector<const char*> mFeatures;
	std::vector<Device> mDevices;

	QueueType mRequiredQueues;

	int mMinimumScore = 1;
	VkPhysicalDeviceType mPreferredType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;


};
