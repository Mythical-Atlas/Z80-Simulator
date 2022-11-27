#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "windowVariables.hpp"

void beginRecordingCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
void endRecordingCommandBuffer(VkCommandBuffer commandBuffer);
VkCommandBuffer beginSingleTimeCommands();
void endSingleTimeCommands(VkCommandBuffer commandBuffer);

void beginRecordingCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    // used for passing flags to command buffer
    // currently no flags set
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    // try to begin recording command buffer
    if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        std::cout << "Failed to begin recording command buffer" << std::endl;
        exit(-1);
    }

    // passing framebuffer and other info for render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;
    renderPassInfo.clearValueCount = 1;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.pClearValues = &clearColor;

    // begin the render pass
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // bind the graphics pipeline to the command buffer
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    // define viewport for draw command
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // define scissor for draw command
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
}
void endRecordingCommandBuffer(VkCommandBuffer commandBuffer) {
    // end the render pass
    vkCmdEndRenderPass(commandBuffer);

    // try to finish recording command buffer
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        std::cout << "Failed to record command buffer" << std::endl;
        exit(-1);
    }
}

VkCommandBuffer beginSingleTimeCommands() {
    // buffer allocation settings
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    // allocate a temporary command buffer for gpu memory transfer operations
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    // begin recording the command buffer
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}
void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    // end recording command buffer
    vkEndCommandBuffer(commandBuffer);

    // submit the command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

    // make the cpu wait until the operation finishes executing before continuing
    vkQueueWaitIdle(graphicsQueue);

    // free the command buffer's resources
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

#endif