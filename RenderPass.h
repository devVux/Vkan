#pragma once

#include "vulkan/vulkan.h"

#include "Common.h"
#include <vector>
#include <iostream>

class RenderPass {

public:

	RenderPass(VkRenderPass renderPass = VK_NULL_HANDLE, 
		const std::vector<VkAttachmentDescription>& attachments = {},
		const std::vector<VkSubpassDependency>& dependencies = {},
		const VkSubpassDescription& subpass = {},
		VkDevice device = VK_NULL_HANDLE): 
			mRenderPass(renderPass), mAttachments(attachments), mDependencies(dependencies), 
			mSubpass(subpass), mDevice(device) { }
	RenderPass(const RenderPass&) = delete;
	RenderPass(RenderPass&& other) noexcept: mRenderPass(other.mRenderPass), mDevice(other.mDevice),
		mAttachments(std::move(other.mAttachments)), mDependencies(std::move(other.mDependencies)), mSubpass(std::move(other.mSubpass)) {
		other.mRenderPass = VK_NULL_HANDLE;
		other.mDevice = VK_NULL_HANDLE;
	}
	RenderPass& operator=(const RenderPass&) = delete;
	RenderPass& operator=(RenderPass&& other) noexcept {
		if (this != &other) {
			if (mRenderPass != VK_NULL_HANDLE)
				vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
			
			mRenderPass = other.mRenderPass;
			mDevice = other.mDevice;
			mAttachments = std::move(other.mAttachments);
			mDependencies = std::move(other.mDependencies);
			mSubpass = std::move(other.mSubpass);

			other.mRenderPass = VK_NULL_HANDLE;
			other.mDevice = VK_NULL_HANDLE;
		}
		return *this;
	}
	~RenderPass() {
		if (mRenderPass == VK_NULL_HANDLE)
			return;
		
		if (mDevice == VK_NULL_HANDLE)
			return;
		

		vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
	}

	operator VkRenderPass() const { return mRenderPass; }
	std::size_t attachmentsCount() const { return mAttachments.size(); }
	uint32_t subpassIndex() const { return 0; }	// we have a single subpass

private:

	VkRenderPass mRenderPass;
	std::vector<VkAttachmentDescription> mAttachments;
    std::vector<VkSubpassDependency> mDependencies;
    VkSubpassDescription mSubpass{};

	VkDevice mDevice;

};

