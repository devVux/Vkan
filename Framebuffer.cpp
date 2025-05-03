#include "Framebuffer.h"

#include "RenderPass.h"
#include "GPU.h"

std::vector<Framebuffer*> FramebufferBuilder::create(const RenderPass& renderPass) const {
    VkFramebufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = renderPass;
    info.attachmentCount = renderPass.attachmentsCount();
    info.width = mExtent.width;
    info.height = mExtent.height;
    info.layers = 1;

	std::vector<Framebuffer*> framebuffers(mImageViews.size());
	for (std::size_t i = 0; i < mImageViews.size(); i++) {
		info.pAttachments = &mImageViews[i];
		VkFramebuffer f;

		if (vkCreateFramebuffer(*pGPU, &info, nullptr, &f) != VK_SUCCESS)
			throw std::runtime_error("failed to create framebuffer");

		framebuffers[i] = new Framebuffer(f);
	}

    return framebuffers;
}
