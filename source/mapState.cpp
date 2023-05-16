#include <string>
#include <iostream>

#include "windows.h"
#include "psapi.h"

#include "mapState.hpp"
#include "audio.hpp"

#define VERTS_SIZE 16

using namespace std;

void MapState::load()  {
	rp = RenderProgram("Sprite Program");
    rp.attachShader(GL_VERTEX_SHADER, "resources/versatileShader.vert");
    rp.attachShader(GL_FRAGMENT_SHADER, "resources/versatileShader.frag");
    rp.link();

	fontTexture.load("resources/font.png");

    fontSprite.init(&fontTexture, 0, 0, 0, 8, 12); // 512x144

    int attribSizes[2] = {2, 2};
    rb = RenderBuffer(2, attribSizes, 1 * VERTS_SIZE);

    float vertDataBuffer[16];
    fontSprite.getData(vertDataBuffer); rb.uploadData(0 * VERTS_SIZE, VERTS_SIZE, vertDataBuffer);
    /*mapsprite.getData(vertDataBuffer);   rb.uploadData(1 * VERTS_SIZE, VERTS_SIZE, vertDataBuffer);
    fernsprite.getData(vertDataBuffer);  rb.uploadData(2 * VERTS_SIZE, VERTS_SIZE, vertDataBuffer);
    iconsprite.getData(vertDataBuffer);  rb.uploadData(3 * VERTS_SIZE, VERTS_SIZE, vertDataBuffer);*/

    /*music = AudioStream("romfs/shortMusic.raw", SDL_MIX_MAXVOLUME, true);
	jump = AudioStream("romfs/grow.raw", SDL_MIX_MAXVOLUME, false);
	coin = AudioStream("romfs/coin.raw", SDL_MIX_MAXVOLUME, false);
	death = AudioStream("romfs/death.raw", SDL_MIX_MAXVOLUME, false);*/
}
void MapState::init(Window* window, Game* game)  {
    cam.init((int)window->getScreenSize().x, (int)window->getScreenSize().y);

    mixer.init();
    memset(&controller, 0, sizeof(Controller)); // could be done with a union in Controller

	/*mixer.addStream(&music);
	mixer.addStream(&jump);
	mixer.addStream(&coin);
	mixer.addStream(&death);

    music.start();*/

    stateStartTime = steady_clock::now();
    frameStartTime = stateStartTime;

    debugPrintTimer = 0;
}
void MapState::update(Window* window, Game* game)  {
    time_point<steady_clock> currentTime = steady_clock::now();
    dt = duration_cast<milliseconds>(currentTime - frameStartTime).count();
    frameStartTime = currentTime;
    timeSinceInit = duration_cast<milliseconds>(currentTime - stateStartTime).count();

    ticksSinceLastDebugPrint++;
}

void renderString(Sprite font, vec2 pos, RenderProgram* rp, RenderBuffer* rb, string text) {
    const char* chars = text.c_str();
    int len = text.length();
    int line = 0;

    for(int i = 0; i < len; i++) {
        if(chars[i] == '\n') {line++;}
        else if(chars[i] >= 'A' && chars[i] <= 'Z') {
            font.pos = pos + vec2(i * 8, line * 12);
            font.render(rp, rb, chars[i] - 'A' + 33, 0);
        }
        else if(chars[i] >= 'a' && chars[i] <= 'z') {
            font.pos = pos + vec2(i * 8, line * 12);
            font.render(rp, rb, chars[i] - 'a' + 1, 1);
        }
        else if(chars[i] >= '0' && chars[i] <= '9') {
            font.pos = pos + vec2(i * 8, line * 12);
            font.render(rp, rb, chars[i] - '0' + 16, 0);
        }
    }
}

void MapState::render(Window* window, Game* game)  { // TODO: layering using z position
    if(timeSinceInit - debugPrintTimer >= 1000) {
        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;

        float tickRate = (float)ticksSinceLastDebugPrint / (timeSinceInit - debugPrintTimer) * 1000.0f;

        cout << "Virtual memory used: " << virtualMemUsedByMe << " bytes" << endl;
        cout << "Tick rate: " << tickRate << " fps" << endl;

        ticksSinceLastDebugPrint = 0;
        while(timeSinceInit - debugPrintTimer >= 1000) {debugPrintTimer += 1000;}
    }

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    rp.useViewMatrix(&cam);

    renderString(fontSprite, vec2(20, 20), &rp, &rb, "HELLO WORLD");
}
void MapState::unload() {
	mixer.unload();
}

void MapState::handleEvent(SDL_Event* event) {controller.handleEvent(event);}