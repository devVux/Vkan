#pragma once

#include <vulkan/vulkan.h>

#include <string>

#include "ResourceFactory.h"
#include "Window.h"

#include "Common.h"

class Context {

public:

    Context(VkInstance instance = VK_NULL_HANDLE ): mInstance(instance) {}
	Context(const Context&) = delete;
	Context(Context&& other) noexcept: mInstance(other.mInstance), mSurface(other.mSurface) {
		other.mInstance = VK_NULL_HANDLE;
		other.mSurface = VK_NULL_HANDLE;
	}
	Context& operator=(const Context&) = delete;
	Context& operator=(Context&& other) noexcept;
	~Context();

	VkSurfaceKHR createSurface(const Window& window);

	VkSurfaceKHR surface() const { return mSurface; }

	std::vector<VkPhysicalDevice> tellGPUs() const;

	operator VkInstance() const { return mInstance; }

private:
	VkInstance mInstance;
	VkSurfaceKHR mSurface { VK_NULL_HANDLE };
};



class ContextBuilder {
public:

    explicit ContextBuilder();
    
    Context create();
    
    ContextBuilder& applicationName(const char* name);
    ContextBuilder& applicationVersion(uint32_t major, uint32_t minor, uint32_t patch);
    ContextBuilder& engineName(const char* name);
    ContextBuilder& engineVersion(uint32_t major, uint32_t minor, uint32_t patch);
    ContextBuilder& apiVersion(uint32_t variant, uint32_t major, uint32_t minor);

private:
    
	VkApplicationInfo appInfo{};

};
