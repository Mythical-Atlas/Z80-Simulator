#ifndef WINDOW_VARIABLES_H
#define WINDOW_VARIABLES_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <vector>

#include "global.hpp"

GLFWwindow* window;
VkInstance instance;
VkSurfaceKHR surface;
VkPhysicalDevice physicalDevice;
VkDevice device;
Optional graphicsQFIndex;
Optional presentQFIndex;
VkQueue graphicsQueue;
VkQueue presentQueue;
VkSurfaceFormatKHR swapChainImageFormat;
VkExtent2D swapChainExtent;
VkSwapchainKHR swapChain;
std::vector<VkImage> swapChainImages;
std::vector<VkImageView> swapChainImageViews;
VkRenderPass renderPass;
VkPipelineLayout pipelineLayout;
VkPipeline graphicsPipeline;
std::vector<VkFramebuffer> swapChainFramebuffers;
VkCommandPool commandPool;
std::vector<VkCommandBuffer> commandBuffers;
std::vector<VkSemaphore> imageAvailableSemaphores;
std::vector<VkSemaphore> renderFinishedSemaphores;
std::vector<VkFence> inFlightFences;
VkBuffer vertexBuffer;
VkDeviceMemory vertexBufferMemory;
VkBuffer indexBuffer;
VkDeviceMemory indexBufferMemory;
VkDescriptorSetLayout descriptorSetLayout;
std::vector<VkBuffer> uniformBuffers;
std::vector<VkDeviceMemory> uniformBuffersMemory;
std::vector<void*> uniformBuffersMapped;
VkDescriptorPool descriptorPool;
std::vector<VkDescriptorSet> descriptorSets;
VkImage textureImage;
VkDeviceMemory textureImageMemory;
VkImageView textureImageView;
VkSampler textureSampler;

uint32_t currentFrame = 0;
bool framebufferResized = false;

#endif