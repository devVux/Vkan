#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <stdexcept>
#include <functional>
#include <memory>
#include <iostream>

#include "Common.h"
#include "Fence.h"
#include "Semaphore.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "Window.h"


class GPU {

	public:

	    struct Queues {
			VkQueue graphics = VK_NULL_HANDLE;
			VkQueue present = VK_NULL_HANDLE;
			VkQueue compute = VK_NULL_HANDLE;
			VkQueue transfer = VK_NULL_HANDLE;
		};

		GPU(VkPhysicalDevice physicalDevice = VK_NULL_HANDLE, VkDevice device = VK_NULL_HANDLE, QueueFamilyIndices indices = {}):
			mPhysicalDevice(physicalDevice), mDevice(device), mIndices(indices) { }
		GPU(const GPU&) = delete;
		GPU(GPU&& other) noexcept: 
			mDevice(other.mDevice), mPhysicalDevice(other.mPhysicalDevice), mIndices(std::move(other.mIndices)) {}
		GPU& operator=(const GPU&) = delete;
		GPU& operator=(GPU&& other) noexcept {
			if (this != &other) {
				mDevice = other.mDevice;
				mPhysicalDevice = other.mPhysicalDevice;
				mIndices = std::move(other.mIndices);

				other.mDevice = VK_NULL_HANDLE;
				other.mPhysicalDevice = VK_NULL_HANDLE;
			}
			return *this;
		}
		~GPU() {
			if (mPhysicalDevice == VK_NULL_HANDLE)
				return;
			
			if(mDevice == VK_NULL_HANDLE)
				return;
			

			vkDestroyDevice(mDevice, nullptr);
		}

		void waitIdle() const {
			vkDeviceWaitIdle(mDevice); 
		}

		VkSurfaceCapabilitiesKHR surfaceCapabilitiesFor(const Window& window) const;
		std::vector<VkPresentModeKHR> modesFor(const Window& window) const;
		std::vector<VkSurfaceFormatKHR> formatsFor(const Window& window) const;
		bool supportsPresentation(const Window& window) const;

		QueueFamilyIndices queueFamilies() const { return mIndices; }
		operator VkDevice() const { return mDevice; }
		operator VkPhysicalDevice() const { return mPhysicalDevice; }

		PrimaryCommandBuffer createPrimaryCommandBuffer(const CommandPool& pool) const;
		SecondaryCommandBuffer createSecondaryCommandBuffer(const CommandPool& pool) const;
		CommandPool createCommandPool() const {
			VkCommandPool pool;

			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			poolInfo.queueFamilyIndex = mIndices.graphicsFamily.value();

			if (vkCreateCommandPool(mDevice, &poolInfo, nullptr, &pool) != VK_SUCCESS)
				throw std::runtime_error("failed to create command pool!");

			return CommandPool(pool, mDevice);
		}

		VkQueue createGraphicsQueue() const {
			VkQueue graphics;
			vkGetDeviceQueue(mDevice, mIndices.graphicsFamily.value(), 0, &graphics);
			return graphics;
		}
		VkQueue createPresentQueue() const {
			VkQueue present;
			vkGetDeviceQueue(mDevice, mIndices.presentFamily.value(), 0, &present);
			return present;
		}

		Semaphore createSemaphore() const {
			VkSemaphore semaphore;

			VkSemaphoreCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			if (vkCreateSemaphore(mDevice, &info, nullptr, &semaphore) != VK_SUCCESS)
				std::runtime_error("Cannot create semaphore");

			return Semaphore(semaphore, mDevice);
		}
		Fence createFence(bool signaled = true) {
			VkFence fence;

		    VkFenceCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

			if (vkCreateFence(mDevice, &info, nullptr, &fence) != VK_SUCCESS)
				throw std::runtime_error("failed to create mFence!");

			return Fence(fence, mDevice);
		}

	private:

		VkDevice mDevice;
		VkPhysicalDevice mPhysicalDevice;
		QueueFamilyIndices mIndices;

};
