#ifndef SHADER_H
#define SHADER_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <vector>
#include <iostream>

VkShaderModule createShaderModule(VkDevice device, std::vector<char> code) {
    // settings for creating the shader module
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    // try to create the shader module, throw error if it fails
    VkShaderModule shaderModule;
    if(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        std::cout << "Failed to create shader module" << std::endl;
        exit(-1);
    }

    return shaderModule;
}

#endif