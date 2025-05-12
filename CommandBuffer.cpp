#include "CommandBuffer.h"

#include "GPU.h"

void PrimaryCommandBuffer::submit(VkQueue queue, std::vector<VkSemaphore> waitSemaphores, std::vector<VkSemaphore> signalSemaphores, const Fence& fence) const {
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommands;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
	submitInfo.pWaitSemaphores = waitSemaphores.data();
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
	submitInfo.pSignalSemaphores = signalSemaphores.data();

    if (vkQueueSubmit(queue, 1, &submitInfo, fence) != VK_SUCCESS)
        throw std::runtime_error("failed to submit command buffer!");
}

void PrimaryCommandBuffer::beginRenderPass(const RenderPass& renderpass, const VkFramebuffer& framebuffer, VkExtent2D extent) {
	VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderpass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

	VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(mCommands, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
}


void PrimaryCommandBuffer::endRenderPass(const RenderPass & renderpass) {
	vkCmdEndRenderPass(mCommands);
}

void PrimaryCommandBuffer::reset() {
    vkResetCommandBuffer(mCommands, /*VkCommandBufferResetFlagBits*/ 0);
}

void PrimaryCommandBuffer::begin() {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(mCommands, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("failed to begin command buffer!");
}

void PrimaryCommandBuffer::end() {
	if (vkEndCommandBuffer(mCommands) != VK_SUCCESS)
        throw std::runtime_error("failed to end command buffer!");
} 






void SecondaryCommandBuffer::bindGraphicsPipeline(const GraphicsPipeline& pipeline) {
	vkCmdBindPipeline(mCommands, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void SecondaryCommandBuffer::exec(const PrimaryCommandBuffer& primaryCommands) const {
	vkCmdExecuteCommands(primaryCommands, 1, &mCommands);
}

void SecondaryCommandBuffer::begin(const RenderPass& renderpass, const Framebuffer& framebuffer, uint32_t subpassIndex) {
	VkCommandBufferInheritanceInfo inheritanceInfo{};
	inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	inheritanceInfo.renderPass = renderpass;
	inheritanceInfo.subpass = subpassIndex;
	inheritanceInfo.framebuffer = framebuffer;

	VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; // Execute within render pass
	beginInfo.pInheritanceInfo = &inheritanceInfo;

    if (vkBeginCommandBuffer(mCommands, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("failed to begin command buffer!");
}

void SecondaryCommandBuffer::end() {
	if (vkEndCommandBuffer(mCommands) != VK_SUCCESS)
        throw std::runtime_error("failed to end command buffer!");
} 
