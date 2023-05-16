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

    memset(wireTable, 0, 160 * 60);
}
void MapState::update(Window* window, Game* game)  {
    time_point<steady_clock> currentTime = steady_clock::now();
    dt = duration_cast<milliseconds>(currentTime - frameStartTime).count();
    frameStartTime = currentTime;
    timeSinceInit = duration_cast<milliseconds>(currentTime - stateStartTime).count();

    ticksSinceLastDebugPrint++;
}

void renderString(Sprite font, vec2 pos, string text, RenderProgram* rp, RenderBuffer* rb) {
    int line = 0;
    int offset = 0;
    const char* chars = text.c_str();

    for(int c = 0; c < text.length(); c++) {
        font.pos = pos + vec2((c - offset) * 8, line * 12);

        if(chars[c] == '\n') {
            line++;
            offset = c + 1;
        }
        else if(chars[c] == 0) {}
        else {font.render(rp, rb, (chars[c] - 32) % 64, (int)((chars[c] - 32) / 64));}
    }
}

void renderRaw(Sprite font, vec2 pos, int* text, int numInts, RenderProgram* rp, RenderBuffer* rb) {
    int line = 0;
    int offset = 0;

    for(int c = 0; c < numInts; c++) {
        font.pos = pos + vec2((c - offset) * 8, line * 12);
        
        if(text[c] == '\n') {
            line++;
            offset = c + 1;
        }
        else if(text[c] == 0) {}
        else {font.render(rp, rb, (text[c] - 32) % 64, (int)((text[c] - 32) / 64));}
    }
}
int buildBorder(int w, int h, int s, int* buffer, int len) {
    for(int x = 0; x < w + 1; x++) {
        for(int y = 0; y < h; y++) {
            if(x + y * (w + 1) >= len) {return len;}
            if(x == w) {
                buffer[x + y * (w + 1)] = '\n';
                continue;
            }

            int xa = 0;
            if(x == 0) {xa = -1;}
            if(x == w - 1) {xa = 1;}
            int ya = 0;
            if(y == 0) {ya = -1;}
            if(y == h - 1) {ya = 1;}

            int i = 0;

            if(xa == -1 && ya == -1) { // top left
                if(s == 1) {i = 704;}
                if(s == 2) {i = 717;}
                buffer[x + y * (w + 1)] = i;
            }
            if(xa == 1 && ya == -1) { // top right
                if(s == 1) {i = 705;}
                if(s == 2) {i = 720;}
                buffer[x + y * (w + 1)] = i;
            }
            if(xa == -1 && ya == 1) { // bottom left
                if(s == 1) {i = 706;}
                if(s == 2) {i = 723;}
                buffer[x + y * (w + 1)] = i;
            }
            if(xa == 1 && ya == 1) { // bottom right
                if(s == 1) {i = 707;}
                if(s == 2) {i = 726;}
                buffer[x + y * (w + 1)] = i;
            }
            
            if((xa == -1 || xa == 1) && ya == 0) {
                if(s == 1) {i = 703;}
                if(s == 2) {i = 714;}
                buffer[x + y * (w + 1)] = i;
            }
            if((ya == -1 || ya == 1) && xa == 0) {
                if(s == 1) {i = 702;}
                if(s == 2) {i = 713;}
                buffer[x + y * (w + 1)] = i;
            }
        }
    }

    return w + h * (w + 1);
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

    int border[512];
    int borderLen = 0;

    renderString(fontSprite, vec2(8, 12), 
        "A11       A10\n"
        "A12        A9\n"
        "A13        A8\n"
        "A14        A7\n"
        "A15        A6\n"
        "CLK        A5\n"
        "D4         A4\n"
        "D3         A3\n"
        "D5         A2\n"
        "D6         A1\n"
        "+5V        A0\n"
        "D2        GND\n"
        "D7      /RFSH\n"
        "D0        /M1\n"
        "D1     /RESET\n"
        "/INT   /BUSRQ\n"
        "/NMI    /WAIT\n"
        "/HALT /BUSACK\n"
        "/MREQ     /WR\n"
        "/IORQ     /RD\n"
    , &rp, &rb);

    renderString(fontSprite, vec2(8 * 20, 12), 
        "      +5V\n"
        "A12   /WE\n"
        "A7       \n"
        "A6     A8\n"
        "A5     A9\n"
        "A4    A11\n"
        "A3    /OE\n"
        "A2    A10\n"
        "A1    /CE\n"
        "A0   I/O7\n"
        "I/O0 I/O6\n"
        "I/O1 I/O5\n"
        "I/O2 I/O4\n"
        "GND  I/O3\n"
    , &rp, &rb);

    renderString(fontSprite, vec2(8 * 40, 12), 
        "1A  VCC\n"
        "1Y   6A\n"
        "2A   6Y\n"
        "2Y   5A\n"
        "3A   5Y\n"
        "3Y   4A\n"
        "GND  4Y\n"
    , &rp, &rb);

    memset(border, 0, 512 * sizeof(int));
    borderLen = buildBorder(15, 22, 1, border, 512);
    renderRaw(fontSprite, vec2(0, 0), border, 15, &rp, &rb);
    renderRaw(fontSprite, vec2(0, 12 * 21), &(border[16 * 21]), 15, &rp, &rb);

    memset(border, 0, 512 * sizeof(int));
    for(int i = 0; i < 20; i++) {
        border[i * 16 + 0] = 709;
        border[i * 16 + 14] = 708;
        border[i * 16 + 15] = '\n';
    }
    renderRaw(fontSprite, vec2(0, 12), border, 20 * 16, &rp, &rb);
}
void MapState::unload() {
	mixer.unload();
}

void MapState::handleEvent(SDL_Event* event) {controller.handleEvent(event);}