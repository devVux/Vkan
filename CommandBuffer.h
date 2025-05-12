#pragma once

#include <vulkan/vulkan.h>

#include "Fence.h"
#include "Common.h"

#include "Framebuffer.h"
#include "RenderPass.h"
#include "GraphicsPipeline.h"

class PrimaryCommandBuffer {

public:

	PrimaryCommandBuffer(VkCommandBuffer commands = VK_NULL_HANDLE): mCommands(commands) { }
	PrimaryCommandBuffer(const PrimaryCommandBuffer&) = delete;
	PrimaryCommandBuffer(PrimaryCommandBuffer&& other) noexcept: mCommands(other.mCommands) {
		other.mCommands = VK_NULL_HANDLE;
	}
	PrimaryCommandBuffer& operator=(const PrimaryCommandBuffer&) = delete;
	PrimaryCommandBuffer& operator=(PrimaryCommandBuffer&& other) noexcept {
		if (this != &other) {
			mCommands = other.mCommands;

			other.mCommands = VK_NULL_HANDLE;
		}
		return *this;
	}
	~PrimaryCommandBuffer() = default;
	

	// TODO: rewrite in a more functional style
    void submit(VkQueue queue, std::vector<VkSemaphore> waitSemaphores, std::vector<VkSemaphore> signalSemaphores, const Fence& fence) const;
		
	void begin();
	void beginRenderPass(const RenderPass& renderpass, const VkFramebuffer& framebuffer);

    void reset();

	void endRenderPass(const RenderPass& renderpass);
    void end();

	operator VkCommandBuffer() const { return mCommands; }


private:

	VkCommandBuffer mCommands;

};


class SecondaryCommandBuffer {

public:
		
	SecondaryCommandBuffer(VkCommandBuffer commands = VK_NULL_HANDLE): mCommands(commands) { }
	SecondaryCommandBuffer(const SecondaryCommandBuffer&) = delete;
	SecondaryCommandBuffer(SecondaryCommandBuffer&& other) noexcept: mCommands(other.mCommands) {
		other.mCommands = VK_NULL_HANDLE;
	}
	SecondaryCommandBuffer& operator=(const SecondaryCommandBuffer&) = delete;
	SecondaryCommandBuffer& operator=(SecondaryCommandBuffer&& other) noexcept {
		if (this != &other) {
			mCommands = other.mCommands;

			other.mCommands = VK_NULL_HANDLE;
		}
		return *this;
	}
	~SecondaryCommandBuffer() = default;
	

	void begin(const RenderPass& renderpass, const Framebuffer& framebuffer, uint32_t subpassIndex);
	void bindGraphicsPipeline(const GraphicsPipeline& pipeline);

	void reset() { }
	void exec(const PrimaryCommandBuffer& primaryCommands) const;

	void end();
	

	operator VkCommandBuffer() const { return mCommands; }

private:

	VkCommandBuffer mCommands;

};
