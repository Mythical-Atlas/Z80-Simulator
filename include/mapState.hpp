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
#include "system.hpp"
#include "instructions.hpp"

#define HISTORY_LENGTH 40

#define CIRCUIT_TOP_LEFT 0, 0

#define Z80_TOP_LEFT 10, 11
#define Z80_SIZE     14, 21
#define Z80_MIDDLE   6

#define ROM_TOP_LEFT  51, 33
#define RAM_TOP_LEFT  89, 33
#define MEM_IC_SIZE   10, 15
#define MEM_IC_MIDDLE 4

#define CLOCK_TOP_LEFT 20, 13
#define CLOCK_SIZE     4, 5
#define CLOCK_MIDDLE   1

#define VCC_1_TOP_LEFT 18, 22
#define VCC_2_TOP_LEFT 48, 26
#define VCC_3_TOP_LEFT 88, 39
#define VCC_4_TOP_LEFT 126, 39

#define GND_1_POS 52, 26
#define GND_2_POS 78, 57
#define GND_3_POS 116, 57

#define RW_BUS_RAISE 7
#define RW_BUS_BUFFER 3

#define DATA_BUS_DIP          0
#define DATA_BUS_RAISE        9
#define DATA_BUS_LEFT_BUFFER  1
#define DATA_BUS_RIGHT_BUFFER 5

#define ROM_LEFT_BUFFER  1
#define ROM_RIGHT_BUFFER 2
#define RAM_LEFT_BUFFER  1
#define RAM_RIGHT_BUFFER 2

#define ADDRESS_TOP_BUFFER   0
#define ADDRESS_LEFT_BUFFER  1
#define ADDRESS_RIGHT_BUFFER 0

#define SHP(x, y, i) wt.setHPointer(x, y, &circuit.netValues[i])
#define SHPS(x1, x2, y, i) for(int x = x1; x <= x2; x++) {SHP(x, y, i);}
#define SVP(x, y, i) wt.setVPointer(x, y, &circuit.netValues[i])
#define SVPS(x, y1, y2, i) for(int y = y1; y <= y2; y++) {SVP(x, y, i);}

