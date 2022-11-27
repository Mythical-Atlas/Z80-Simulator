#ifndef MEMORY_H
#define MEMORY_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "windowVariables.hpp"

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    // find a suitable memory type
    Optional memoryType;
    for(int i = 0; i < memProperties.memoryTypeCount; i++) {
        // check if memory type is suitable (type bits) and check if we can map the memory so it's visible to the cpu (properties flags)
        if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            memoryType.value = i;
            memoryType.exists = true;
        }
    }

    // throw an error if a memory type wasn't found
    if(!memoryType.exists) {
        std::cout << "Failed to find suitable memory type" << std::endl;
        exit(-1);
    }

    return memoryType.value;
}

#endif