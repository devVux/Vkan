#pragma once

#include <vulkan/vulkan.h>

#include "Common.h"

class GraphicsPipeline {

public:
	
	GraphicsPipeline(VkPipeline pipeline = VK_NULL_HANDLE, VkPipelineLayout layout = VK_NULL_HANDLE, VkDevice device = VK_NULL_HANDLE): 
		mPipeline(pipeline), mLayout(layout), mDevice(device) { }
	GraphicsPipeline(const GraphicsPipeline&) = delete;
	GraphicsPipeline(GraphicsPipeline&& other) noexcept: mPipeline(other.mPipeline), mLayout(other.mLayout), mDevice(other.mDevice) {
		other.mPipeline = VK_NULL_HANDLE;
		other.mLayout = VK_NULL_HANDLE;
		other.mDevice = VK_NULL_HANDLE;
	}
	GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
	GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept {
		if (this != &other) {
			if (mPipeline != VK_NULL_HANDLE)
				vkDestroyPipeline(mDevice, mPipeline, nullptr);
			if (mLayout != VK_NULL_HANDLE)
				vkDestroyPipelineLayout(mDevice, mLayout, nullptr);

			mPipeline = other.mPipeline;
			mLayout = other.mLayout;
			mDevice = other.mDevice;

			other.mPipeline = VK_NULL_HANDLE;
			other.mLayout = VK_NULL_HANDLE;
			other.mDevice = VK_NULL_HANDLE;
		}
		return *this;
	}
	~GraphicsPipeline() {
		if (mPipeline == VK_NULL_HANDLE)
			return;
		
		if (mLayout == VK_NULL_HANDLE)
			return;
		

		vkDestroyPipeline(mDevice, mPipeline, nullptr);
        vkDestroyPipelineLayout(mDevice, mLayout, nullptr);
	}

	operator VkPipeline() const { return mPipeline; }


	VkPipeline mPipeline;
	VkPipelineLayout mLayout;
	VkDevice mDevice;

};