#define INDICATE(x, y, i) wireSprite.pos = vec2(x, y) * vec2(8, 12); if(circuit.netValues[i] == 2) {wireSprite.render(&rp, &rb, 0, 1);} if(circuit.netValues[i] == 3) {wireSprite.render(&rp, &rb, 0, 2);}

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

	Circuit circuit;

	bool clkHis[HISTORY_LENGTH];
	bool rdHis[HISTORY_LENGTH];
	bool wrHis[HISTORY_LENGTH];
	bool addHis0[HISTORY_LENGTH];
	bool addHis1[HISTORY_LENGTH];
	bool datHis0[HISTORY_LENGTH];
	bool datHis1[HISTORY_LENGTH];
	bool mreqHis[HISTORY_LENGTH];
	bool rfshHis[HISTORY_LENGTH];
	bool m1His[HISTORY_LENGTH];

	/*AudioStream music;
	AudioStream move;
	AudioStream select;*/

	void load();
	void init(class Window* window, class Game* game);
	void update(Window* window, Game* game);
	void render(Window* window, Game* game);
	void unload();

	void handleEvent(SDL_Event* event);

	void initWires() {
		int clockWireL = vec2(CLOCK_TOP_LEFT).x + CLOCK_MIDDLE + 1;
		int clockWireR = vec2(Z80_TOP_LEFT).x - 1;
		int clockWireT = vec2(CLOCK_TOP_LEFT).y + vec2(CLOCK_SIZE).y;
		int clockWireB = vec2(Z80_TOP_LEFT).y + 6;

		int z80P1L = vec2(Z80_TOP_LEFT).x;
		int z80P1T = vec2(Z80_TOP_LEFT).y + 1;
		int z80P21L = vec2(Z80_TOP_LEFT).x + vec2(Z80_SIZE).x;
		int z80P21T = vec2(Z80_TOP_LEFT).y + vec2(Z80_SIZE).y - 1;

		int romP1L = vec2(ROM_TOP_LEFT).x;
		int romP1T = vec2(ROM_TOP_LEFT).y + 1;
		int romP21L = vec2(ROM_TOP_LEFT).x + vec2(MEM_IC_SIZE).x;
		int romP21T = vec2(ROM_TOP_LEFT).y + vec2(MEM_IC_SIZE).y - 1;

		int ramP1L = vec2(RAM_TOP_LEFT).x;
		int ramP1T = vec2(RAM_TOP_LEFT).y + 1;
		int ramP21L = vec2(RAM_TOP_LEFT).x + vec2(MEM_IC_SIZE).x;
		int ramP21T = vec2(RAM_TOP_LEFT).y + vec2(MEM_IC_SIZE).y - 1;

		int rwBusL = z80P21L + RW_BUS_BUFFER + 1;
		int rwBusT = z80P21T - RW_BUS_RAISE - 1;

		int dBusL1 = z80P1L - DATA_BUS_LEFT_BUFFER - 8;
		int dBusT1 = z80P21T + DATA_BUS_DIP + 3;
		int dBusL2 = z80P21L + DATA_BUS_RIGHT_BUFFER + 1;
		int dBusT2 = dBusT1 - DATA_BUS_RAISE;

		int romBusL = romP1L - 1 - ROM_LEFT_BUFFER;
		int romBusR = romP21L + ROM_RIGHT_BUFFER;

		int ramBusL = ramP1L - 1 - RAM_LEFT_BUFFER;
		int ramBusR = ramP21L + RAM_RIGHT_BUFFER;

		int addressBusL = z80P1L - ADDRESS_LEFT_BUFFER - 3;
		int addressBusT = z80P1T - ADDRESS_TOP_BUFFER - 4;
		int addressBusR = z80P21L + ADDRESS_RIGHT_BUFFER + 2;

		// clk
		//SHPS(clockWireL, clockWireR, clockWireB, 0); SVPS(clockWireL, clockWireT, clockWireB - 1, 0);
		// rd
		SHPS(z80P21L + 1, rwBusL, z80P21T, 1); SVPS(rwBusL + 1, rwBusT + 1, z80P21T - 1, 1); SHPS(rwBusL + 1, ramBusR + 5, rwBusT + 1, 1);
		SVPS(romBusR + 5, rwBusT + 1, romP21T - 8, 1); SHPS(romP21L + 1, romBusR + 4, romP21T - 7, 1);
		SVPS(ramBusR + 6, rwBusT + 1, ramP21T - 8, 1); SHPS(ramP21L + 1, ramBusR + 5, ramP21T - 7, 1);
		// wr
		SHPS(z80P21L + 1, rwBusL - 1, z80P21T - 1, 2); SVPS(rwBusL, rwBusT, z80P21T - 2, 2); SHPS(rwBusL, ramBusR, rwBusT, 2);
		SVPS(romBusR + 1, rwBusT, romP1T, 2); SHPS(romP21L + 1, romBusR, romP1T + 1, 2);
		SVPS(ramBusR + 1, rwBusT, ramP1T, 2); SHPS(ramP21L + 1, ramBusR, ramP1T + 1, 2);
		// a0
		SHPS(z80P21L + 1, ramBusL - 9, z80P21T - 9, 3);
		SVPS(romBusL - 8, z80P21T - 9, romP21T - 5, 3); SHPS(romBusL - 8, romP1L - 2, romP21T - 4, 3);
		SVPS(ramBusL - 8, z80P21T - 9, ramP21T - 5, 3); SHPS(ramBusL - 8, ramP1L - 2, ramP21T - 4, 3);
		// a1
		SHPS(z80P21L + 1, ramBusL - 8, z80P21T - 10, 4);
		SVPS(romBusL - 7, z80P21T - 10, romP21T - 6, 4); SHPS(romBusL - 7, romP1L - 2, romP21T - 5, 4);
		SVPS(ramBusL - 7, z80P21T - 10, ramP21T - 6, 4); SHPS(ramBusL - 7, ramP1L - 2, ramP21T - 5, 4);
		// a2
		SHPS(z80P21L + 1, ramBusL - 7, z80P21T - 11, 5);
		SVPS(romBusL - 6, z80P21T - 11, romP21T - 7, 5); SHPS(romBusL - 6, romP1L - 2, romP21T - 6, 5);
		SVPS(ramBusL - 6, z80P21T - 11, ramP21T - 7, 5); SHPS(ramBusL - 6, ramP1L - 2, ramP21T - 6, 5);
		// a3
		SHPS(z80P21L + 1, ramBusL - 6, z80P21T - 12, 6);
		SVPS(romBusL - 5, z80P21T - 12, romP21T - 8, 6); SHPS(romBusL - 5, romP1L - 2, romP21T - 7, 6);
		SVPS(ramBusL - 5, z80P21T - 12, ramP21T - 8, 6); SHPS(ramBusL - 5, ramP1L - 2, ramP21T - 7, 6);
		// a4
		SHPS(z80P21L + 1, ramBusL - 5, z80P21T - 13, 7);
		SVPS(romBusL - 4, z80P21T - 13, romP21T - 9, 7); SHPS(romBusL - 4, romP1L - 2, romP21T - 8, 7);
		SVPS(ramBusL - 4, z80P21T - 13, ramP21T - 9, 7); SHPS(ramBusL - 4, ramP1L - 2, ramP21T - 8, 7);
		// a5
		SHPS(z80P21L + 1, ramBusL - 4, z80P21T - 14, 8);
		SVPS(romBusL - 3, z80P21T - 14, romP21T - 10, 8); SHPS(romBusL - 3, romP1L - 2, romP21T - 9, 8);
		SVPS(ramBusL - 3, z80P21T - 14, ramP21T - 10, 8); SHPS(ramBusL - 3, ramP1L - 2, ramP21T - 9, 8);
		// a6
		SHPS(z80P21L + 1, ramBusL - 3, z80P21T - 15, 9);
		SVPS(romBusL - 2, z80P21T - 15, romP21T - 11, 9); SHPS(romBusL - 2, romP1L - 2, romP21T - 10, 9);
		SVPS(ramBusL - 2, z80P21T - 15, ramP21T - 11, 9); SHPS(ramBusL - 2, ramP1L - 2, ramP21T - 10, 9);
		// a7
		SHPS(z80P21L + 1, ramBusL - 2, z80P21T - 16, 10);
		SVPS(romBusL - 1, z80P21T - 16, romP21T - 12, 10); SHPS(romBusL - 1, romP1L - 2, romP21T - 11, 10);
		SVPS(ramBusL - 1, z80P21T - 16, ramP21T - 12, 10); SHPS(ramBusL - 1, ramP1L - 2, ramP21T - 11, 10);
		// a8
		SHPS(z80P21L + 1, ramBusR + 2, z80P1T + 2, 11);
		SVPS(romBusR + 2, z80P1T + 2, romP1T + 2, 11); SHPS(romP21L + 1, romBusR + 1, romP1T + 3, 11);
		SVPS(ramBusR + 3, z80P1T + 2, ramP1T + 2, 11); SHPS(ramP21L + 1, ramBusR + 2, ramP1T + 3, 11);
		// a9
		SHPS(z80P21L + 1, ramBusR + 3, z80P1T + 1, 12);
		SVPS(romBusR + 3, z80P1T + 1, romP1T + 3, 12); SHPS(romP21L + 1, romBusR + 2, romP1T + 4, 12);
		SVPS(ramBusR + 4, z80P1T + 1, ramP1T + 3, 12); SHPS(ramP21L + 1, ramBusR + 3, ramP1T + 4, 12);
		// a10
		SHPS(z80P21L + 1, ramBusR + 6, z80P1T, 13);
		SVPS(romBusR + 6, z80P1T, romP1T + 6, 13); SHPS(romP21L + 1, romBusR + 5, romP1T + 7, 13);
		SVPS(ramBusR + 7, z80P1T, ramP1T + 6, 13); SHPS(ramP21L + 1, ramBusR + 6, ramP1T + 7, 13);
		// a11
		SHPS(addressBusL + 2, z80P1L - 2, z80P1T, 14); SVPS(addressBusL + 2, addressBusT + 2, z80P1T - 1, 14);
		SHPS(addressBusL + 2, addressBusR - 2, addressBusT + 2, 14); SVPS(addressBusR - 1, addressBusT + 2, z80P1T - 2, 14);
		SHPS(addressBusR - 1, ramBusR + 4, z80P1T - 1, 14);
		SVPS(romBusR + 4, z80P1T - 1, romP1T + 4, 14); SHPS(romP21L + 1, romBusR + 3, romP1T + 5, 14);
		SVPS(ramBusR + 5, z80P1T - 1, ramP1T + 4, 14); SHPS(ramP21L + 1, ramBusR + 4, ramP1T + 5, 14);
		// a12
		SHPS(addressBusL + 1, z80P1L - 2, z80P1T + 1, 15); SVPS(addressBusL + 1, addressBusT + 1, z80P1T, 15);
		SHPS(addressBusL + 1, addressBusR - 1, addressBusT + 1, 15); SVPS(addressBusR, addressBusT + 1, z80P1T - 3, 15);
		SHPS(addressBusR, ramBusL - 1, z80P1T - 2, 15);
		SVPS(romBusL, z80P1T - 2, romP1T, 15); SHPS(romBusL, romP1L - 2, romP1T + 1, 15);
		SVPS(ramBusL, z80P1T - 2, ramP1T, 15); SHPS(ramBusL, ramP1L - 2, ramP1T + 1, 15);
		// a13
		SHPS(addressBusL, z80P1L - 2, z80P1T + 2, 16); SVPS(addressBusL, addressBusT, z80P1T + 1, 16);
		SHPS(addressBusL, addressBusR, addressBusT, 16); SVPS(addressBusR + 1, addressBusT, z80P1T - 4, 16);
		SHPS(addressBusR + 1, ramBusR + 1, z80P1T - 3, 16);
		SVPS(romBusR + 7, z80P1T - 3, romP1T + 7, 16); SHPS(romP21L + 1, romBusR + 6, romP1T + 8, 16);
		SVPS(ramBusR + 2, z80P1T - 3, ramP1T + 1, 16); SHPS(ramP21L + 1, ramBusR + 1, ramP1T + 2, 16);
		// d0
		SHPS(dBusL1 + 0, z80P1L - 2, z80P21T - 6, 17); SVPS(dBusL1 + 0, z80P21T - 6, dBusT1 + 6, 17);
		SHPS(dBusL1 + 0, dBusL2 + 6, dBusT1 + 7, 17); SVPS(dBusL2 + 7, dBusT2 + 7, dBusT1 + 6, 17);
		SHPS(dBusL2 + 7, ramBusL - 10, dBusT2 + 7, 17);
		SVPS(romBusL - 9, dBusT2 + 7, romP21T - 4, 17); SHPS(romBusL - 9, romP1L - 2, romP21T - 3, 17);
		SVPS(ramBusL - 9, dBusT2 + 7, ramP21T - 4, 17); SHPS(ramBusL - 9, ramP1L - 2, ramP21T - 3, 17);
		// d1
		SHPS(dBusL1 + 1, z80P1L - 2, z80P21T - 5, 18); SVPS(dBusL1 + 1, z80P21T - 5, dBusT1 + 5, 18);
		SHPS(dBusL1 + 1, dBusL2 + 5, dBusT1 + 6, 18); SVPS(dBusL2 + 6, dBusT2 + 6, dBusT1 + 5, 18);
		SHPS(dBusL2 + 6, ramBusL - 11, dBusT2 + 6, 18);
		SVPS(romBusL - 10, dBusT2 + 6, romP21T - 3, 18); SHPS(romBusL - 10, romP1L - 2, romP21T - 2, 18);
		SVPS(ramBusL - 10, dBusT2 + 6, ramP21T - 3, 18); SHPS(ramBusL - 10, ramP1L - 2, ramP21T - 2, 18);
		// d2
		SHPS(dBusL1 + 2, z80P1L - 2, z80P21T - 8, 19); SVPS(dBusL1 + 2, z80P21T - 8, dBusT1 + 4, 19);
		SHPS(dBusL1 + 2, dBusL2 + 4, dBusT1 + 5, 19); SVPS(dBusL2 + 5, dBusT2 + 5, dBusT1 + 4, 19);
		SHPS(dBusL2 + 5, ramBusL - 12, dBusT2 + 5, 19);
		SVPS(romBusL - 11, dBusT2 + 5, romP21T - 2, 19); SHPS(romBusL - 11, romP1L - 2, romP21T - 1, 19);
		SVPS(ramBusL - 11, dBusT2 + 5, ramP21T - 2, 19); SHPS(ramBusL - 11, ramP1L - 2, ramP21T - 1, 19);
		// d3
		SHPS(dBusL1 + 3, z80P1L - 2, z80P21T - 12, 20); SVPS(dBusL1 + 3, z80P21T - 12, dBusT1 + 3, 20);
		SHPS(dBusL1 + 3, dBusL2 + 3, dBusT1 + 4, 20); SVPS(dBusL2 + 4, dBusT2 + 4, dBusT1 + 3, 20);
		SHPS(dBusL2 + 4, ramBusR + 11, dBusT2 + 4, 20);
		SVPS(romBusR + 12, dBusT2 + 4, romP21T - 1, 20); SHPS(romP21L + 1, romBusR + 11, romP21T, 20);
		SVPS(ramBusR + 12, dBusT2 + 4, ramP21T - 1, 20); SHPS(ramP21L + 1, ramBusR + 11, ramP21T, 20);
		// d4
		SHPS(dBusL1 + 4, z80P1L - 2, z80P21T - 13, 21); SVPS(dBusL1 + 4, z80P21T - 13, dBusT1 + 2, 21);
		SHPS(dBusL1 + 4, dBusL2 + 2, dBusT1 + 3, 21); SVPS(dBusL2 + 3, dBusT2 + 3, dBusT1 + 2, 21);
		SHPS(dBusL2 + 3, ramBusR + 10, dBusT2 + 3, 21);
		SVPS(romBusR + 11, dBusT2 + 3, romP21T - 2, 21); SHPS(romP21L + 1, romBusR + 10, romP21T - 1, 21);
		SVPS(ramBusR + 11, dBusT2 + 3, ramP21T - 2, 21); SHPS(ramP21L + 1, ramBusR + 10, ramP21T - 1, 21);
		// d5
		SHPS(dBusL1 + 5, z80P1L - 2, z80P21T - 11, 22); SVPS(dBusL1 + 5, z80P21T - 11, dBusT1 + 1, 22);
		SHPS(dBusL1 + 5, dBusL2 + 1, dBusT1 + 2, 22); SVPS(dBusL2 + 2, dBusT2 + 2, dBusT1 + 1, 22);
		SHPS(dBusL2 + 2, ramBusR + 9, dBusT2 + 2, 22);
		SVPS(romBusR + 10, dBusT2 + 2, romP21T - 3, 22); SHPS(romP21L + 1, romBusR + 9, romP21T - 2, 22);
		SVPS(ramBusR + 10, dBusT2 + 2, ramP21T - 3, 22); SHPS(ramP21L + 1, ramBusR + 9, ramP21T - 2, 22);
		// d6
		SHPS(dBusL1 + 6, z80P1L - 2, z80P21T - 10, 23); SVPS(dBusL1 + 6, z80P21T - 10, dBusT1 + 0, 23);
		SHPS(dBusL1 + 6, dBusL2 + 0, dBusT1 + 1, 23); SVPS(dBusL2 + 1, dBusT2 + 1, dBusT1 + 0, 23);
		SHPS(dBusL2 + 1, ramBusR + 8, dBusT2 + 1, 23);
		SVPS(romBusR + 9, dBusT2 + 1, romP21T - 4, 23); SHPS(romP21L + 1, romBusR + 8, romP21T - 3, 23);
		SVPS(ramBusR + 9, dBusT2 + 1, ramP21T - 4, 23); SHPS(ramP21L + 1, ramBusR + 8, ramP21T - 3, 23);
		// d7
		SHPS(dBusL1 + 7, z80P1L - 2, z80P21T - 7, 24); SVPS(dBusL1 + 7, z80P21T - 7, dBusT1 - 1, 24);
		SHPS(dBusL1 + 7, dBusL2 - 1, dBusT1 + 0, 24); SVPS(dBusL2 + 0, dBusT2 + 0, dBusT1 - 1, 24);
		SHPS(dBusL2 + 0, ramBusR + 7, dBusT2 + 0, 24);
		SVPS(romBusR + 8, dBusT2 + 0, romP21T - 5, 24); SHPS(romP21L + 1, romBusR + 7, romP21T - 4, 24);
		SVPS(ramBusR + 8, dBusT2 + 0, ramP21T - 5, 24); SHPS(ramP21L + 1, ramBusR + 7, ramP21T - 4, 24);
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

	void renderZ80() {
		int bl = vec2(Z80_TOP_LEFT).x + vec2(CIRCUIT_TOP_LEFT).x;
		int br = bl + vec2(Z80_SIZE).x;
		int bt = vec2(Z80_TOP_LEFT).y + vec2(CIRCUIT_TOP_LEFT).y;
		int bb = bt + vec2(Z80_SIZE).y;

		// corners
		wireSprite.pos = vec2(bl, bt) * vec2(8, 12); wireSprite.render(&rp, &rb, 3, 0);
		wireSprite.pos = vec2(br, bt) * vec2(8, 12); wireSprite.render(&rp, &rb, 4, 0);
		wireSprite.pos = vec2(bl, bb) * vec2(8, 12); wireSprite.render(&rp, &rb, 5, 0);
		wireSprite.pos = vec2(br, bb) * vec2(8, 12); wireSprite.render(&rp, &rb, 6, 0);

		// top and bottom
		for(int i = 0; i < br - bl - 1; i++) {
			wireSprite.pos = (vec2(i, 0) + vec2(bl + 1, bt)) * vec2(8, 12);

			if(i == Z80_MIDDLE) {wireSprite.render(&rp, &rb, 11, 3);} // notch
			else {wireSprite.render(&rp, &rb, 1, 0);}
			wireSprite.pos = (vec2(i, 0) + vec2(bl + 1, bb)) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
		}

		// sides
		for(int i = 0; i < bb - bt - 1; i++) {
			wireSprite.pos = (vec2(0, i) + vec2(bl, bt + 1)) * vec2(8, 12); wireSprite.render(&rp, &rb, 8, 0);

			wireSprite.pos = (vec2(0, i) + vec2(br, bt + 1)) * vec2(8, 12); wireSprite.render(&rp, &rb, 7, 0);
		}

		renderString(fontSprite, vec2(bl, bt + 1) * vec2(8, 12), 
			" A11       A10\n"
			" A12        A9\n"
			" A13        A8\n"
			" A14        A7\n"
			" A15        A6\n"
			" CLK        A5\n"
			" D4         A4\n"
			" D3         A3\n"
			" D5         A2\n"
			" D6         A1\n"
			" +5V        A0\n"
			" D2        GND\n"
			" D7      /RFSH\n"
			" D0        /M1\n"
			" D1     /RESET\n"
			" /INT   /BUSRQ\n"
			" /NMI    /WAIT\n"
			" /HALT /BUSACK\n"
			" /MREQ     /WR\n"
			" /IORQ     /RD\n"
			"\n"
			"Z80"
		, &rp, &rb);

		// indicators
		INDICATE(bl - 1, bt +  1, 14); INDICATE(br + 1, bt +  1, 13);
		INDICATE(bl - 1, bt +  2, 15); INDICATE(br + 1, bt +  2, 12);
		INDICATE(bl - 1, bt +  3, 16); INDICATE(br + 1, bt +  3, 11);
		                               INDICATE(br + 1, bt +  4, 10);
		                               INDICATE(br + 1, bt +  5,  9);
		INDICATE(bl - 1, bt +  6,  0); INDICATE(br + 1, bt +  6,  8);
		INDICATE(bl - 1, bt +  7, 21); INDICATE(br + 1, bt +  7,  7);
		INDICATE(bl - 1, bt +  8, 20); INDICATE(br + 1, bt +  8,  6);
		INDICATE(bl - 1, bt +  9, 22); INDICATE(br + 1, bt +  9,  5);
		INDICATE(bl - 1, bt + 10, 23); INDICATE(br + 1, bt + 10,  4);
		                               INDICATE(br + 1, bt + 11,  3);
		INDICATE(bl - 1, bt + 12, 19); 
		INDICATE(bl - 1, bt + 13, 24); 
		INDICATE(bl - 1, bt + 14, 17); 
		INDICATE(bl - 1, bt + 15, 18); 
		                               
		                               
		                               
		                               INDICATE(br + 1, bt + 19,  2);
		                               INDICATE(br + 1, bt + 20,  1);
	}

	void renderROM () {
		int bl = vec2(ROM_TOP_LEFT).x + vec2(CIRCUIT_TOP_LEFT).x;
		int br = bl + vec2(MEM_IC_SIZE).x;
		int bt = vec2(ROM_TOP_LEFT).y + vec2(CIRCUIT_TOP_LEFT).y;
		int bb = bt + vec2(MEM_IC_SIZE).y;

		// corners
		wireSprite.pos = vec2(bl, bt) * vec2(8, 12); wireSprite.render(&rp, &rb, 3, 0);
		wireSprite.pos = vec2(br, bt) * vec2(8, 12); wireSprite.render(&rp, &rb, 4, 0);
		wireSprite.pos = vec2(bl, bb) * vec2(8, 12); wireSprite.render(&rp, &rb, 5, 0);
		wireSprite.pos = vec2(br, bb) * vec2(8, 12); wireSprite.render(&rp, &rb, 6, 0);

		// top and bottom
		for(int i = 0; i < br - bl - 1; i++) {
			wireSprite.pos = (vec2(i, 0) + vec2(bl + 1, bt)) * vec2(8, 12);
			if(i == MEM_IC_MIDDLE) {wireSprite.render(&rp, &rb, 11, 3);} // notch
			else {wireSprite.render(&rp, &rb, 1, 0);}

			wireSprite.pos = (vec2(i, 0) + vec2(bl + 1, bb)) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
		}

		// sides
		for(int i = 0; i < bb - bt - 1; i++) {
			wireSprite.pos = (vec2(0, i) + vec2(bl, bt + 1)) * vec2(8, 12);
			if(i != 0) {wireSprite.render(&rp, &rb, 8, 0);}
			else {wireSprite.render(&rp, &rb, 2, 0);}

			wireSprite.pos = (vec2(0, i) + vec2(br, bt + 1)) * vec2(8, 12);
			if(i != 2) {wireSprite.render(&rp, &rb, 7, 0);}
			else {wireSprite.render(&rp, &rb, 2, 0);}
		}

		renderString(fontSprite, vec2(bl, bt + 1) * vec2(8, 12), 
			"       +5V\n"
			" A12   /WE\n"
			" A7       \n"
			" A6     A8\n"
			" A5     A9\n"
			" A4    A11\n"
			" A3    /OE\n"
			" A2    A10\n"
			" A1    /CE\n"
			" A0   I/O7\n"
			" I/O0 I/O6\n"
			" I/O1 I/O5\n"
			" I/O2 I/O4\n"
			" GND  I/O3\n"
			"\n"
			"ROM"
		, &rp, &rb);

		// indicators
		                               
		INDICATE(bl - 1, bt +  2, 15); INDICATE(br + 1, bt +  2,  2);
		INDICATE(bl - 1, bt +  3, 10);
		INDICATE(bl - 1, bt +  4,  9); INDICATE(br + 1, bt +  4, 11);
		INDICATE(bl - 1, bt +  5,  8); INDICATE(br + 1, bt +  5, 12);
		INDICATE(bl - 1, bt +  6,  7); INDICATE(br + 1, bt +  6, 14);
		INDICATE(bl - 1, bt +  7,  6); INDICATE(br + 1, bt +  7,  1);
		INDICATE(bl - 1, bt +  8,  5); INDICATE(br + 1, bt +  8, 13);
		INDICATE(bl - 1, bt +  9,  4); INDICATE(br + 1, bt +  9, 16);
		INDICATE(bl - 1, bt + 10,  3); INDICATE(br + 1, bt + 10, 24);
		INDICATE(bl - 1, bt + 11, 17); INDICATE(br + 1, bt + 11, 23);
		INDICATE(bl - 1, bt + 12, 18); INDICATE(br + 1, bt + 12, 22);
		INDICATE(bl - 1, bt + 13, 19); INDICATE(br + 1, bt + 13, 21);
		                               INDICATE(br + 1, bt + 14, 20);
	}

	void renderRAM() {
		int bl = vec2(RAM_TOP_LEFT).x + vec2(CIRCUIT_TOP_LEFT).x;
		int br = bl + vec2(MEM_IC_SIZE).x;
		int bt = vec2(RAM_TOP_LEFT).y + vec2(CIRCUIT_TOP_LEFT).y;
		int bb = bt + vec2(MEM_IC_SIZE).y;

		// corners
		wireSprite.pos = vec2(bl, bt) * vec2(8, 12); wireSprite.render(&rp, &rb, 3, 0);
		wireSprite.pos = vec2(br, bt) * vec2(8, 12); wireSprite.render(&rp, &rb, 4, 0);
		wireSprite.pos = vec2(bl, bb) * vec2(8, 12); wireSprite.render(&rp, &rb, 5, 0);
		wireSprite.pos = vec2(br, bb) * vec2(8, 12); wireSprite.render(&rp, &rb, 6, 0);

		// top and bottom
		for(int i = 0; i < br - bl - 1; i++) {
			wireSprite.pos = (vec2(i, 0) + vec2(bl + 1, bt)) * vec2(8, 12);
			if(i == MEM_IC_MIDDLE) {wireSprite.render(&rp, &rb, 11, 3);} // notch
			else {wireSprite.render(&rp, &rb, 1, 0);}

			wireSprite.pos = (vec2(i, 0) + vec2(bl + 1, bb)) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);
		}

		// sides
		for(int i = 0; i < bb - bt - 1; i++) {
			wireSprite.pos = (vec2(0, i) + vec2(bl, bt + 1)) * vec2(8, 12);
			if(i != 0) {wireSprite.render(&rp, &rb, 8, 0);}
			else {wireSprite.render(&rp, &rb, 2, 0);}

			wireSprite.pos = (vec2(0, i) + vec2(br, bt + 1)) * vec2(8, 12); wireSprite.render(&rp, &rb, 7, 0);
		}

		renderString(fontSprite, vec2(bl, bt + 1) * vec2(8, 12), 
			"       +5V\n"
			" A12   /WE\n"
			" A7    CS2\n"
			" A6     A8\n"
			" A5     A9\n"
			" A4    A11\n"
			" A3    /OE\n"
			" A2    A10\n"
			" A1   /CS1\n"
			" A0   I/O7\n"
			" I/O0 I/O6\n"
			" I/O1 I/O5\n"
			" I/O2 I/O4\n"
			" GND  I/O3\n"
			"\n"
			"RAM"
		, &rp, &rb);

		// indicators
		                               
		INDICATE(bl - 1, bt +  2, 15); INDICATE(br + 1, bt +  2,  2);
		INDICATE(bl - 1, bt +  3, 10); INDICATE(br + 1, bt +  3, 16);
		INDICATE(bl - 1, bt +  4,  9); INDICATE(br + 1, bt +  4, 11);
		INDICATE(bl - 1, bt +  5,  8); INDICATE(br + 1, bt +  5, 12);
		INDICATE(bl - 1, bt +  6,  7); INDICATE(br + 1, bt +  6, 14);
		INDICATE(bl - 1, bt +  7,  6); INDICATE(br + 1, bt +  7,  1);
		INDICATE(bl - 1, bt +  8,  5); INDICATE(br + 1, bt +  8, 13);
		INDICATE(bl - 1, bt +  9,  4);
		INDICATE(bl - 1, bt + 10,  3); INDICATE(br + 1, bt + 10, 24);
		INDICATE(bl - 1, bt + 11, 17); INDICATE(br + 1, bt + 11, 23);
		INDICATE(bl - 1, bt + 12, 18); INDICATE(br + 1, bt + 12, 22);
		INDICATE(bl - 1, bt + 13, 19); INDICATE(br + 1, bt + 13, 21);
		                               INDICATE(br + 1, bt + 14, 20);
	}

	void renderClock() {
		int bl = vec2(CLOCK_TOP_LEFT).x + vec2(CIRCUIT_TOP_LEFT).x;
		int br = bl + vec2(CLOCK_SIZE).x;
		int bt = vec2(CLOCK_TOP_LEFT).y + vec2(CIRCUIT_TOP_LEFT).y;
		int bb = bt + vec2(CLOCK_SIZE).y;

		// corners
		wireSprite.pos = vec2(bl, bt) * vec2(8, 12); wireSprite.render(&rp, &rb, 3, 0);
		wireSprite.pos = vec2(br, bt) * vec2(8, 12); wireSprite.render(&rp, &rb, 4, 0);
		wireSprite.pos = vec2(bl, bb) * vec2(8, 12); wireSprite.render(&rp, &rb, 5, 0);
		wireSprite.pos = vec2(br, bb) * vec2(8, 12); wireSprite.render(&rp, &rb, 6, 0);

		// top and bottom
		for(int i = 0; i < br - bl - 1; i++) {
			wireSprite.pos = vec2(bl + 1 + i, bt) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 0);

			wireSprite.pos = vec2(bl + 1 + i, bb) * vec2(8, 12);
			if(i == CLOCK_MIDDLE) {wireSprite.render(&rp, &rb, 9, 0);} // pin
			else {wireSprite.render(&rp, &rb, 1, 0);}
		}

		// sides
		for(int i = 0; i < bb - bt - 1; i++) {
			wireSprite.pos = vec2(bl, bt + 1 + i) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);

			wireSprite.pos = vec2(br, bt + 1 + i) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 0);
		}

		// indicator
		if(circuit.netValues[0] == 2) {
			for(int y = 0; y < bb - bt - 3; y++) {
				for(int x = 0; x < br - bl - 1; x++) {
					wireSprite.pos = vec2(bl + 1 + x, bt + 1 + y) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 1);
				}
			}
		}
		if(circuit.netValues[0] == 3) {
			for(int y = 0; y < bb - bt - 3; y++) {
				for(int x = 0; x < br - bl - 1; x++) {
					wireSprite.pos = vec2(bl + 1 + x, bt + 3 + y) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 2);
				}
			}
		}
	}

	void renderVCCs() {
		int v1l = vec2(VCC_1_TOP_LEFT).x;
		int v1t = vec2(VCC_1_TOP_LEFT).y;
		int v2l = vec2(VCC_2_TOP_LEFT).x;
		int v2t = vec2(VCC_2_TOP_LEFT).y;
		int v3l = vec2(VCC_3_TOP_LEFT).x;
		int v3t = vec2(VCC_3_TOP_LEFT).y;
		int v4l = vec2(VCC_4_TOP_LEFT).x;
		int v4t = vec2(VCC_4_TOP_LEFT).y;

		renderString(fontSprite, vec2(v1l, v1t) * vec2(8, 12), "VCC", &rp, &rb);
		renderString(fontSprite, vec2(v2l, v2t) * vec2(8, 12), "VCC", &rp, &rb);
		renderString(fontSprite, vec2(v3l, v3t) * vec2(8, 12), "VCC", &rp, &rb);
		renderString(fontSprite, vec2(v4l, v4t) * vec2(8, 12), "VCC", &rp, &rb);

		// underlines
		wireSprite.pos = vec2(v1l, v1t + 1) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);
		wireSprite.pos = vec2(v1l + 1, v1t + 1) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 3);
		wireSprite.pos = vec2(v1l + 2, v1t + 1) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);

		wireSprite.pos = vec2(v2l, v2t + 1) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);
		wireSprite.pos = vec2(v2l + 1, v2t + 1) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 3);
		wireSprite.pos = vec2(v2l + 2, v2t + 1) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);

		wireSprite.pos = vec2(v3l, v3t + 1) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);
		wireSprite.pos = vec2(v3l + 1, v3t + 1) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 3);
		wireSprite.pos = vec2(v3l + 2, v3t + 1) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);

		wireSprite.pos = vec2(v4l, v4t + 1) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);
		wireSprite.pos = vec2(v4l + 1, v4t + 1) * vec2(8, 12); wireSprite.render(&rp, &rb, 1, 3);
		wireSprite.pos = vec2(v4l + 2, v4t + 1) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 3);
	}

	void renderGNDs() {
		wireSprite.pos = vec2(GND_1_POS) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 3);
		wireSprite.pos = vec2(GND_2_POS) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 3);
		wireSprite.pos = vec2(GND_3_POS) * vec2(8, 12); wireSprite.render(&rp, &rb, 2, 3);
	}

	void renderCircuit() {
		renderZ80();
		renderROM();
		renderRAM();
		//renderClock();
		//renderVCCs();
		//renderGNDs();

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

		/*// clock border
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
		if(circuit.netValues[0] == 2) {
			wireSprite.pos = vec2(21, 14) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 1);
			wireSprite.pos = vec2(22, 14) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 1);
			wireSprite.pos = vec2(23, 14) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 1);
			wireSprite.pos = vec2(21, 15) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 1);
			wireSprite.pos = vec2(22, 15) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 1);
			wireSprite.pos = vec2(23, 15) * vec2(8, 12); wireSprite.render(&rp, &rb, 0, 1);
		}
		if(circuit.netValues[0] == 3) {
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
		}*/
	}

	void renderHistogram(Sprite wireSprite, vec2 leftCenter, bool* history, RenderProgram* rp, RenderBuffer* rb) {
		for(int i = 0; i < HISTORY_LENGTH; i++) {
			if(history[i]) {
				wireSprite.pos = leftCenter + vec2(i * 24, -12);
				wireSprite.render(rp, rb, 1, 1);
				wireSprite.pos = leftCenter + vec2(i * 24 + 8, -12);
				wireSprite.render(rp, rb, 1, 1);
			}
			else {
				wireSprite.pos = leftCenter + vec2(i * 24, 12);
				wireSprite.render(rp, rb, 1, 2);
				wireSprite.pos = leftCenter + vec2(i * 24 + 8, 12);
				wireSprite.render(rp, rb, 1, 2);
			}

			if(i != HISTORY_LENGTH - 1) {
				if(history[i] && history[i + 1]) {
					wireSprite.pos = leftCenter + vec2(i * 24 + 16, -12);
					wireSprite.render(rp, rb, 1, 1);
				}
				if(!history[i] && !history[i + 1]) {
					wireSprite.pos = leftCenter + vec2(i * 24 + 16, 12);
					wireSprite.render(rp, rb, 1, 2);
				}
				if(history[i] && !history[i + 1]) {
					wireSprite.pos = leftCenter + vec2(i * 24 + 16, -12);
					wireSprite.render(rp, rb, 4, 1);
					wireSprite.pos = leftCenter + vec2(i * 24 + 16, 0);
					wireSprite.render(rp, rb, 2, 0);
					wireSprite.pos = leftCenter + vec2(i * 24 + 16, 12);
					wireSprite.render(rp, rb, 5, 2);
				}
				if(!history[i] && history[i + 1]) {
					wireSprite.pos = leftCenter + vec2(i * 24 + 16, -12);
					wireSprite.render(rp, rb, 3, 1);
					wireSprite.pos = leftCenter + vec2(i * 24 + 16, 0);
					wireSprite.render(rp, rb, 2, 0);
					wireSprite.pos = leftCenter + vec2(i * 24 + 16, 12);
					wireSprite.render(rp, rb, 6, 2);
				}
			}
		}
	}

	string decodeInstruction(int instruction) {
		string output = "";

		if(instruction == 0x00) {return "No Operation";}

		if(
			instruction >= 0x00 && instruction <= 0x3F && (
				instruction % 16 == 1 ||
				instruction % 16 == 6 && instruction != 0x36 ||
				instruction % 16 == 10 ||
				instruction % 16 == 14
			) ||
			instruction >= 0x40 && instruction <= 0x6F ||
			instruction >= 0x78 && instruction <= 0x7F ||
			instruction == 0xF9
		) {
			output += "Load ";

			if(instruction >= 0x00 && instruction <= 0x3F) {
				if(instruction % 8 == 6) {output += "Immediate Byte Into ";}
				if(instruction % 16 == 1) {output += "Immediate Short Into ";}
				if(instruction % 8 == 6) {output += "Indirect Byte Pointed To by HL Into ";}
				if(instruction == 0x0A) {output += "Indirect Byte Pointed To by BC Into ";}
				if(instruction == 0x1A) {output += "Indirect Byte Pointed To by DE Into ";}
				if(instruction == 0x2A) {output += "Indirect Short Pointed To by Immediate Short Into ";}
				if(instruction == 0x3A) {output += "Indirect Byte Pointed To by Immediate Short Into ";}
			}
			if(instruction >= 0x40 && instruction <= 0x7F) {
				if(instruction % 8 == 0) {output += "Contents of B Into ";}
				if(instruction % 8 == 1) {output += "Contents of C Into ";}
				if(instruction % 8 == 2) {output += "Contents of D Into ";}
				if(instruction % 8 == 3) {output += "Contents of E Into ";}
				if(instruction % 8 == 4) {output += "Contents of H Into ";}
				if(instruction % 8 == 5) {output += "Contents of L Into ";}
				if(instruction % 8 == 6) {output += "Indirect Byte Pointed To by HL Into ";}
				if(instruction % 8 == 7) {output += "Contents of Accumulator Into ";}
			}
			if(instruction == 0xF9) {output += "Contents of HL Into ";}

			if(instruction >= 0x00 && instruction <= 0x3F) {
				if(instruction == 0x01) {output += "BC";}
				if(instruction == 0x11) {output += "DE";}
				if(instruction == 0x21 || instruction == 0x2A) {output += "HL";}
				if(instruction == 0x31) {output += "SP";}
				if(instruction == 0x06) {output += "B";}
				if(instruction == 0x0E) {output += "C";}
				if(instruction == 0x16) {output += "D";}
				if(instruction == 0x1E) {output += "E";}
				if(instruction == 0x26) {output += "H";}
				if(instruction == 0x2E) {output += "L";}
				if(instruction == 0x0A ||instruction == 0x1A ||instruction == 0x3A || instruction == 0x3E) {output += "Accumulator";}
			}
			if(instruction >= 0x40 && instruction <= 0x7F) {
				if(instruction >= 0x40 && instruction <= 0x47) {output += "B";}
				if(instruction >= 0x48 && instruction <= 0x4F) {output += "C";}
				if(instruction >= 0x50 && instruction <= 0x57) {output += "D";}
				if(instruction >= 0x58 && instruction <= 0x5F) {output += "E";}
				if(instruction >= 0x60 && instruction <= 0x67) {output += "H";}
				if(instruction >= 0x68 && instruction <= 0x6F) {output += "L";}
				if(instruction >= 0x78 && instruction <= 0x7F) {output += "Accumulator";}
			}
			if(instruction == 0xF9) {output += "SP";}
		}
		if(
			instruction >= 0x00 && instruction <= 0x3F && (instruction % 16 == 2) || instruction == 0x36 ||
			instruction >= 0x70 && instruction <= 0x77 && instruction != 0x76
		) {
			output += "Store ";

			if(instruction >= 0x00 && instruction <= 0x3F) {
				if(instruction == 0x02 || instruction == 0x12 || instruction == 0x32) {output += "Contents of Accumulator To ";}
				if(instruction == 0x22) {output += "Contents of HL To ";}
				if(instruction == 0x36) {output += "Immediate Byte To ";}
			}
			if(instruction >= 0x70 && instruction <= 0x77) {
				if(instruction == 0x70) {output += "Contents of B To ";}
				if(instruction == 0x71) {output += "Contents of C To ";}
				if(instruction == 0x72) {output += "Contents of D To ";}
				if(instruction == 0x73) {output += "Contents of E To ";}
				if(instruction == 0x74) {output += "Contents of H To ";}
				if(instruction == 0x75) {output += "Contents of L To ";}
				if(instruction == 0x77) {output += "Contents of Accumulator To ";}
			}

			if(instruction >= 0x70 && instruction <= 0x77) {output += "Indirect Byte Pointed To by HL";}
			if(instruction >= 0x00 && instruction <= 0x3F) {
				if(instruction == 0x02) {output += "Indirect Byte Pointed To by BC";}
				if(instruction == 0x12) {output += "Indirect Byte Pointed To by DE";}
				if(instruction == 0x22 || instruction == 0x32) {output += "Indirect Byte Pointed To by Immediate Short";}
				if(instruction == 0x36) {output += "Indirect Byte Pointed To by HL";}
			}
		}

		if(instruction >= 0x00 && instruction <= 0x3F && (instruction % 8 == 4 || instruction % 16 == 3)) {
			output += "Increment ";

			if(instruction == 0x03) {output += "BC";}
			if(instruction == 0x13) {output += "DE";}
			if(instruction == 0x23) {output += "HL";}
			if(instruction == 0x33) {output += "SP";}
			if(instruction == 0x04) {output += "B";}
			if(instruction == 0x14) {output += "D";}
			if(instruction == 0x24) {output += "H";}
			if(instruction == 0x34) {output += "Indirect Byte Pointed To by HL";}
			if(instruction == 0x0C) {output += "C";}
			if(instruction == 0x1C) {output += "E";}
			if(instruction == 0x2C) {output += "L";}
			if(instruction == 0x3C) {output += "Accumulator";}
		}
		if(instruction >= 0x00 && instruction <= 0x3F && (instruction % 8 == 5 || instruction % 16 == 13)) {
			output += "Decrement ";

			if(instruction == 0x05) {output += "B";}
			if(instruction == 0x15) {output += "D";}
			if(instruction == 0x25) {output += "H";}
			if(instruction == 0x35) {output += "Indirect Byte Pointed To by HL";}
			if(instruction == 0x0B) {output += "BC";}
			if(instruction == 0x1B) {output += "DE";}
			if(instruction == 0x2B) {output += "HL";}
			if(instruction == 0x3B) {output += "SP";}
			if(instruction == 0x0D) {output += "C";}
			if(instruction == 0x1D) {output += "E";}
			if(instruction == 0x2D) {output += "L";}
			if(instruction == 0x3D) {output += "Accumulator";}
		}

		return output;
	}

	string decodeMCycles(int instruction) {
		if(instructionMCycles[instruction] == M_F) {return "(1): Fetch";}
		if(instructionMCycles[instruction] == M_FR) {return "(2): Fetch, Read";}
		if(instructionMCycles[instruction] == M_FW) {return "(2): Fetch, Write";}
		if(instructionMCycles[instruction] == M_FRR) {return "(3): Fetch, Read, Read";}
		if(instructionMCycles[instruction] == M_FRW) {return "(3): Fetch, Read, Write";}
		if(instructionMCycles[instruction] == M_FTR) {return "(4): Fetch, Read, Read, Read";}
		if(instructionMCycles[instruction] == M_FQR) {return "(5): Fetch, Read, Read, Read, Read";}
	}
	string decodeTCycles(int instruction) {
		if(instructionMCycles[instruction] == M_F) {return "(4): 4";}
		if(instructionMCycles[instruction] == M_FR) {return "(7): 4, 3";}
		if(instructionMCycles[instruction] == M_FW) {return "(7): 4, 3";}
		if(instructionMCycles[instruction] == M_FRR) {return "(10): 4, 3, 3";}
		if(instructionMCycles[instruction] == M_FRW) {return "(10): 4, 3, 3";}
		if(instructionMCycles[instruction] == M_FTR) {return "(13): 4, 3, 3, 3";}
		if(instructionMCycles[instruction] == M_FQR) {return "(16): 4, 3, 3, 3, 3";}
	}
};

#endif