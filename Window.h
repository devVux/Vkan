#pragma once

#include <string>

#include "Common.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

struct GLFWwindow;

class Window {

public:

	Window(VkInstance context, std::string title, Size size = { 800, 600 });
	~Window();

	void update() const;
	bool shouldClose() const;

	Size size() const;

	GLFWwindow* native() const;
	operator VkSurfaceKHR() const { return mSurface; }

private:

	GLFWwindow* pWindow { nullptr };
	VkSurfaceKHR mSurface { VK_NULL_HANDLE };
	VkInstance mContext;

};

