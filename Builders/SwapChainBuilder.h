#pragma once

#include "Window.h"
#include "GPU.h"
#include "SwapChain.h"

class SwapChainBuilder {

public:

    SwapChain create(const Window& window, const GPU& gpu) const;

};
