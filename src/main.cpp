/*
Zilog Z80 simulator
Designed and programmed by Ben Correll
Last updated: 11-14-2022
*/

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <iostream>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <fstream>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_TITLE "Z80 Simulator"

typedef struct {uint32_t value; bool exists;} Optional;

GLFWwindow* window;
VkInstance instance;
VkDevice device;
VkSurfaceKHR surface;
VkQueue graphicsQueue;
VkQueue presentQueue;
VkFormat swapChainImageFormat;
VkExtent2D swapChainExtent;
VkSwapchainKHR swapChain;
std::vector<VkImage> swapChainImages;
std::vector<VkImageView> swapChainImageViews;

void initGraphics();
void mainLoop();
void exitGraphics();

VkShaderModule createShaderModule(std::vector<char> code);

static std::vector<char> readFile(std::string filename);

int main() {
    initGraphics();

    mainLoop();

    exitGraphics();

    return 0;
}

void initGraphics() {
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // actually init window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);

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

    /*--------------------PHYSICAL DEVICE--------------------*/

    // count gpus that support vulkan
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
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
    Optional graphicsQFIndex;
    Optional presentQFIndex;

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
    if(graphicsQFIndex.value == presentQFIndex.value) {vkGetDeviceQueue(device, uniqueQueueFamilies[1], 0, &presentQueue);}
    else {vkGetDeviceQueue(device, uniqueQueueFamilies[1], 0, &presentQueue);}

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
    VkSurfaceFormatKHR swapChainImageFormat = formats[0];
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
    VkExtent2D swapChainExtent = {
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

    // init a vector for the swap chain image views and loop through them
    swapChainImageViews.resize(swapChainImages.size());
    for(int i = 0; i < swapChainImages.size(); i++) {
        // set the creation info for the current image view in the loop
        VkImageViewCreateInfo imageViewCreateInfo{};
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
        }
    }

    std::cout <<  "Successfully created " << swapChainImages.size() << " swap chain image views" << std::endl;

    /*--------------------SHADERS--------------------*/

    // open shader files
    std::vector<char> vertShaderCode = readFile("shaders/vert.spv");
    std::vector<char> fragShaderCode = readFile("shaders/frag.spv");

    // load the code into shader modules
    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

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








    // free shader module resources (no longer needed now that .......)
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void exitGraphics() {
    // free vulkan resources
    for (int i = 0; i < swapChainImageViews.size(); i++) {vkDestroyImageView(device, swapChainImageViews[i], nullptr);}
    vkDestroySwapchainKHR(device, swapChain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    // free glfw resources
    glfwDestroyWindow(window);
    glfwTerminate();
}

VkShaderModule createShaderModule(std::vector<char> code) {
    // settings for creating the shader module
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    // try to create the shader module, throw error if it fails
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        std::cout << "Failed to create shader module" << std::endl;
        exit(-1);
    }

    return shaderModule;
}

static std::vector<char> readFile(std::string filePath) {
    // open file, throw error if fails
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if(!file.is_open()) {
        std::cout << "Failed to open file: " << filePath << std::endl;
        exit(-1);
    }

    // create vector to hold data (seeks to end of file stream to get size of file)
    uint32_t fileSize = (uint32_t)file.tellg();
    std::vector<char> buffer(fileSize);

    // go to beginning of file and read data
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}