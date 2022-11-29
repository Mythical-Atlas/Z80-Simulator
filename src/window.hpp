#ifndef WINDOW_H
#define WINDOW_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLM_FORCE_RADIANS
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <iostream>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <chrono>
#include <fstream>

#include "global.hpp"
#include "vertex.hpp"
#include "shader.hpp"
#include "windowVariables.hpp"
#include "commandBuffer.hpp"
#include "buffer.hpp"
#include "graphics.hpp"

#define WINDOW_START_WIDTH 1280
#define WINDOW_START_HEIGHT 720
#define WINDOW_TITLE "Z80 Simulator"
#define MAX_FRAMES_IN_FLIGHT 2

void initWindow();
int beginRenderingWindow();
void endRenderingWindow(uint32_t imageIndex);
void freeWindow();

void createSwapChain();
void recreateSwapChain();
void freeSwapChain();

void framebufferResizeCallback(GLFWwindow* window, int width, int height);
void updateUniformBuffer(uint32_t currentImage);

void initWindow() {
    /*--------------------INITIALIZATION--------------------*/
    
    // try to init glfw
    // throw error if glfw fails to init
    if(glfwInit()) {std::cout <<  "Successfully initialized GLFW" << std::endl;}
    else {
        std::cout << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    // tell glfw not to create opengl context since we're using vulkan
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // actually init window
    window = glfwCreateWindow(WINDOW_START_WIDTH, WINDOW_START_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    //glfwSetWindowUserPointer(window, this); // needed if inside of class
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    /*--------------------APP--------------------*/

    // set vulkan application info parameters
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = WINDOW_TITLE;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Correll Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    /*--------------------INSTANCE--------------------*/

    // set instance creation parameters
    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    // get glfw extensions for vulkan to interface with the window system
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // bind glfw extensions to vulkan instance creation
    instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
    instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;
    instanceCreateInfo.enabledLayerCount = 0;

    // actually create the vulkan instance
    // throw error if instance failed to init
    if(vkCreateInstance(&instanceCreateInfo, nullptr, &instance) == VK_SUCCESS) {std::cout <<  "Successfully created Vulkan instance" << std::endl;}
    else {
        std::cout << "Failed to create Vulkan instance" << std::endl;
        exit(-1);
    }

    /*--------------------SURFACE--------------------*/

    // set surface create info
    // windows specific - different for each platform
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hwnd = glfwGetWin32Window(window);
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

    // create vulkan surface
    // throw error if fails
    if(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface) == VK_SUCCESS) {std::cout <<  "Successfully created window surface (Vulkan)" << std::endl;}
    else {
        std::cout << "Failed to create window surface (Vulkan)" << std::endl;
        exit(-1);
    }

    // create glfw surface
    // throw error if fails
    if(glfwCreateWindowSurface(instance, window, nullptr, &surface) == VK_SUCCESS) {std::cout <<  "Successfully created window surface (GLFW)" << std::endl;}
    else {
        std::cout << "Failed to create window surface (GLFW)" << std::endl;
        exit(-1);
    }

    /*--------------------PHYSICAL DEVICE--------------------*/

    // count gpus that support vulkan
    physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    // throw an error if no gpus are found
    if(deviceCount >= 0) {std::cout <<  "Successfully found " << deviceCount << " Vulkan-enabled GPU(s)" << std::endl;}
    else {
        std::cout << "Failed to find a Vulkan-enabled GPU" << std::endl;
        exit(-1);
    }

    // load all vulkan-enabled devices into a vector
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // set the gpu that we'll use to the first one on the list by default
    physicalDevice = devices[0];

    // loop through the gpus and find one that is a discrete gpu (graphics card, not integrated)
    for(int d = 0; d < deviceCount; d++) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(devices[d], &deviceProperties);
        vkGetPhysicalDeviceFeatures(devices[d], &deviceFeatures);

        // use the first graphics card we find
        if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            std::cout <<  "Successfully found a Vulkan-enabled graphics card (will be preferred over integrated GPU)" << std::endl;
            physicalDevice = devices[d];
            break;
        }
    }

    /*--------------------QUEUE FAMILIES--------------------*/

    // get the number of queue families supported by the device
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    // store the queue families' information in a vector
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    // the datatype "Optional" doesn't have to exist
    // it has a value and a flag to determine whether it holds a value or not
    // this mimics std::optional in C++17
    
    // different queue families for graphics and presentation (although they could be the same)

    // loop through queue families and find one that supports VK_QUEUE_GRAPHICS_BIT
    for(int f = 0; f < queueFamilyCount; f++) {
        // check if presentation is supported
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, f, surface, &presentSupport);

        if(queueFamilies[f].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQFIndex.value = f;
            graphicsQFIndex.exists = true;
        }
        if(presentSupport) {
            presentQFIndex.value = f;
            presentQFIndex.exists = true;
        }

        if(graphicsQFIndex.exists && presentQFIndex.exists) {break;}
    }

    // if suitable queue families weren't found, throw an error
    if(graphicsQFIndex.exists && presentQFIndex.exists) {std::cout <<  "Successfully found queue families that support graphics and presentation" << std::endl;}
    else {
        if(!graphicsQFIndex.exists) {std::cout << "Failed to find a queue family that supports graphics" << std::endl;}
        if(!presentQFIndex.exists) {std::cout << "Failed to find a queue family that supports presentation" << std::endl;}
        exit(-1);
    }

    // create a vector to hold the queue families we plan to use
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::vector<uint32_t> uniqueQueueFamilies;
    
    // if the queues have the same family, just add the one index to the vector
    // otherwise add both
    if(graphicsQFIndex.value == presentQFIndex.value) {uniqueQueueFamilies = {graphicsQFIndex.value};}
    else {uniqueQueueFamilies = {graphicsQFIndex.value, presentQFIndex.value};}

    // used for scheduling command buffer execution from queues
    // required, even though useless since we only have 1 queue
    float queuePriority = 1.0f;

    // loop through queue families to create queues for
    for(int q = 0; q < uniqueQueueFamilies.size(); q++) {
        // set queue creation info (passes queue index to creation function)
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[q];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        // add queue creation info to vector
        queueCreateInfos.push_back(queueCreateInfo);
    }

    /*--------------------LOGICAL DEVICE--------------------*/

    // we don't need any special features, so this is empty
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    // create a vector to hold the extensions we plan to use (necessary in order to pass to device creation info)
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // set device create info (passes queue creation info and device features to device creation function)
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.enabledLayerCount = 0; // currently not using validation layers

    // actually create logical device
    // throw an error if creation fails
    if(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) == VK_SUCCESS) {std::cout <<  "Successfully created logical device" << std::endl;}
    else {
        std::cout << "Failed to create logical device" << std::endl;
        exit(-1);
    }

    // get queue handles for the queue families we're using
    // checks if the queues have the same family in order to avoid reading past the size of the vector holding the indices
    vkGetDeviceQueue(device, uniqueQueueFamilies[0], 0, &graphicsQueue);
    if(graphicsQFIndex.value == presentQFIndex.value) {vkGetDeviceQueue(device, uniqueQueueFamilies[0], 0, &presentQueue);}
    else {vkGetDeviceQueue(device, uniqueQueueFamilies[1], 0, &presentQueue);}

    /*--------------------SHADERS--------------------*/

    // open shader files
    std::vector<char> vertShaderCode = readFile("shaders/vert.spv");
    std::vector<char> fragShaderCode = readFile("shaders/frag.spv");

    // load the code into shader modules
    VkShaderModule vertShaderModule = createShaderModule(device, vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(device, fragShaderCode);

    // creation settings for vertex shader stage
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    // creation settings for fragment shader stage
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    // store the creation settings for the shader stages in an array
    // this is for the pipeline creation later
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    /*--------------------RENDER PASS--------------------*/

    // color attachment settings (multisampling, stenciling, image layout)
    // currently 1 sample and no stenciling
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // clear framebuffer to black before rendering
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // store resulting frame for display
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // color attachment reference settings
    // referenced by fragment shader
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // subpass settings
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // subpass dependency settings
    // used to to make image layout transitions occur at the correct times
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // render pass settings
    // passes color attachments and subpasses to the render pass
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    // try to create render pass
    if(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        std::cout << "Failed to create render pass" << std::endl;
        exit(-1);
    }

    createSwapChain();

    /*--------------------DESCRIPTOR SET LAYOUT--------------------*/

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        std::cout << "Failed to create descriptor set layout" << std::endl;
        exit(-1);
    }

    /*--------------------GRAPHICS PIPELINE--------------------*/

    // get the vertex binding and attribute info
    VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = Vertex::getAttributeDescriptions();

    // specifying the format of the vertex data
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    // specifying what type of geometry we plan to draw (lists of triangles)
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // specifies that we want to pass the viewport extent at draw time
    // this way we can modify it on the fly instead of it being static
    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // specifying number of viewports and scissors
    // if these weren't dynamic, they would be specified here
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // specifying rasterizer creation settings
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // specifying multisampling (antialiasing) settings
    // disabled for now
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // specifying color blend settings
    // currently off
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    // specifying color blend constants
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // pipeline layout settings
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    // try to create pipeline layout
    if(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        std::cout << "Failed to create pipeline layout" << std::endl;
        exit(-1);
    }

    // pipeline settings
    // passing shader stages to pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    // try to create graphics pipeline
    if(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        std::cout << "Failed to create graphics pipeline" << std::endl;
        exit(-1);
    }

    // free shader module resources (no longer needed now that pipeline has shader stages loaded)
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);

    /*--------------------COMMAND BUFFERS--------------------*/

    // command pool settings
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphicsQFIndex.value;

    // try to create command pool
    if(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        std::cout << "Failed to create command pool" << std::endl;
        exit(-1);
    }

    // command buffer settings and vector resizing
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    // try to allocate command buffer
    if(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        std::cout << "Failed to allocate command buffer(s)" << std::endl;
        exit(-1);
    }

    /*--------------------SYNCHRONIZATION--------------------*/

    // semaphore settings
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // fence settings
    // the fence is created in the signaled state so that the first frame
    // will not try to wait on the previous (non-existant) frame to finish
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    // loop through sync objects for each frame in flight
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if(
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS
        ) {
            std::cout << "Failed to create synchronization objects" << std::endl;
            exit(-1);
        }
    }

    // images
    createTextureImage();
    createTextureImageView();
    createTextureSampler();

    /*--------------------VERTEX BUFFER--------------------*/

    // staging buffer and memory
    // the staging buffer is what the cpu accesses, while the actual vertex buffer is only visible to the gpu
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    // create staging buffer
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    // write the vertex data to the vbo
    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    // create vertex buffer
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    // make the gpu copy the data from the staging buffer to the vertex buffer
    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    // free the staging buffer's resources
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    /*--------------------INDEX BUFFER--------------------*/

    // staging buffer and memory
    VkBuffer indStagingBuffer;
    VkDeviceMemory indStagingBufferMemory;

    // create staging buffer
    VkDeviceSize indBufferSize = sizeof(indices[0]) * indices.size();
    createBuffer(indBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indStagingBuffer, indStagingBufferMemory);

    // write the index data to the ibo
    void* indData;
    vkMapMemory(device, indStagingBufferMemory, 0, indBufferSize, 0, &indData);
    memcpy(indData, indices.data(), (size_t) indBufferSize);
    vkUnmapMemory(device, indStagingBufferMemory);

    // create index buffer
    createBuffer(indBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    // make the gpu copy the data from the staging buffer to the index buffer
    copyBuffer(indStagingBuffer, indexBuffer, indBufferSize);

    // free the staging buffer's resources
    vkDestroyBuffer(device, indStagingBuffer, nullptr);
    vkFreeMemory(device, indStagingBufferMemory, nullptr);

    /*--------------------UNIFORM BUFFERS--------------------*/

    VkDeviceSize unBufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(unBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

        vkMapMemory(device, uniformBuffersMemory[i], 0, unBufferSize, 0, &uniformBuffersMapped[i]);
    }

    /*--------------------DESCRIPTOR POOL--------------------*/

    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo descPoolInfo{};
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descPoolInfo.pPoolSizes = poolSizes.data();
    descPoolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(device, &descPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        std::cout << "Failed to create descriptor pool" << std::endl;
        exit(-1);
    }

    /*--------------------DESCRIPTOR SETS--------------------*/

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    VkDescriptorSetAllocateInfo setAllocInfo{};
    setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocInfo.descriptorPool = descriptorPool;
    setAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    setAllocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(device, &setAllocInfo, descriptorSets.data()) != VK_SUCCESS) {
        std::cout << "Failed to allcoate descriptor sets" << std::endl;
        exit(-1);
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureImageView;
        imageInfo.sampler = textureSampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

int beginRenderingWindow() {
    // wait for previous frame to finish so that semaphores and command buffers are available to use
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    // get an image from the swap chain
    // uses semaphore to make the gpu wait for an image to be available
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    // check if swap chain is out of date
    // recreate swap chain and skip this frame if it is
    if(result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return -1;
    }
    else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        std::cout << "Failed to aquire swap chain image" << std::endl;
        exit(-1);
    }

    // reset fence once finished waiting
    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    // reset the command buffer
    vkResetCommandBuffer(commandBuffers[currentFrame], 0);

    // start recording the command buffer using the image we got
    beginRecordingCommandBuffer(commandBuffers[currentFrame], imageIndex);

    updateUniformBuffer(currentFrame);

    // return the image index for the other rendering functions
    return imageIndex;
}
void endRenderingWindow(uint32_t imageIndex) {
    // record the command buffer using the image we got
    endRecordingCommandBuffer(commandBuffers[currentFrame]);

    // command buffer submission settings
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // try to submit the command buffer to the graphics queue
    // passes fence so that we can wait for this frame to finish before we draw the next one
    if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        std::cout << "Failed to submit draw command buffer" << std::endl;
        exit(-1);
    }

    // presentation settings
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    // submit request to present image to the swap chain
    VkResult result = result = vkQueuePresentKHR(presentQueue, &presentInfo);

    // check if swapchain is out of date
    // recreate swap chain and skip this frame if it is
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        recreateSwapChain();
        return;
    }
    else if(result != VK_SUCCESS) {
        std::cout << "Failed to present swap chain image" << std::endl;
        exit(-1);
    }

    // advance frame
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void freeWindow() {
    // free vulkan resources
    vkDestroySampler(device, textureSampler, nullptr);
    vkDestroyImageView(device, textureImageView, nullptr);
    vkDestroyImage(device, textureImage, nullptr);
    vkFreeMemory(device, textureImageMemory, nullptr);
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexBufferMemory, nullptr);
    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr);
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    freeSwapChain();
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    // free glfw resources
    glfwDestroyWindow(window);
    glfwTerminate();
}

