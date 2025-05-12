#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Framebuffer.h"
#include "RenderPass.h"
#include "SwapChain.h"
#include "GPU.h"

class FramebufferBuilder {

public:

    std::vector<Framebuffer> create(const RenderPass& renderPass, const SwapChain& swapchain, const GPU& gpu) const;

	// TODO: put this inside the swapchain
    FramebufferBuilder& extent(VkExtent2D extent) {
        mExtent = extent;
        return *this;
    }


private:
    
    VkExtent2D mExtent;

};
