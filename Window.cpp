#include "Window.h"

#include "ResourceFactory.h"

#include <GLFW/glfw3.h>
#include <cassert>
#include <stdexcept>

Window::Window(VkInstance context, std::string title, Size size): mContext(context) {

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    pWindow = glfwCreateWindow(size.width, size.height, title.c_str(), nullptr, nullptr);
	if (!pWindow)
        throw std::runtime_error("Failed to create GLFW window");

	mSurface = ResourceFactory::createSurface(mContext, pWindow);
}

Window::~Window() {
	assert(mContext != VK_NULL_HANDLE && "Context should be valid when destroying the window");
	assert(mSurface != VK_NULL_HANDLE && "Surface should be valid when destroying the window");
	
	vkDestroySurfaceKHR(mContext, mSurface, nullptr);
    glfwDestroyWindow(pWindow);
}

void Window::update() const { return glfwPollEvents(); }

bool Window::shouldClose() const { return glfwWindowShouldClose(pWindow); }

Size Window::size() const {
 	int width, height;
	glfwGetFramebufferSize(pWindow, &width, &height);
	return Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

GLFWwindow* Window::native() const { 
	return pWindow; 
}
