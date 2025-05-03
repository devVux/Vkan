#pragma once

#include <vulkan/vulkan.h>

#include "Fence.h"
#include "Common.h"

class CommandBuffer {
	friend class GPU;

public:
	
    void submit(VkQueue queue, Fence fence) const;

	void begin();
    void end();
    void reset();

	operator VkCommandBuffer() const { return mCommands; }

private:

	CommandBuffer(VkCommandBuffer commands): mCommands(std::move(commands)) { }

private:

	VkCommandBuffer mCommands;

};

