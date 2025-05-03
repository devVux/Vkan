#pragma once

#include <string>
#include <cstdint>

#include "Common.h"

struct GLFWwindow;

class Window {

public:

	Window(std::string title, Size size = { 800, 600 });
	~Window();

	void update() const;
	bool shouldClose() const;

	Size size() const;

	GLFWwindow* native() const;


private:

	GLFWwindow* pWindow;

};

