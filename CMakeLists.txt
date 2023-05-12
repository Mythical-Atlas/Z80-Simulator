cmake_minimum_required(VERSION 3.24.2)
project(z80)

set(CMAKE_CXX_STANDARD 17)

add_executable(z80
	include/animation.hpp 		source/animation.cpp
	include/assetManager.hpp 	source/assetManager.cpp
	include/audio.hpp
	include/camera.hpp 			source/camera.cpp
	include/collision.hpp
	include/controller.hpp		source/controller.cpp
	include/game.hpp 			source/game.cpp
	include/graphics.hpp 		source/graphics.cpp
	include/levelState.hpp 		source/levelState.cpp
	include/main.hpp 			source/main.cpp
	include/mapState.hpp		source/mapState.cpp
	include/menuState.hpp 		source/menuState.cpp
	include/renderBuffer.hpp 	source/renderBuffer.cpp
	include/renderProgram.hpp 	source/renderProgram.cpp
	include/window.hpp 			source/window.cpp
	dependencies/glad/src/glad.c
)

set_target_properties(z80 PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ../ RUNTIME_OUTPUT_DIRECTORY_RELEASE ../)

target_include_directories(z80 PRIVATE include PUBLIC dependencies PUBLIC dependencies/glm-0.9.9.8/glm PUBLIC dependencies/glad/include PUBLIC dependencies/SDL2-2.26.4/include PUBLIC dependencies/rapidjson-1.1.0/include)
target_link_directories(z80 PUBLIC dependencies/SDL2-2.26.4/lib)
target_link_libraries(z80 SDL2main SDL2 psapi)

#target_compile_definitions(z80 PUBLIC USE_OPENGL_4_3)