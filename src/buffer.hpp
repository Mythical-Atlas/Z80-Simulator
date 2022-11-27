#ifndef BUFFER_H
#define BUFFER_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "windowVariables.hpp"
#include "commandBuffer.hpp"
#include "memory.hpp"

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    // send the command to copy the source buffer to the destination buffer
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    // buffer settings
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // try to create buffer
    if(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        std::cout << "Failed to create buffer" << std::endl;
        exit(-1);
    }

    // getting the memory requirements for the buffer
    // fields:
    // - size
    // - alignment
    // - suitable memory types
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    // buffer memory allocation info
    VkMemoryAllocateInfo memAllocInfo{};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = memRequirements.size;
    memAllocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);;

    // try to allocate memory
    if(vkAllocateMemory(device, &memAllocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        std::cout << "Failed to allocate buffer memory" << std::endl;
        exit(-1);
    }

    // bind the memory we just allocated to the buffer
    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

#endif