@echo off

C:/VulkanSDK/1.3.231.1/Bin/glslc.exe shaders/shader.vert -o shaders/vert.spv
C:/VulkanSDK/1.3.231.1/Bin/glslc.exe shaders/shader.frag -o shaders/frag.spv

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

SET includes=/Isrc /I%VULKAN_SDK%/Include /Iinclude
SET links=/link /LIBPATH:%VULKAN_SDK%/Lib vulkan-1.lib /LIBPATH:lib glfw3dll.lib
SET defines=/D DEBUG

echo "Building main..."

cl /EHsc %includes% %defines% src/main.cpp %links%