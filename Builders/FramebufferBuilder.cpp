#include "FramebufferBuilder.h"

#include "ResourceFactory.h"

std::vector<Framebuffer> FramebufferBuilder::create(const RenderPass& renderPass, const SwapChain& swapchain, const GPU& gpu) const {
    VkFramebufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = renderPass;
    info.attachmentCount = renderPass.attachmentsCount();
    info.width = swapchain.extent().width;
    info.height = swapchain.extent().height;
    info.layers = 1;
	
	std::vector<Framebuffer> framebuffers;
	framebuffers.reserve(swapchain.imageViewCount());

	for (std::size_t i = 0; i < swapchain.imageViewCount(); i++) {
		info.pAttachments = &swapchain[i];
		
		framebuffers.push_back(Framebuffer(ResourceFactory::createFramebuffer(gpu, info), gpu));
	}

    return framebuffers;
}
