#include <string>
#include <iostream>

#include "windows.h"
#include "psapi.h"

#include "mapState.hpp"
#include "audio.hpp"
#include "system.hpp"

#define VERTS_SIZE 16

#define SHP(x, y, i) wt.setHPointer(x, y, &netValues[i])
#define SHPS(x1, x2, y, i) for(int x = x1; x <= x2; x++) {SHP(x, y, i);}
#define SVP(x, y, i) wt.setVPointer(x, y, &netValues[i])
#define SVPS(x, y1, y2, i) for(int y = y1; y <= y2; y++) {SVP(x, y, i);}

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
}
void MapState::init(Window* window, Game* game)  {
    cam.init((int)window->getScreenSize().x, (int)window->getScreenSize().y);

    mixer.init();
    memset(&controller, 0, sizeof(Controller)); // could be done with a union in Controller

    stateStartTime = steady_clock::now();
    frameStartTime = stateStartTime;

    debugPrintTimer = 0;

    wt.init();

    // 28, 9

    // clk
    SVP(22, 18, 0); SHPS(22, 31, 19, 0);
    // rd
    SHPS(46, 54, 33, 1); SVPS(55, 27, 32, 1); SHPS(55, 134, 27, 1); SVPS(96, 27, 48, 1); SHPS(90, 95, 49, 1); SVPS(135, 27, 48, 1); SHPS(128, 134, 49, 1);
    // wr
    SHPS(46, 53, 32, 2); SVPS(54, 26, 31, 2); SHPS(54, 129, 26, 2); SVPS(92, 26, 43, 2); SHPS(90, 91, 44, 2); SVPS(130, 26, 43, 2); SHPS(128, 129, 44, 2);
    // a0
    SHPS(46, 107, 24, 3); SVPS(70, 24, 51, 3); SHPS(70, 79, 52, 3); SVPS(108, 24, 51, 3); SHPS(108, 117, 52, 3);
    // a1
    SHPS(46, 108, 23, 4); SVPS(71, 23, 50, 4); SHPS(71, 79, 51, 4); SVPS(109, 23, 50, 4); SHPS(109, 117, 51, 4);
    // a2
    SHPS(46, 109, 22, 5); SVPS(72, 22, 49, 5); SHPS(72, 79, 50, 5); SVPS(110, 22, 49, 5); SHPS(110, 117, 50, 5);
    // a3
    SHPS(46, 110, 21, 6); SVPS(73, 21, 48, 6); SHPS(73, 79, 49, 6); SVPS(111, 21, 48, 6); SHPS(111, 117, 49, 6);
    // a4
    SHPS(46, 111, 20, 7); SVPS(74, 20, 47, 7); SHPS(74, 79, 48, 7); SVPS(112, 20, 47, 7); SHPS(112, 117, 48, 7);
    // a5
    SHPS(46, 112, 19, 8); SVPS(75, 19, 46, 8); SHPS(75, 79, 47, 8); SVPS(113, 19, 46, 8); SHPS(113, 117, 47, 8);
    // a6
    SHPS(46, 113, 18, 9); SVPS(76, 18, 45, 9); SHPS(76, 79, 46, 9); SVPS(114, 18, 45, 9); SHPS(114, 117, 46, 9);
    // a7
    SHPS(46, 114, 17, 10); SVPS(77, 17, 44, 10); SHPS(77, 79, 45, 10); SVPS(115, 17, 44, 10); SHPS(115, 117, 45, 10);
    // a8
    SHPS(46, 131, 16, 11); SVPS(93, 16, 45, 11); SHPS(90, 92, 46, 11); SVPS(132, 16, 45, 11); SHPS(128, 131, 46, 11);
    // a9
    SHPS(46, 132, 15, 12); SVPS(94, 15, 46, 12); SHPS(90, 93, 47, 12); SVPS(133, 15, 46, 12); SHPS(128, 132, 47, 12);
    // a10
    SHPS(46, 135, 14, 13); SVPS(97, 14, 49, 13); SHPS(90, 96, 50, 13); SVPS(136, 14, 49, 13); SHPS(128, 135, 50, 13);
    // a11
    SHPS(30, 31, 14, 14); SVPS(30, 11, 13, 14); SHPS(30, 47, 11, 14); SVPS(48, 11, 12, 14); SHPS(48, 133, 13, 14); SVPS(95, 13, 47, 14); SHPS(90, 94, 48, 14); SVPS(134, 13, 47, 14); SHPS(128, 133, 48, 14);
    // a12
    SHPS(29, 31, 15, 15); SVPS(29, 10, 14, 15); SHPS(29, 48, 10, 15); SVPS(49, 10, 11, 15); SHPS(49, 115, 12, 15); SVPS(78, 12, 43, 15); SHPS(78, 79, 44, 15); SVPS(116, 12, 43, 15); SHPS(116, 117, 44, 15);
    // a13
    SHPS(28, 31, 16, 16); SVPS(28, 9, 15, 16); SHPS(28, 49, 9, 16); SVPS(50, 9, 10, 16); SHPS(50, 130, 11, 16); SVPS(98, 11, 50, 16); SHPS(90, 97, 51, 16); SVPS(131, 11, 44, 16); SHPS(128, 130, 45, 16);
    // d0
    SHPS(21, 31, 27, 17); SVPS(21, 27, 42, 17); SHPS(21, 64, 43, 17); SVPS(65, 37, 42, 17); SHPS(65, 106, 37, 17); SVPS(69, 37, 52, 17); SHPS(69, 79, 53, 17); SVPS(107, 37, 52, 17); SHPS(107, 117, 53, 17);
    // d1
    SHPS(22, 31, 28, 18); SVPS(22, 28, 41, 18); SHPS(22, 63, 42, 18); SVPS(64, 36, 41, 18); SHPS(64, 105, 36, 18); SVPS(68, 36, 53, 18); SHPS(68, 79, 54, 18); SVPS(106, 36, 53, 18); SHPS(106, 117, 54, 18);
    // d2
    SHPS(23, 31, 25, 19); SVPS(23, 25, 40, 19); SHPS(23, 62, 41, 19); SVPS(63, 35, 40, 19); SHPS(63, 104, 35, 19); SVPS(67, 35, 54, 19); SHPS(67, 79, 55, 19); SVPS(105, 35, 54, 19); SHPS(105, 117, 55, 19);
    // d3
    SHPS(24, 31, 21, 20); SVPS(24, 21, 39, 20); SHPS(24, 61, 40, 20); SVPS(62, 34, 39, 20); SHPS(62, 140, 34, 20); SVPS(103, 34, 55, 20); SHPS(90, 102, 56, 20); SVPS(141, 34, 55, 20); SHPS(128, 140, 56, 20);
    // d4
    SHPS(25, 31, 20, 21); SVPS(25, 20, 38, 21); SHPS(25, 60, 39, 21); SVPS(61, 33, 38, 21); SHPS(61, 139, 33, 21); SVPS(102, 33, 54, 21); SHPS(90, 101, 55, 21); SVPS(140, 33, 54, 21); SHPS(128, 139, 55, 21);
    // d5
    SHPS(26, 31, 22, 22); SVPS(26, 22, 37, 22); SHPS(26, 59, 38, 22); SVPS(60, 32, 37, 22); SHPS(60, 138, 32, 22); SVPS(101, 32, 53, 22); SHPS(90, 100, 54, 22); SVPS(139, 32, 53, 22); SHPS(128, 138, 54, 22);
    // d6
    SHPS(27, 31, 23, 23); SVPS(27, 23, 36, 23); SHPS(27, 58, 37, 23); SVPS(59, 31, 36, 23); SHPS(59, 137, 31, 23); SVPS(100, 31, 52, 23); SHPS(90, 99, 53, 23); SVPS(138, 31, 52, 23); SHPS(128, 137, 53, 23);
    // d7
    SHPS(28, 31, 26, 24); SVPS(28, 26, 35, 24); SHPS(28, 57, 36, 24); SVPS(58, 30, 35, 24); SHPS(58, 136, 30, 24); SVPS(99, 30, 51, 24); SHPS(90, 98, 52, 24); SVPS(137, 30, 51, 24); SHPS(128, 136, 52, 24);

    canClock = false;
    canSelectUp = false;
    canSelectDown = false;
    canModify = false;

    selectedWire = 1;
    selectedValue = netValues[selectedWire];

    initSystem();
}
void MapState::update(Window* window, Game* game)  {
    time_point<steady_clock> currentTime = steady_clock::now();
    dt = duration_cast<milliseconds>(currentTime - frameStartTime).count();
    frameStartTime = currentTime;
    timeSinceInit = duration_cast<milliseconds>(currentTime - stateStartTime).count();

    ticksSinceLastDebugPrint++;

    if(controller.space) {
        if(canClock) {
            if(netValues[0] == 2) {netValues[0] = 3;}
            else {netValues[0] = 2;}
        }
        canClock = false;
    }
    else {canClock = true;}

    /*if(controller.up) {
        if(canSelectUp) {
            netValues[selectedWire] = selectedValue;

            if(selectedWire == 24) {selectedWire = 1;}
            else {selectedWire++;}

            selectedValue = netValues[selectedWire];

            selectStartTime = timeSinceInit;
        }
        canSelectUp = false;
    }
    else {canSelectUp = true;}
    if(controller.down) {
        if(canSelectDown) {
            netValues[selectedWire] = selectedValue;

            if(selectedWire == 1) {selectedWire = 24;}
            else {selectedWire--;}

            selectedValue = netValues[selectedWire];

            selectStartTime = timeSinceInit;
        }
        canSelectDown = false;
    }
    else {canSelectDown = true;}

    if(controller.right) {
        if(canModify) {
            if(selectedValue == 2) {selectedValue = 3;}
            else {selectedValue = 2;}

            selectStartTime = timeSinceInit - 150;
        }
        canModify = false;
    }
    else {canModify = true;}*/

    updateSystem();
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
        if(l == 2) {x = 13;} // h wire is red
        if(l == 3) {x = 12;} // h wire is green
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
    else if(t == l && b == 0 && r == 0) { // top left
        x = 6;
        if(t == 1) {y = 0;} // wire is white
        if(t == 2) {y = 1;} // wire is red
        if(t == 3) {y = 2;} // wire is green
    }
    else if(t == r && b == 0 && l == 0) { // top right
        x = 5;
        if(t == 1) {y = 0;} // wire is white
        if(t == 2) {y = 1;} // wire is red
        if(t == 3) {y = 2;} // wire is green
    }
    else if(b == l && t == 0 && r == 0) { // bottom left
        x = 4;
        if(b == 1) {y = 0;} // wire is white
        if(b == 2) {y = 1;} // wire is red
        if(b == 3) {y = 2;} // wire is green
    }
    else if(b == r && t == 0 && l == 0) { // bottom right
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

    /*if((timeSinceInit - selectStartTime) % 300 >= 150) {netValues[selectedWire] = selectedValue;}
    else {netValues[selectedWire] = 1;}*/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    rp.useViewMatrix(&cam);

    renderString(fontSprite, vec2(0, 0) * vec2(8, 12), "Clock Cycles: " + to_string((int)((htcCount - 1) / 2)), &rp, &rb);
    
    for(int y = 0; y < 60; y++) {
        for(int x = 0; x < 160; x++) {
            int l = 0;
            int r = 0;
            int t = 0;
            int b = 0;

            if(x != 0) {l = wt.getHValue(x - 1, y);}
            if(x != 160 - 1) {r = wt.getHValue(x, y);}
            if(y != 0) {t = wt.getVValue(x, y - 1);}
            if(y != 60 - 1) {b = wt.getVValue(x, y);}

            renderWireSquare(wireSprite, vec2(x * 8, y * 12), l, r, t, b, &rp, &rb);
        }
    }

    // clock border
    wireSprite.pos = vec2(20, 13) * vec2(8, 12); wireSprite.render(&rp, &rb, 3, 0);
    wireSprite.pos = vec2(21, 13) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(22, 13) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(23, 13) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(24, 13) * vec2(8, 12); wireSprite.render(&rp, &rb, 4, 0);
    wireSprite.pos = vec2(20, 18) * vec2(8, 12); wireSprite.render(&rp, &rb, 5, 0);
    wireSprite.pos = vec2(20, 14) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(24, 14) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(20, 15) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(24, 15) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(20, 16) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(24, 16) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(20, 17) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(24, 17) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(21, 18) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(22, 18) * vec2(8, 12); wireSprite.render(&rp, &rb, 9, 0);
    wireSprite.pos = vec2(23, 18) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(24, 18) * vec2(8, 12); wireSprite.render(&rp, &rb, 6, 0);

    // clock indicators
    if(netValues[0] == 2) {
        wireSprite.pos = vec2(21, 14) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 1);
        wireSprite.pos = vec2(22, 14) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 1);
        wireSprite.pos = vec2(23, 14) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 1);
        wireSprite.pos = vec2(21, 15) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 1);
        wireSprite.pos = vec2(22, 15) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 1);
        wireSprite.pos = vec2(23, 15) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 1);
    }
    if(netValues[0] == 3) {
        wireSprite.pos = vec2(21, 16) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 2);
        wireSprite.pos = vec2(22, 16) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 2);
        wireSprite.pos = vec2(23, 16) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 2);
        wireSprite.pos = vec2(21, 17) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 2);
        wireSprite.pos = vec2(22, 17) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 2);
        wireSprite.pos = vec2(23, 17) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 2);
    }

    // VCCs
    renderString(fontSprite, vec2(18, 22) * vec2(8, 12), "VCC", &rp, &rb);
    renderString(fontSprite, vec2(48, 26) * vec2(8, 12), "VCC", &rp, &rb);
    renderString(fontSprite, vec2(88, 39) * vec2(8, 12), "VCC", &rp, &rb);
    renderString(fontSprite, vec2(126, 39) * vec2(8, 12), "VCC", &rp, &rb);

    // VCC underlines
    wireSprite.pos = vec2(18, 23) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);
    wireSprite.pos = vec2(19, 23) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 3);
    wireSprite.pos = vec2(20, 23) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);

    wireSprite.pos = vec2(48, 27) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);
    wireSprite.pos = vec2(49, 27) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 3);
    wireSprite.pos = vec2(50, 27) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);

    wireSprite.pos = vec2(88, 40) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);
    wireSprite.pos = vec2(89, 40) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 3);
    wireSprite.pos = vec2(90, 40) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);

    wireSprite.pos = vec2(126, 40) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);
    wireSprite.pos = vec2(127, 40) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 3);
    wireSprite.pos = vec2(128, 40) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);

    // VCC wires
    for(int i = 20; i <= 31; i++) {
        wireSprite.pos = vec2(i, 24) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
        wireSprite.pos = vec2(i, 29) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
        wireSprite.pos = vec2(i, 30) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    }

    wireSprite.pos = vec2(19, 24) * vec2(8, 12); wireSprite.render(&rp, &rb, 7, 0);
    wireSprite.pos = vec2(19, 25) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(19, 26) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(19, 27) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(19, 28) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(19, 29) * vec2(8, 12); wireSprite.render(&rp, &rb, 7, 0);
    wireSprite.pos = vec2(19, 30) * vec2(8, 12); wireSprite.render(&rp, &rb, 5, 0);

    wireSprite.pos = vec2(49, 28) * vec2(8, 12); wireSprite.render(&rp, &rb, 8, 0);
    wireSprite.pos = vec2(49, 29) * vec2(8, 12); wireSprite.render(&rp, &rb, 8, 0);
    wireSprite.pos = vec2(49, 30) * vec2(8, 12); wireSprite.render(&rp, &rb, 6, 0);

    wireSprite.pos = vec2(47, 28) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(48, 28) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(47, 29) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(48, 29) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(47, 30) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(48, 30) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    
    wireSprite.pos = vec2(89, 41) * vec2(8, 12); wireSprite.render(&rp, &rb, 5, 0);
    wireSprite.pos = vec2(90, 41) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(91, 41) * vec2(8, 12); wireSprite.render(&rp, &rb, 4, 0);
    wireSprite.pos = vec2(91, 42) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(91, 43) * vec2(8, 12); wireSprite.render(&rp, &rb, 6, 0);

    wireSprite.pos = vec2(127, 41) * vec2(8, 12); wireSprite.render(&rp, &rb, 5, 0);
    wireSprite.pos = vec2(128, 41) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(129, 41) * vec2(8, 12); wireSprite.render(&rp, &rb, 4, 0);
    wireSprite.pos = vec2(129, 42) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
    wireSprite.pos = vec2(129, 43) * vec2(8, 12); wireSprite.render(&rp, &rb, 6, 0);

    // GNDs
    wireSprite.pos = vec2(52, 26) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 3);
    wireSprite.pos = vec2(78, 57) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 3);
    wireSprite.pos = vec2(116, 57) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 3);

    // GND wires
    wireSprite.pos = vec2(47, 25) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(48, 25) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(49, 25) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(50, 25) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(51, 25) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    wireSprite.pos = vec2(52, 25) * vec2(8, 12); wireSprite.render(&rp, &rb, 4, 0);

    wireSprite.pos = vec2(78, 56) * vec2(8, 12); wireSprite.render(&rp, &rb, 3, 0);
    wireSprite.pos = vec2(79, 56) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);

    wireSprite.pos = vec2(116, 56) * vec2(8, 12); wireSprite.render(&rp, &rb, 3, 0);
    wireSprite.pos = vec2(117, 56) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);

    // Z80 text
    renderString(fontSprite, vec2(33, 14) * vec2(8, 12), 
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
        "\n"
        "Z80"
    , &rp, &rb);

    // Z80 corners
    wireSprite.pos = vec2(32, 13) * vec2(8, 12); wireSprite.render(&rp, &rb, 3, 0);
    wireSprite.pos = vec2(46, 13) * vec2(8, 12); wireSprite.render(&rp, &rb, 4, 0);
    wireSprite.pos = vec2(32, 34) * vec2(8, 12); wireSprite.render(&rp, &rb, 5, 0);
    wireSprite.pos = vec2(46, 34) * vec2(8, 12); wireSprite.render(&rp, &rb, 6, 0);

    // Z80 top and bottom
    for(int i = 0; i < 13; i++) {
        wireSprite.pos = (vec2(i, 0) + vec2(33, 13)) * vec2(8, 12);
        if(i == 6) {wireSprite.render(&rp, &rb, 11, 3);}
        else {wireSprite.render(&rp, &rb, 1, 0);}
        wireSprite.pos = (vec2(i, 0) + vec2(33, 34)) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    }

    // Z80 sides
    for(int i = 0; i < 20; i++) {
        wireSprite.pos = (vec2(0, i) + vec2(32, 14)) * vec2(8, 12); wireSprite.render(&rp, &rb, 8, 0);
        wireSprite.pos = (vec2(0, i) + vec2(46, 14)) * vec2(8, 12); wireSprite.render(&rp, &rb, 7, 0);
    }

    // ROM text
    renderString(fontSprite, vec2(81, 43) * vec2(8, 12), 
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
        "\n"
        "ROM"
    , &rp, &rb);

    // RAM text
    renderString(fontSprite, vec2(119, 43) * vec2(8, 12), 
        "      +5V\n"
        "A12   /WE\n"
        "A7    CS2\n"
        "A6     A8\n"
        "A5     A9\n"
        "A4    A11\n"
        "A3    /OE\n"
        "A2    A10\n"
        "A1   /CS1\n"
        "A0   I/O7\n"
        "I/O0 I/O6\n"
        "I/O1 I/O5\n"
        "I/O2 I/O4\n"
        "GND  I/O3\n"
        "\n"
        "RAM"
    , &rp, &rb);


    // ROM corners
    wireSprite.pos = vec2(80, 42) * vec2(8, 12); wireSprite.render(&rp, &rb, 3, 0);
    wireSprite.pos = vec2(90, 42) * vec2(8, 12); wireSprite.render(&rp, &rb, 4, 0);
    wireSprite.pos = vec2(80, 57) * vec2(8, 12); wireSprite.render(&rp, &rb, 5, 0);
    wireSprite.pos = vec2(90, 57) * vec2(8, 12); wireSprite.render(&rp, &rb, 6, 0);

    // RAM corners
    wireSprite.pos = vec2(118, 42) * vec2(8, 12); wireSprite.render(&rp, &rb, 3, 0);
    wireSprite.pos = vec2(128, 42) * vec2(8, 12); wireSprite.render(&rp, &rb, 4, 0);
    wireSprite.pos = vec2(118, 57) * vec2(8, 12); wireSprite.render(&rp, &rb, 5, 0);
    wireSprite.pos = vec2(128, 57) * vec2(8, 12); wireSprite.render(&rp, &rb, 6, 0);

    // ROM and RAM top and bottom
    for(int i = 0; i < 9; i++) {
        wireSprite.pos = (vec2(i, 0) + vec2(81, 42)) * vec2(8, 12);
        if(i == 4) {wireSprite.render(&rp, &rb, 11, 3);}
        else {wireSprite.render(&rp, &rb, 1, 0);}
        wireSprite.pos = (vec2(i, 0) + vec2(81, 57)) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);

        wireSprite.pos = (vec2(i, 0) + vec2(119, 42)) * vec2(8, 12);
        if(i == 4) {wireSprite.render(&rp, &rb, 11, 3);}
        else {wireSprite.render(&rp, &rb, 1, 0);}
        wireSprite.pos = (vec2(i, 0) + vec2(119, 57)) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
    }

    // ROM and RAM sides
    for(int i = 0; i < 14; i++) {
        wireSprite.pos = (vec2(0, i) + vec2(80, 43)) * vec2(8, 12);
        if(i != 0) {wireSprite.render(&rp, &rb, 8, 0);}
        else {wireSprite.render(&rp, &rb, 2, 0);}
        wireSprite.pos = (vec2(0, i) + vec2(90, 43)) * vec2(8, 12);
        if(i != 2) {wireSprite.render(&rp, &rb, 7, 0);}
        else {wireSprite.render(&rp, &rb, 2, 0);}

        wireSprite.pos = (vec2(0, i) + vec2(118, 43)) * vec2(8, 12);
        if(i != 0) {wireSprite.render(&rp, &rb, 8, 0);}
        else {wireSprite.render(&rp, &rb, 2, 0);}
        wireSprite.pos = (vec2(0, i) + vec2(128, 43)) * vec2(8, 12); wireSprite.render(&rp, &rb, 7, 0);
    }
}
void MapState::unload() {
	mixer.unload();
}

void MapState::handleEvent(SDL_Event* event) {controller.handleEvent(event);}