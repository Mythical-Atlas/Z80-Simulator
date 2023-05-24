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
}
void MapState::init(Window* window, Game* game)  {
    cam.init((int)window->getScreenSize().x, (int)window->getScreenSize().y);
    cam.pos += vec2(0, 12 * 8);

    mixer.init();
    memset(&controller, 0, sizeof(Controller)); // could be done with a union in Controller

    stateStartTime = steady_clock::now();
    frameStartTime = stateStartTime;

    debugPrintTimer = 0;

    wt.init();

    initWires();

    canClock = false;
    canSelectUp = false;
    canSelectDown = false;
    canModify = false;

    selectedWire = 1;
    selectedValue = circuit.netValues[selectedWire];

    circuit.initSystem();

    memset(clkHis, 0, HISTORY_LENGTH * sizeof(bool));
    memset(rdHis, 1, HISTORY_LENGTH * sizeof(bool));
    memset(wrHis, 1, HISTORY_LENGTH * sizeof(bool));
    memset(addHis0, 0, HISTORY_LENGTH * sizeof(bool));
    memset(addHis1, 1, HISTORY_LENGTH * sizeof(bool));
    memset(datHis0, 0, HISTORY_LENGTH * sizeof(bool));
    memset(datHis1, 1, HISTORY_LENGTH * sizeof(bool));
    memset(mreqHis, 1, HISTORY_LENGTH * sizeof(bool));
    memset(rfshHis, 1, HISTORY_LENGTH * sizeof(bool));
    memset(m1His, 1, HISTORY_LENGTH * sizeof(bool));
}
void MapState::update(Window* window, Game* game)  {
    time_point<steady_clock> currentTime = steady_clock::now();
    dt = duration_cast<milliseconds>(currentTime - frameStartTime).count();
    frameStartTime = currentTime;
    timeSinceInit = duration_cast<milliseconds>(currentTime - stateStartTime).count();

    ticksSinceLastDebugPrint++;

    uint16_t addValInit = circuit.getAddressBus();
    uint8_t datValInit = circuit.getDataBus();

    bool updateHis = false;
    if(controller.space) {
        controller.space = false;
        //if(canClock) {
            if(circuit.netValues[0] == 2) {circuit.netValues[0] = 3;}
            else {circuit.netValues[0] = 2;}
            updateHis = true;
        //}
        canClock = false;
    }
    else {canClock = true;}

    circuit.updateSystem();

    uint16_t addValFinal = circuit.getAddressBus();
    uint8_t datValFinal = circuit.getDataBus();

    if(updateHis) {
        for(int i = 0; i < HISTORY_LENGTH - 1; i++) {
            clkHis[i] = clkHis[i + 1];
            rdHis[i] = rdHis[i + 1];
            wrHis[i] = wrHis[i + 1];
            addHis0[i] = addHis0[i + 1];
            addHis1[i] = addHis1[i + 1];
            datHis0[i] = datHis0[i + 1];
            datHis1[i] = datHis1[i + 1];
            mreqHis[i] = mreqHis[i + 1];
            rfshHis[i] = rfshHis[i + 1];
            m1His[i] = m1His[i + 1];
        }
        clkHis[HISTORY_LENGTH - 1] = circuit.netValues[0] == 2;
        rdHis[HISTORY_LENGTH - 1] = circuit.netValues[1] == 2;
        wrHis[HISTORY_LENGTH - 1] = circuit.netValues[2] == 2;

        mreqHis[HISTORY_LENGTH - 1] = circuit.netValues[25] == 2;
        rfshHis[HISTORY_LENGTH - 1] = circuit.netValues[26] == 2;
        m1His[HISTORY_LENGTH - 1] = circuit.netValues[27] == 2;

        addHis0[HISTORY_LENGTH - 1] = addHis0[HISTORY_LENGTH - 2];
        addHis1[HISTORY_LENGTH - 1] = addHis1[HISTORY_LENGTH - 2];
        datHis0[HISTORY_LENGTH - 1] = datHis0[HISTORY_LENGTH - 2];
        datHis1[HISTORY_LENGTH - 1] = datHis1[HISTORY_LENGTH - 2];

        if(addValInit != addValFinal) {
            addHis0[HISTORY_LENGTH - 1] = !addHis0[HISTORY_LENGTH - 1];
            addHis1[HISTORY_LENGTH - 1] = !addHis1[HISTORY_LENGTH - 1];
        }
        if(datValInit != datValFinal) {
            datHis0[HISTORY_LENGTH - 1] = !datHis0[HISTORY_LENGTH - 1];
            datHis1[HISTORY_LENGTH - 1] = !datHis1[HISTORY_LENGTH - 1];
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

    renderCircuit();

    renderString(fontSprite, vec2(1, 40) * vec2(8, 12), "Address Bus: 0x" + circuit.hex16(circuit.getAddressBus()), &rp, &rb);
    renderString(fontSprite, vec2(1, 41) * vec2(8, 12), "Data Bus: 0x" + circuit.hex8(circuit.getDataBus()), &rp, &rb);
    renderString(fontSprite, vec2(1, 43) * vec2(8, 12), "PC: 0x" + circuit.hex16(circuit.pc), &rp, &rb);

    if(circuit.currentTCycleHalf == 0) {renderString(fontSprite, vec2(10, 45) * vec2(8, 12), "First Half of T Cycle (Clock Just Rose)", &rp, &rb);}
    else if(circuit.currentTCycleHalf == 1) {renderString(fontSprite, vec2(10, 45) * vec2(8, 12), "Second Half of T Cycle (Clock Just Fell)", &rp, &rb);}
    else {renderString(fontSprite, vec2(10, 45) * vec2(8, 12), "Not in T Cycle Yet (Clock Hasn't Changed)", &rp, &rb);}
    renderString(fontSprite, vec2(10, 46) * vec2(8, 12), "Current T Cycle: " + to_string(circuit.currentTCycle), &rp, &rb);
    renderString(fontSprite, vec2(10, 47) * vec2(8, 12), "Current M Cycle: " + to_string(circuit.currentMCycle), &rp, &rb);
    if(circuit.mType == 0) {renderString(fontSprite, vec2(10, 48) * vec2(8, 12), "Operation: Instruction Fetch (M1)", &rp, &rb);}
    if(circuit.mType == 1) {renderString(fontSprite, vec2(10, 48) * vec2(8, 12), "Operation: Memory Read", &rp, &rb);}
    if(circuit.mType == 2) {renderString(fontSprite, vec2(10, 48) * vec2(8, 12), "Operation: Memory Write", &rp, &rb);}

    if(circuit.currentInstruction != -1) {
        renderString(fontSprite, vec2(10, 50) * vec2(8, 12), "Instruction: " + decodeInstruction(circuit.currentInstruction), &rp, &rb);
        renderString(fontSprite, vec2(10, 51) * vec2(8, 12), "Op Code: 0x" + circuit.hex8(circuit.currentInstruction), &rp, &rb);
        renderString(fontSprite, vec2(10, 52) * vec2(8, 12), "M Cycles " + decodeMCycles(circuit.currentInstruction), &rp, &rb);
        renderString(fontSprite, vec2(10, 53) * vec2(8, 12), "T Cycles " + decodeTCycles(circuit.currentInstruction), &rp, &rb);
    }
    else {
        renderString(fontSprite, vec2(10, 50) * vec2(8, 12), "Instruction: Unknown", &rp, &rb);
        renderString(fontSprite, vec2(10, 51) * vec2(8, 12), "Op Code: 0x??", &rp, &rb);
        renderString(fontSprite, vec2(10, 52) * vec2(8, 12), "M Cycles (?): Unknown", &rp, &rb);
        renderString(fontSprite, vec2(10, 53) * vec2(8, 12), "T Cycles (?): Unknown", &rp, &rb);
    }

    renderString(fontSprite, vec2(10, 60) * vec2(8, 12),
        "   /CLK\n\n\n\n"
        "ADDRESS\n\n\n\n"
        "   MREQ\n\n\n\n"
        "    /RD\n\n\n\n"
        "    /WR\n\n\n\n"
        "     M1\n\n\n\n"
        "   DATA\n\n\n\n"
        "   RFSH\n\n\n\n"
    , &rp, &rb);

    renderHistogram(wireSprite, vec2(20, 60) * vec2(8, 12), clkHis, &rp, &rb);
    renderHistogram(wireSprite, vec2(20, 64) * vec2(8, 12), addHis0, &rp, &rb);
    renderHistogram(wireSprite, vec2(20, 64) * vec2(8, 12), addHis1, &rp, &rb);
    renderHistogram(wireSprite, vec2(20, 68) * vec2(8, 12), mreqHis, &rp, &rb);
    renderHistogram(wireSprite, vec2(20, 72) * vec2(8, 12), rdHis, &rp, &rb);
    renderHistogram(wireSprite, vec2(20, 76) * vec2(8, 12), wrHis, &rp, &rb);
    renderHistogram(wireSprite, vec2(20, 80) * vec2(8, 12), m1His, &rp, &rb);
    renderHistogram(wireSprite, vec2(20, 84) * vec2(8, 12), datHis0, &rp, &rb);
    renderHistogram(wireSprite, vec2(20, 84) * vec2(8, 12), datHis1, &rp, &rb);
    renderHistogram(wireSprite, vec2(20, 88) * vec2(8, 12), rfshHis, &rp, &rb);
}
void MapState::unload() {
	mixer.unload();
}

void MapState::handleEvent(SDL_Event* event) {controller.handleEvent(event);}