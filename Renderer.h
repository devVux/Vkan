#pragma once

#include <vulkan/vulkan.h>
#include "FrameContext.h"


struct Renderer {

	static void drawTriangle() {

	}

};

struct RenderCommands {

	static void viewport(const FrameContext& context, float x, float y, float w, float h, float minDepth = 0.0f, float maxDepth = 1.0f) {
		VkViewport viewport{};
		viewport.x = x;
		viewport.y = y;
		viewport.width = w;
		viewport.height = h;
		viewport.minDepth = minDepth;
		viewport.maxDepth = maxDepth;
		vkCmdSetViewport(context, 0, 1, &viewport);
	}

	static void scissor(const FrameContext& context, int32_t x, int32_t y, uint32_t w, uint32_t h) {
		VkRect2D scissor{};
		scissor.offset = { x, y };
		scissor.extent = { w, h };
		vkCmdSetScissor(context, 0, 1, &scissor);
	}

	static void draw(const FrameContext& context, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
		vkCmdDraw(context, vertexCount, instanceCount, firstVertex, firstInstance);
	}

};
