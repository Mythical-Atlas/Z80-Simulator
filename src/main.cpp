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
#include <set>

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

void initGraphics();
void mainLoop();
void exitGraphics();

int main() {
    initGraphics();

    mainLoop();

    exitGraphics();

    return 0;
}

void initGraphics() {
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

    // set vulkan application info parameters
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = WINDOW_TITLE;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Correll Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // set instance creation parameters
    VkInstanceCreateInfo instanceCreateInfo{};
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

    // set surface create info
    // windows specific - different for each platform
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
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
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[q];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        // add queue creation info to vector
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // we don't need any special features, so this is empty
    VkPhysicalDeviceFeatures deviceFeatures{};

    // create a vector to hold the extensions we plan to use (necessary in order to pass to device creation info)
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // set device create info (passes queue creation info and device features to device creation function)
    VkDeviceCreateInfo deviceCreateInfo{};
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
}

void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void exitGraphics() {
    // free vulkan resources
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    // free glfw resources
    glfwDestroyWindow(window);
    glfwTerminate();
}