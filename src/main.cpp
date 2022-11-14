#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

int main() {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Pong";
    appInfo.pEngineName = "Ponggine";

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

    VkInstance instance;
    VkResult result = vkCreateInstance(&instanceInfo, 0, &instance);

    if(result == VK_SUCCESS) {std::cout <<  "Successfully created Vulkan instance" << std::endl;}
    else {
        std::cout << "Failed to create Vulkan instance" << std::endl;
        return -1;
    }

    if(glfwInit()) {std::cout <<  "Successfully initialized GLFW" << std::endl;}
    else {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }


    return 0;
}