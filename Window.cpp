#include "Window.h"

#include <GLFW/glfw3.h>

Window::Window(std::string title, Size size) {

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


    pWindow = glfwCreateWindow(size.width, size.height, title.c_str(), nullptr, nullptr);

}

Window::~Window() {
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
