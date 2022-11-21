/*
Zilog Z80 simulator
Designed and programmed by Ben Correll
Last updated: 11-16-2022
*/

#include "window.hpp"

int main() {
    // setup rendering objects
    initWindow();

    // loop until the window closes
    while(!glfwWindowShouldClose(window)) {
        // check for window events
        glfwPollEvents();

        // begin the rendering/recording process
        int imageIndex = beginRenderingWindow();

        // skip rendering this frame if the framebuffer was recreated
        if(imageIndex != -1) {
            // bind the vbo
            VkBuffer vertexBuffers[] = {vertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffers[currentFrame], 0, 1, vertexBuffers, offsets);

            // draw the vertices in the vbo
            vkCmdDraw(commandBuffers[currentFrame], static_cast<uint32_t>(vertices.size()), 1, 0, 0);

            endRenderingWindow(imageIndex);
        }
    }

    // wait for logical device to finish operations before we destroy the window
    vkDeviceWaitIdle(device);

    // free glfw/vulkan resources
    freeWindow();

    return 0;
}