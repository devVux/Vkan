#pragma once

#include "ResourceFactory.h"
#include "RenderPass.h"
#include "GPU.h"

class RenderPassBuilder {

public:

    RenderPassBuilder() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB; // Default color format
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		mAttachments.push_back(colorAttachment);


		VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		mAttachmentRefs.push_back(colorAttachmentRef);


		VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = static_cast<uint32_t>(mAttachmentRefs.size());
        subpass.pColorAttachments = mAttachmentRefs.data();

		mSubpass = subpass;


        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		mDependencies.push_back(dependency);
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

    RenderPass create(const GPU& gpu) const {
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(mAttachments.size());
        renderPassInfo.pAttachments = mAttachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &mSubpass;
        renderPassInfo.dependencyCount = static_cast<uint32_t>(mDependencies.size());
        renderPassInfo.pDependencies = mDependencies.data();

        auto renderPass = ResourceFactory::createRenderPass(gpu, renderPassInfo);
        return RenderPass(renderPass, mAttachments, mDependencies, mSubpass, gpu);
    }


private:
    
    std::vector<VkAttachmentDescription> mAttachments;
    std::vector<VkSubpassDependency> mDependencies;
    std::vector<VkAttachmentReference> mAttachmentRefs;
    VkSubpassDescription mSubpass{};

};