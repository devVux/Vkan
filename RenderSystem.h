#pragma once

#include <vulkan/vulkan.h>

#include "Common.h"

#include "Window.h"

#include "Context.h"
#include "GPUSelector.h"
#include "RenderPassBuilder.h"
#include "SwapChainBuilder.h"
#include "FramebufferBuilder.h"
#include "GraphicsPipelineBuilder.h"
#include "FrameContext.h"

#include "CommandPool.h"
#include "Fence.h"
#include "Semaphore.h"
#include "CommandBuffer.h"


class RenderSystem {

public:

	RenderSystem() = default;
	~RenderSystem() {
		mGPU.waitIdle();
	}

	void init(const Context& context, const Window& window) {
		VkExtent2D extent = { window.size().width, window.size().height };

		mGPU = GPUSelector(context)
			.requiredQueues(QueueType::Compute | QueueType::Graphics | QueueType::Present)
			.tellFor(window)
			.select(0);

		mRenderpass = RenderPassBuilder()
 			.create(mGPU);

		mSwapchain = SwapChainBuilder()
			.create(window, mGPU);
		
		mFramebuffer = FramebufferBuilder()
			.extent(extent)
			.create(mRenderpass, mSwapchain, mGPU);

		mPipeline = GraphicsPipelineBuilder()
			.create(mRenderpass, mGPU);


		mPool = mGPU.createCommandPool();
		mCommands = mGPU.createPrimaryCommandBuffer(mPool);

		mGraphicsQueue = mGPU.createGraphicsQueue();
		
		mInFlightFence = mGPU.createFence();
		mImageAvailableSemaphore = mGPU.createSemaphore();
		mRenderFinishedSemaphore = mGPU.createSemaphore();

	}

	FrameContext createFrameContext() const {
		mInFlightFence.wait();
		mInFlightFence.reset();

		return FrameContext(mGPU.createSecondaryCommandBuffer(mPool));
	}

	void begin(FrameContext& context) {
		mImageIndex = mSwapchain.acquireNextImage(mImageAvailableSemaphore);
		
		mCommands.reset();
		mCommands.begin();
		mCommands.beginRenderPass(mRenderpass, mFramebuffer[mImageIndex]);

		context.begin(mRenderpass, mFramebuffer[mImageIndex], mRenderpass.subpassIndex());
		context.bindGraphicsPipeline(mPipeline);
	}

	void end(FrameContext& context) {
		context.end(mCommands);
		
		mCommands.endRenderPass(mRenderpass);
		mCommands.end();
		mCommands.submit(
			mGraphicsQueue, 
			{ mImageAvailableSemaphore }, 
			{ mRenderFinishedSemaphore }, 
			mInFlightFence
		);

		mSwapchain.present(mImageIndex, mRenderFinishedSemaphore);
	}


private:

	// Reminder: order of deletion matters! we might create ::destroy() methods, we'll see...
	GPU mGPU;
	RenderPass mRenderpass;
	GraphicsPipeline mPipeline;
	std::vector<Framebuffer> mFramebuffer;
	SwapChain mSwapchain;
	CommandPool mPool;
	Fence mInFlightFence;
    Semaphore mImageAvailableSemaphore;
    Semaphore mRenderFinishedSemaphore;
	
	// These will be deleted by the pool and gpu
	PrimaryCommandBuffer mCommands;
    VkQueue mGraphicsQueue;

	uint32_t mImageIndex;

};