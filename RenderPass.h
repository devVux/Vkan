#pragma once

#include "vulkan/vulkan.h"

#include "GPU.h"
#include "CommandBuffer.h"
#include "Common.h"
#include "ResourceFactory.h"

class GPU;

class RenderPass {
	friend class RenderPassBuilder;

public:

	~RenderPass() {
		vkDestroyRenderPass(*pGPU, mRenderPass, nullptr);
	}

	void begin(const VkFramebuffer& framebuffer, CommandBuffer& commands);
	void end(CommandBuffer& commands) const;

	operator VkRenderPass() const { return mRenderPass; }
	std::size_t attachmentsCount() const { return mAttachments.size(); }

private:

	RenderPass(VkRenderPass renderPass, 
		const std::vector<VkAttachmentDescription>& attachments,
		const std::vector<VkSubpassDependency>& dependencies,
		const VkSubpassDescription& subpass,
		Handle<GPU> gpu): 
			mRenderPass(renderPass), mAttachments(attachments), mDependencies(dependencies), 
			mSubpass(subpass), pGPU(gpu) {
	}


	VkRenderPass mRenderPass;
	std::vector<VkAttachmentDescription> mAttachments;
    std::vector<VkSubpassDependency> mDependencies;
    VkSubpassDescription mSubpass{};

	Handle<GPU> pGPU;

};

class RenderPassBuilder {

public:

    RenderPassBuilder(Handle<GPU> gpu): pGPU(gpu) {

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB; // Default color format
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        

		mAttachmentRefs.push_back(colorAttachmentRef);
		mAttachments.push_back(colorAttachment);
		mDependencies.push_back(dependency);
		mSubpass = subpass;

	}

    RenderPassBuilder& attachments(const std::vector<VkAttachmentDescription>& attachments) {
        mAttachments = attachments;
        return *this;
    }
	
    RenderPassBuilder& addAttachment(const VkAttachmentDescription& attachment) {
        mAttachments.push_back(attachment);
        return *this;
    }

    RenderPassBuilder& setSubpass(const VkSubpassDescription& subpass) {
        mSubpass = subpass;
        return *this;
    }

    RenderPassBuilder& addDependency(const VkSubpassDependency& dependency) {
        mDependencies.push_back(dependency);
        return *this;
    }

    RenderPass* create() const {
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(mAttachments.size());
        renderPassInfo.pAttachments = mAttachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &mSubpass;
        renderPassInfo.dependencyCount = static_cast<uint32_t>(mDependencies.size());
        renderPassInfo.pDependencies = mDependencies.data();

        auto renderPass = ResourceFactory::createRenderPass(*pGPU, renderPassInfo);
        return new RenderPass(renderPass, mAttachments, mDependencies, mSubpass, pGPU);
    }


private:
    
	Handle<GPU> pGPU;
    std::vector<VkAttachmentDescription> mAttachments;
    std::vector<VkSubpassDependency> mDependencies;
    std::vector<VkAttachmentReference> mAttachmentRefs;
    VkSubpassDescription mSubpass{};

};
