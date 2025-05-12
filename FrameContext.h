#pragma once

#include <vulkan/vulkan.h>
#include "CommandBuffer.h"
#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "SwapChain.h"
#include <iostream>

class FrameContext {
	friend struct Renderer;
	friend struct RenderCommands;
public:

	FrameContext(SecondaryCommandBuffer&& commands): mCommands(std::move(commands)) { }
	

	void begin(const RenderPass& renderpass, const Framebuffer& framebuffer, uint32_t subpassIndex) {
		mCommands.reset();
		mCommands.begin(renderpass, framebuffer, subpassIndex);
	}

	void bindGraphicsPipeline(const GraphicsPipeline& pipeline) {
		mCommands.bindGraphicsPipeline(pipeline);
	}

	void end(const PrimaryCommandBuffer& primaryCommands) {
		mCommands.end();
		mCommands.exec(primaryCommands);
	}

private:

	operator VkCommandBuffer() const { return mCommands; }
	
	SecondaryCommandBuffer mCommands;

};