void createSwapChain() {
    /*--------------------SWAP CHAIN--------------------*/

    // init variables to hold swap chain info
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    // get the swap chain's capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

    // get the formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if(formatCount != 0) {
        formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
    }

    // get the present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if(presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
    }

    // throw an error if there are no formats or present modes supported
    if(!formats.empty() && !presentModes.empty()) {std::cout <<  "Successfully fetched swap chain support" << std::endl;}
    else {
        std::cout << "Failed to fetch swap chain support" << std::endl;
        exit(-1);
    }

    // get the surface format to use
    // prefers SRGB, but otherwise uses the first format as default
    swapChainImageFormat = formats[0];
    for(int f = 0; f < formatCount; f++) {
        if(formats[f].format == VK_FORMAT_B8G8R8A8_SRGB && formats[f].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            swapChainImageFormat = formats[f];
            break;
        }
    }

    // get the present mode to use
    // prefers mailbox (triple buffering), otherwise uses FIFO as default (vsync)
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for(int m = 0; m < presentModeCount; m++) {
        if(presentModes[m] == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = presentModes[m];
            break;
        }
    }
    
    // gets the size of the window in pixels rather than screen coordinates
    // important for operating systems where the ui size is not 100% (i.e. a laptop with a high pixel density)
    int width;
    int height;
    glfwGetFramebufferSize(window, &width, &height);

    // sets the extent for vulkan
    swapChainExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    // clamps the window size so that it does not exceed bounds of implementation
    if(swapChainExtent.width < capabilities.minImageExtent.width) {swapChainExtent.width = capabilities.minImageExtent.width;}
    if(swapChainExtent.width > capabilities.maxImageExtent.width) {swapChainExtent.width = capabilities.maxImageExtent.width;}
    if(swapChainExtent.height < capabilities.minImageExtent.height) {swapChainExtent.height = capabilities.minImageExtent.height;}
    if(swapChainExtent.height > capabilities.maxImageExtent.height) {swapChainExtent.height = capabilities.maxImageExtent.height;}

    // sets the number of images to keep in the swap chain
    // there is a minimum required to function (it's not recommended to stay at minimum, so our minimum is that + 1)
    // 0 means there is no maximum, otherwise we would like to use the maximum if supported
    uint32_t imageCount = capabilities.minImageCount + 1;
    if(capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {imageCount = capabilities.maxImageCount;}

    // set the swap chain settings for creation
    VkSwapchainCreateInfoKHR swapCreateInfo = {};
    swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapCreateInfo.surface = surface;
    swapCreateInfo.minImageCount = imageCount;
    swapCreateInfo.imageFormat = swapChainImageFormat.format;
    swapCreateInfo.imageColorSpace = swapChainImageFormat.colorSpace;
    swapCreateInfo.imageExtent = swapChainExtent;
    swapCreateInfo.imageArrayLayers = 1;
    swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // use different image sharing mode depending on whether graphics and present queues are same family or not
    if (graphicsQFIndex.value != presentQFIndex.value) {
        // redefining the earlier vector as an array to pass to swap creation (bad practice but oh well...)
        uint32_t queueFamilyIndices[2] = {graphicsQFIndex.value, presentQFIndex.value};

        // concurrent better for 2 separate families to avoid worrying about transfering ownership
        swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapCreateInfo.queueFamilyIndexCount = 2;
        swapCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        // exclusive better for same family because concurrent requires at least 2 families
        swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapCreateInfo.queueFamilyIndexCount = 0;
        swapCreateInfo.pQueueFamilyIndices = nullptr;
    }

    // more swap chain settings
    swapCreateInfo.preTransform = capabilities.currentTransform;
    swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapCreateInfo.presentMode = presentMode;
    swapCreateInfo.clipped = VK_TRUE;
    swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    // create swap chain and throw error if fails
    if(vkCreateSwapchainKHR(device, &swapCreateInfo, nullptr, &swapChain) == VK_SUCCESS) {std::cout <<  "Successfully created swap chain" << std::endl;}
    else {
        std::cout << "Failed to create swap chain" << std::endl;
        exit(-1);
    }

    // get swap chain image handles
    // image count in swap chain may be larger than the minimum we specified earlier, so we pull it from the newly created swap chain
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    /*--------------------IMAGE VIEWS--------------------*/

    // init a vector for the swap chain image views and loop through them
    swapChainImageViews.resize(swapChainImages.size());
    for(int i = 0; i < swapChainImages.size(); i++) {
        // set the creation info for the current image view in the loop
       /* VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = swapChainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = swapChainImageFormat.format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        // create the image view and throw an error if it fails
        if(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
            std::cout << "Failed to create swap chain image view: " << i << std::endl;
            exit(-1);
        }*/

        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat.format);
    }

    std::cout <<  "Successfully created " << swapChainImages.size() << " swap chain image views" << std::endl;

    /*--------------------FRAMEBUFFERS--------------------*/

    // resize container to hold framebuffers
    swapChainFramebuffers.resize(swapChainImageViews.size());

    // loop through image views and create a framebuffer for each one
    for (int i = 0; i < swapChainImageViews.size(); i++) {
        // framebuffer creation settings
        VkImageView attachments[] = {swapChainImageViews[i]};
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        // try to create framebuffer
        if(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            std::cout << "Failed to create framebuffer" << std::endl;
            exit(-1);
        }
    }
}

void recreateSwapChain() {
    // get the size of the frame buffer
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    // if the window is minimized (the size of the frame buffer will be 0), pause execution
    while(width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    // wait for the swap chain to not be in use
    vkDeviceWaitIdle(device);

    // free resources
    freeSwapChain();

    // recreate
    createSwapChain();
}

void freeSwapChain() {
    // free swapchain resources
    for(int i = 0; i < swapChainFramebuffers.size(); i++) {vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);}
    for(int i = 0; i < swapChainImageViews.size(); i++) {vkDestroyImageView(device, swapChainImageViews[i], nullptr);}
    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

void framebufferResizeCallback(GLFWwindow* window, int width, int height) {framebufferResized = true;}

void updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

#endif