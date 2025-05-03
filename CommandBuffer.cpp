#include "CommandBuffer.h"

#include "GPU.h"
#include <stdexcept>

void CommandBuffer::submit(VkQueue queue, Fence fence) const {
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommands;

    if (vkQueueSubmit(queue, 1, &submitInfo, fence) != VK_SUCCESS)
        throw std::runtime_error("failed to submit command buffer!");
}

void CommandBuffer::reset() {
    vkResetCommandBuffer(mCommands, /*VkCommandBufferResetFlagBits*/ 0);
}

void CommandBuffer::begin() {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(mCommands, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("failed to begin command buffer!");
}

void CommandBuffer::end() {
	if (vkEndCommandBuffer(mCommands) != VK_SUCCESS)
        throw std::runtime_error("failed to end command buffer!");
}