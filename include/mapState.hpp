#ifndef MAP_STATE_H
#define MAP_STATE_H

#include <chrono>

#include "game.hpp"
#include "graphics.hpp"
#include "renderProgram.hpp"
#include "renderBuffer.hpp"
#include "camera.hpp"
#include "window.hpp"
#include "audio.hpp"
#include "controller.hpp"

#define WIDTH 160
#define HEIGHT 60

using namespace chrono;

class MapState: public State {
public:
	time_point<steady_clock> stateStartTime;
	time_point<steady_clock> frameStartTime;

	uint32_t dt;
	uint32_t timeSinceInit;

	uint32_t ticksSinceLastDebugPrint;
	uint32_t debugPrintTimer;

	RenderProgram rp;
	RenderBuffer rb;
	Camera cam;
	Controller controller;
	AudioMixer mixer;

	Texture fontTexture;
	Texture wireTexture;

	Sprite fontSprite;
	Sprite wireSprite;

	uint8_t hWireTable[(WIDTH - 1) * HEIGHT];
	uint8_t vWireTable[(HEIGHT - 1) * WIDTH];

	/*AudioStream music;
	AudioStream move;
	AudioStream select;*/

	void load();
	void init(class Window* window, class Game* game);
	void update(Window* window, Game* game);
	void render(Window* window, Game* game);
	void unload();

	void handleEvent(SDL_Event* event);
};

#endif