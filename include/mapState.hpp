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
#include "wires.hpp"

using namespace chrono;

class MapState: public State {
public:
	time_point<steady_clock> stateStartTime;
	time_point<steady_clock> frameStartTime;

	uint32_t dt;
	uint32_t timeSinceInit;

	uint32_t ticksSinceLastDebugPrint;
	uint32_t debugPrintTimer;

	uint32_t selectStartTime;

	RenderProgram rp;
	RenderBuffer rb;
	Camera cam;
	Controller controller;
	AudioMixer mixer;

	Texture fontTexture;
	Texture wireTexture;

	Sprite fontSprite;
	Sprite wireSprite;

	WireTable wt;

	bool canClock;
	bool canSelectUp;
	bool canSelectDown;
	bool canModify;

	int selectedWire;
	int selectedValue;

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