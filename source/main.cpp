#include <string>
#include <iostream>
#include <fstream>

#include "main.hpp"
#include "window.hpp"
#include "audio.hpp"
#include "game.hpp"

// 3ds: osGetTime();
// pc: ???

using namespace std;

int main(int argc, char* args[]) {
	Window window;
	window.init(false, 1280, 1000, "Zilog Z80 Simulator");

	Game game;
	game.init(&window);

	while(game.running) {
		game.update();
		game.render();

		window.swap();
		SDL_Event event;
		while(window.pollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					game.running = false;
					break;
				default: game.handleEvent(&event);
			}
		}
	}

	SDL_Quit();

	return 0;
}