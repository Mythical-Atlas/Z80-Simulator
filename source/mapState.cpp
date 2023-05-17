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
    wireTexture.load("resources/wires.png");

    fontSprite.init(&fontTexture, 0, 0, 0, 8, 12); // 512x144
    wireSprite.init(&wireTexture, 1, 0, 0, 8, 12); // 112x48

    int attribSizes[2] = {2, 2};
    rb = RenderBuffer(2, attribSizes, 2 * VERTS_SIZE);

    float vertDataBuffer[VERTS_SIZE];
    fontSprite.getData(vertDataBuffer); rb.uploadData(0 * VERTS_SIZE, VERTS_SIZE, vertDataBuffer);
    wireSprite.getData(vertDataBuffer); rb.uploadData(1 * VERTS_SIZE, VERTS_SIZE, vertDataBuffer);

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
    //cam.pos = vec2(window->getScreenSize().x / -4, window->getScreenSize().y / -4);
    //cam.scale = vec2(2, 2);

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

    memset(hWireTable, 0, H_SIZE);
    memset(vWireTable, 0, V_SIZE);

    hWireTable[1] = 1; hWireTable[2] = 1;

    vWireTable[4] = 2; vWireTable[5] = 2;
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

void renderWireSquare(Sprite wireSprite, vec2 pos, int l, int r, int t, int b, RenderProgram* rp, RenderBuffer* rb) {
    wireSprite.pos = pos;

    int x = -1;
    int y = -1;

    // conditions not handled:
    // - 3 different wires in one square
    // - top left one color, bottom right a different color
    // - top right vs. bottom left

    if(l == r && t == b && l != 0 && t != 0) { // cross
        if(t == 1) {y = 0;} // v wire is white
        if(t == 2) {y = 1;} // v wire is red
        if(t == 3) {y = 2;} // v wire is green
        if(l == 1) {x = 11;} // h wire is white
        if(l == 2) {x = 12;} // h wire is red
        if(l == 3) {x = 13;} // h wire is green
    }
    else if(l == r && l != 0 && t == 0 && b == 0) { // horizontal wire
        x = 1;
        if(l == 1) {y = 0;} // wire is white
        if(l == 2) {y = 1;} // wire is red
        if(l == 3) {y = 2;} // wire is green
    }
    else if(l == r && t != 0 && b == 0) { // upside down T
        x = 10;
        if(l == 1) {// wire is white
            y = 0;
            if(b == 1) { // red bottom pin of chip
                x = 9;
                y = 3;
            }
            if(b == 2) { // green bottom pin of chip
                x = 5;
                y = 3;
            }
        }
        if(l == 2) {y = 1;} // wire is red
        if(l == 3) {y = 2;} // wire is green
    }
    else if(l == r && t == 0 && b != 0) { // T
        x = 9;
        if(l == 1) {// wire is white
            y = 0;
            if(t == 1) { // red top pin of chip
                x = 10;
                y = 3;
            }
            if(t == 2) { // green top pin of chip
                x = 6;
                y = 3;
            }
        }
        if(l == 2) {y = 1;} // wire is red
        if(l == 3) {y = 2;} // wire is green
    }
    else if(t == b && t != 0 && l == 0 && r == 0) { // vertical wire
        x = 2;
        if(t == 1) {y = 0;} // wire is white
        if(t == 2) {y = 1;} // wire is red
        if(t == 3) {y = 2;} // wire is green
    }
    else if(t == b && l != 0 && r == 0) { // vertical with left
        x = 8;
        if(t == 1) {// wire is white
            y = 0;
            if(l == 1) { // red left pin of chip
                x = 8;
                y = 3;
            }
            if(l == 2) { // green left pin of chip
                x = 4;
                y = 3;
            }
        }
        if(t == 2) {y = 1;} // wire is red
        if(t == 3) {y = 2;} // wire is green
    }
    else if(t == b && l == 0 && r != 0) { // vertical with right
        x = 7;
        if(t == 1) {// wire is white
            y = 0;
            if(r == 1) { // red right pin of chip
                x = 7;
                y = 3;
            }
            if(r == 2) { // green right pin of chip
                x = 3;
                y = 3;
            }
        }
        if(t == 2) {y = 1;} // wire is red
        if(t == 3) {y = 2;} // wire is green
    }
    else if(t == l) { // top left
        x = 6;
        if(t == 1) {y = 0;} // wire is white
        if(t == 2) {y = 1;} // wire is red
        if(t == 3) {y = 2;} // wire is green
    }
    else if(t == r) { // top right
        x = 5;
        if(t == 1) {y = 0;} // wire is white
        if(t == 2) {y = 1;} // wire is red
        if(t == 3) {y = 2;} // wire is green
    }
    else if(b == l) { // bottom left
        x = 4;
        if(b == 1) {y = 0;} // wire is white
        if(b == 2) {y = 1;} // wire is red
        if(b== 3) {y = 2;} // wire is green
    }
    else if(b == r) { // bottom right
        x = 3;
        if(b == 1) {y = 0;} // wire is white
        if(b == 2) {y = 1;} // wire is red
        if(b == 3) {y = 2;} // wire is green
    }

    if(x != -1 && y != -1) {wireSprite.render(rp, rb, x, y);}
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

    
    for(int y = 0; y < HEIGHT; y++) {
        for(int x = 0; x < WIDTH; x++) {
            int l = 0;
            int r = 0;
            int t = 0;
            int b = 0;

            if(x != 0) {l = hWireTable[x + y * (WIDTH - 1) + 1];}
            if(x != WIDTH - 1) {r = hWireTable[x + y * (WIDTH - 1)];}
            if(y != 0) {t = vWireTable[y + x * (HEIGHT - 1) + 1];}
            if(y != HEIGHT - 1) {b = vWireTable[y + x * (HEIGHT - 1)];}

            renderWireSquare(wireSprite, vec2(x * 8, y * 12), l, r, t, b, &rp, &rb);
        }
    }

    //renderString(fontSprite, vec2(40, 40), "asdASDIl1:", &rp, &rb);
}
void MapState::unload() {
	mixer.unload();
}

void MapState::handleEvent(SDL_Event* event) {controller.handleEvent(event);}