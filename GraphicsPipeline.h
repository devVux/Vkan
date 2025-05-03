#pragma once

#include <vulkan/vulkan.h>

#include "Common.h"
#include "CommandBuffer.h"
#include "GPU.h"

class GPU;
class RenderPass;

class GraphicsPipeline {

	friend class GraphicsPipelineBuilder;

public:
	
	~GraphicsPipeline() {
		vkDestroyPipeline(*pGPU, mPipeline, nullptr);
        vkDestroyPipelineLayout(*pGPU, mLayout, nullptr);
	}

	void bind(const CommandBuffer& commands) const {
		vkCmdBindPipeline(commands, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
	}

	operator VkPipeline() const { return mPipeline; }

private:

	GraphicsPipeline(VkPipeline pipeline, VkPipelineLayout layout, Handle<GPU> gpu): 
		mPipeline(pipeline), mLayout(layout), pGPU(gpu) { }
	

	VkPipeline mPipeline;
	VkPipelineLayout mLayout;
	Handle<GPU> pGPU;
};

class GraphicsPipelineBuilder {

public:

	GraphicsPipelineBuilder(Handle<RenderPass> renderPass, Handle<GPU> gpu): 
		pGPU(gpu), pRenderPass(renderPass) { }

	GraphicsPipeline* create() const;

	

private:

	Handle<RenderPass> pRenderPass;
	Handle<GPU> pGPU;

};
