#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Common.h"

class SwapChain;
class RenderPass;
class GPU;

class Framebuffer {
	friend class FramebufferBuilder;
public:

    operator VkFramebuffer() const { return mFramebuffer; }
    
private:

    Framebuffer(VkFramebuffer framebuffer): 
		mFramebuffer(std::move(framebuffer)) {
		}

private:
    VkFramebuffer mFramebuffer;
};

class FramebufferBuilder {

public:

    FramebufferBuilder(Handle<GPU> gpu): pGPU(gpu) {}

	
    std::vector<Framebuffer*> create(const RenderPass& renderPass) const;

    FramebufferBuilder& extent(VkExtent2D extent) {
        mExtent = extent;
        return *this;
    }

    FramebufferBuilder& imageViews(const std::vector<VkImageView>& views) {
        mImageViews = views;
        return *this;
    }



private:
    
	Handle<GPU> pGPU;
    VkExtent2D mExtent;
	std::vector<VkImageView> mImageViews;

};
