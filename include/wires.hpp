#ifndef WIRES_H
#define WIRES_H

#include <list>
#include <cstdint>
#include <cstring>

#define WORLD_WIDTH 160
#define WORLD_HEIGHT 60
#define H_WIDTH (WORLD_WIDTH - 1)
#define H_HEIGHT WORLD_HEIGHT
#define V_WIDTH WORLD_WIDTH
#define V_HEIGHT (WORLD_HEIGHT - 1)
#define H_SIZE (H_WIDTH * H_HEIGHT)
#define V_SIZE (V_WIDTH * V_HEIGHT)

using namespace std;

class WireTable {
public:
	uint8_t* hWireTable[H_SIZE];
	uint8_t* vWireTable[V_SIZE];

	void init() {
		memset(hWireTable, 0, H_SIZE * sizeof(uint8_t*));
		memset(vWireTable, 0, V_SIZE * sizeof(uint8_t*));
	}

	int getHValue(int x, int y) {
		if(hWireTable[x + y * (H_WIDTH)] == 0) {return 0;}
		return *hWireTable[x + y * (H_WIDTH)];
	}
	int getVValue(int x, int y) {
		if(vWireTable[x + y * (V_WIDTH)] == 0) {return 0;}
		return *vWireTable[x + y * (V_WIDTH)];
	}

	void setHPointer(int x, int y, uint8_t* pointer) {hWireTable[x + y * (H_WIDTH)] = pointer;}
	void setVPointer(int x, int y, uint8_t* pointer) {vWireTable[x + y * (V_WIDTH)] = pointer;}
};

class Wire {
public:
	int x;
	int y;
	int state;


	/*int* hLocations[320];
	int* vLocations[120];
	int state;

	void init() {
		memset(hLocations, 0, 320 * sizeof(int*));
		memset(vLocations, 0, 120 * sizeof(int*));
		state = 0;
	}
	void update() {
		for(int i = 0; i < 320; i++) {*(hLocations[i]) = state;}
		for(int i = 0; i < 120; i++) {*(vLocations[i]) = state;}
	}

	int findSpace(int** locations, int listSize, int wantWidth) {
		int index = -1;
		int size = 0;

		for(int i = 0; i < listSize; i++) {
			if(locations[i] == 0) {
				if(index == -1) {index = i;}
				size++;
				if(size == wantWidth) {break;}
			}
			else {
				index = -1;
				size = 0;
			}
		}

		return index;
	}*/
	
	/*void placeHLine(int** wireTable, int tableSize, int tableWidth, int x, int y, int w, bool left, bool right) {
		int start = x + y * tableWidth + 1;
		int index = findSpace(hLocations, 320, w);

		for(int i = 0; i < w; i++) {
			hLocations[index + i] = wireTable
		}
	}*/
	
};

#endif