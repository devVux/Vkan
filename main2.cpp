#include "Common.h"
#include "Context.h"
#include "GPU.h"
#include "GraphicsPipeline.h"
#include "Window.h"
#include "SwapChain.h"
#include "Fence.h"
#include "CommandBuffer.h"
#include "Framebuffer.h"
#include "CommandQueue.h"
#include "RenderPass.h"


class RenderSystem {

public:

	~RenderSystem() {
		pGPU->waitIdle();
		
		delete pSwapchain;
		
        vkDestroyCommandPool(*pGPU, mPool, nullptr);
		for (auto& framebuffer : mFramebuffer)
			vkDestroyFramebuffer(*pGPU, *framebuffer, nullptr);

		delete pPipeline;
		delete pRenderpass;

		delete pGPU;
		delete pContext;
	}

	void init(const Window& window) {
		VkExtent2D extent = { window.size().width, window.size().height };
	
		pContext = ContextBuilder()
			.applicationName("App")
			.applicationVersion(1, 0, 0)
			.engineName("Engine")
			.engineVersion(1, 0, 0)
			.apiVersion(0, 1, 0)
			.create();

		pSurface = pContext->createSurface(window);

		pGPU = GPUSelector(pContext)
			.requiredQueues(QueueType::Compute | QueueType::Graphics)
			.tell()
			.select(0);

		pRenderpass = RenderPassBuilder(pGPU)
 			.create();

		pSwapchain = SwapChainBuilder(pGPU)
			.extent(extent)
			.create(pSurface);
		
		mFramebuffer = FramebufferBuilder(pGPU)
			.extent(extent)
			.imageViews(pSwapchain->imageViews())
			.create(*pRenderpass);

		pPipeline = GraphicsPipelineBuilder(pRenderpass, pGPU)
			.create();


		mPool = pGPU->createCommandPool();
		pCommands = pGPU->createCommandQueue(mPool);

	}

	void begin() {
		pSwapchain->sync();
		mImageIndex = pSwapchain->acquireNextImage();
		pCommands->reset();
		pCommands->begin();

		pRenderpass->begin(*mFramebuffer[mImageIndex], *pCommands);
		pPipeline->bind(*pCommands);


		VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) 800;
        viewport.height = (float) 600;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(*pCommands, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = {800, 600};
        vkCmdSetScissor(*pCommands, 0, 1, &scissor);

        vkCmdDraw(*pCommands, 3, 1, 0, 0);
	}

	void end() {
		pRenderpass->end(*pCommands);
		pCommands->end();
		pSwapchain->present(mImageIndex, *pCommands);
	}


private:

	VkSurfaceKHR pSurface;
	Context* pContext;
	GPU* pGPU;
	RenderPass* pRenderpass;
	SwapChain* pSwapchain;
	GraphicsPipeline* pPipeline;
	CommandBuffer* pCommands;
	VkCommandPool mPool;
	std::vector<Framebuffer*> mFramebuffer;

	uint32_t mImageIndex;

};



int main() {

	glfwInit();

{
	Window window("Test window");

	RenderSystem renderer;
	renderer.init(window);

	while (!window.shouldClose()) {
		window.update();

		renderer.begin();
		{

			//renderer.draw(vertices)

		}
		renderer.end();

	}
}


	glfwTerminate();
}
